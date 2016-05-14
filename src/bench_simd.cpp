#include <benchmark/benchmark.h>

#include <emmintrin.h>
#include <random>
#include <algorithm>
#include <array>

namespace
{
    struct Interval
    {
        Interval()
        {
            for ( size_t i = 0; i < sizeof( *this ) / sizeof( int ); ++i )
                reinterpret_cast< uint32_t* >( this )[ i ] = std::numeric_limits< int >::max();
        }

        /*
         * @ Return the boundLevel index for which boundLevel[ index ] <= value < boundLevel[ index + 1 ]
         */
        int     inBoundIndexNoSimd( int value ) const
        {
            auto it = std::find_if( boundLevel.begin(), boundLevel.end(), [ value ]( auto level ) { return value < level; }  );
            return it - boundLevel.begin(); // last element always ensured to be max int
        }

        int     inBoundIndexSimd( int value ) const
        {
            const __m128i abs0 = _mm_set1_epi32( value ); // [ value, value, value, value ]

            // compare [ a0, a1, a2, a3 ] [ b0, b1, b2, b3 ] (if a0 > b0 ? -1 : 0) --> [ r0, r1, r2, r3 ]
            const __m128i cmpFirstLevel  = _mm_cmpgt_epi32( *reinterpret_cast< const __m128i* >( boundLevel.data() ), abs0 );
            const __m128i cmpSecondLevel = _mm_cmpgt_epi32( *( reinterpret_cast< const __m128i* >( boundLevel.data() ) + 1 ), abs0 );

            // if any of the four ints in boundLevel is less than its corresponding int in abs0 -> return the most significant bit (bit position in a binary number having the greatest value)
            const uint32_t bits0 = _mm_movemask_ps( reinterpret_cast< const __m128& >( cmpFirstLevel ) );
            const uint32_t bits1 = _mm_movemask_ps( reinterpret_cast< const __m128& >( cmpSecondLevel ) );

            // returns 1 + the index of the least significant 1-bit of x, or if x is zero, returns zero
            return __builtin_ffs( bits0 | ( bits1 << 4 ) ) - 1; // -1 meaning not found
        }

        std::array< int, 8 > boundLevel;
    };
}

namespace
{
    // Close level difference for easier distribution fairness
    const std::vector< int > LEVEL_LAYER = {
        {  0 }, // -infinite to 0
        {  5 },
        { 10 },
        { 15 },
        { 25 },
        { 30 },
        { 42 },
    };

    auto    generate_inputs()
    {
        std::random_device                          rd;
        std::mt19937                                gen( rd() );
        std::uniform_int_distribution< int >        rnd( 0, 50 );

        std::array< int, std::numeric_limits< uint8_t >::max() > inputs;
        std::generate( inputs.begin(), inputs.end(), [ &rnd, &gen ]{ return rnd( gen ); } );

        return inputs;
    }

    template < typename F >
    void    start_bench_in_bound( benchmark::State& state, F&& f )
    {
        Interval interval;
        for ( size_t i = 1; i < LEVEL_LAYER.size(); ++i )
            interval.boundLevel[ i - 1 ] = LEVEL_LAYER[ i ];

        auto inputs = generate_inputs();
        uint8_t i = 0; // to not use modulo during the bench when iterating
        while ( state.KeepRunning() )
        {
            auto input = inputs[ i++ ];
            benchmark::DoNotOptimize( f( interval, input ) );
        }
    }

    void    bench_simd_in_bound_no_simd( benchmark::State& state )
    {
        start_bench_in_bound( state, []( Interval& interval, int input ) { return interval.inBoundIndexNoSimd( input ); } );
    }

    void    bench_simd_in_bound( benchmark::State& state )
    {
        start_bench_in_bound( state, []( Interval& interval, int input ) { return interval.inBoundIndexSimd( input ); } );
    }
}

BENCHMARK( bench_simd_in_bound_no_simd );
BENCHMARK( bench_simd_in_bound );

// TODO: test with _mm_stream_si128