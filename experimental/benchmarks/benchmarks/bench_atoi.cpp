#include <benchmark/benchmark.h>

#include <array>
#include <random>
#include <algorithm>
#include <iostream>

#include <utils/macros.h>
#include <utils/utils.h>

// The aim of the benchmark is to compute atoi for a known size string
//  - practical use case: receiving a message from exchange: string size is always fixed / known in advance
//  - can leverage on that property to vectorized the code
//  - make sure StringSize if a multiple of a WORD to leverage on SSE instructions (otherwise optimization of manual atoi will be limited)
namespace
{
    template < size_t SRING_SIZE >
    using StringType = std::array< char, SRING_SIZE >;

    template < size_t STRING_SIZE >
    void    ensure_result_match( const StringType< STRING_SIZE >& );

    template < size_t STRING_SIZE >
    auto    to_string_array( const std::string& s )
    {
        std::array< char, STRING_SIZE > result;
        if ( s.size() >= STRING_SIZE )
        {
            std::copy( s.begin(), s.begin() + STRING_SIZE, result.begin() );
            return result;
        }

        auto arrIt = std::fill_n( result.begin(), result.size() - s.size(), '0' );
        std::copy( s.begin(), s.end(), arrIt );
        return result;
    }

    template < size_t STRING_SIZE >
    struct TFixtureATOI : public benchmark::Fixture
    {
    public:
        TFixtureATOI()
        {
            std::random_device                          rd;
            std::mt19937                                gen( rd() );
            std::uniform_int_distribution< uint32_t>    rnd( 0, std::numeric_limits< uint32_t >::max() );

            std::generate( inputs.begin(), inputs.end(), [ & ] { return to_string_array< STRING_SIZE >( std::to_string( rnd( gen ) ) ); } );

            for ( size_t i = 0; i < inputs.size() && i < 16; ++i )
                ensure_result_match( inputs[i] );
        }

        std::array< StringType< STRING_SIZE >, 1000 > inputs;
    };
}

/*
 * @brief atoi_naive
 */
namespace
{
    template < size_t STRING_SIZE, typename T = uint32_t >
    T __attribute__((noinline)) atoi_naive( const StringType< STRING_SIZE >& s )
    {
        T result = 0;
        for ( size_t i = 0; i < STRING_SIZE; ++i )
            result = result * 10 + ( s[ i ] - '0' ); // computation is dependant of result : no vectorization possible
        return result;
    }
}

/*
 * @brief atoi_vectorization + unroll
 */
namespace
{
    template < typename T >
    constexpr T pow_10( size_t n )
    {
        if ( n > std::numeric_limits< T >::digits10 )
            return 0;

        T res = 1;
        while ( n-- )
            res *= 10;
        return res;
    }

    template < size_t STRING_SIZE, typename T >
    struct Power10Generator
    {
        constexpr Power10Generator() : array()
        {
            for (size_t i = 0; i < STRING_SIZE; ++i)
                array[ STRING_SIZE - 1 - i ] = pow_10< T >( i );
        }

        T array[ STRING_SIZE ]; // can't replace by std::array : operator[] non constexpr
    };

    /*
     * @brief alpha_to_int_less_naive, remark:
     *  - no optimize if
     *      -> notOptimize + inlining
     *      -> input size not known at compile time (if std::string input random generated) (no vectorization possible)
     *      -> size of StringType is not a multiple of WORD (limited to no vectorization)
     *      TODO: Check folly and their compile opti on the array + unroll loop (i.e. power10[s[i]] + power100[s[i+1]] ...)
     */
    template < size_t STRING_SIZE, typename T = uint32_t >
    T __attribute__((noinline))  atoi_vectorization( const StringType< STRING_SIZE >& s )
    {
        static constexpr const auto arrayPower10 = Power10Generator< STRING_SIZE, T >();

        T result = 0;
        for ( size_t i = 0; i < STRING_SIZE; ++i )
            result += arrayPower10.array[ i ] * ( s[ i ] - '0' ); // no dependencies, vectorization friendly

        return result;
    }

    template < size_t STRING_SIZE, typename T = uint32_t >
    T __attribute__((noinline))  atoi_vectorization_unroll( const StringType< STRING_SIZE >& s )
    {
        static constexpr const auto arrayPower10 = Power10Generator< STRING_SIZE, T >();

        T result = 0;
        size_t i = 0;
        for ( ; i + 4 < STRING_SIZE; i += 4 )
        {
            const T v0 = arrayPower10.array[ i ] * ( s[ i ] - '0' );
            const T v1 = arrayPower10.array[ i + 1 ] * ( s[ i + 1 ] - '0' );
            const T v2 = arrayPower10.array[ i + 2 ] * ( s[ i + 2 ] - '0' );
            const T v3 = arrayPower10.array[ i + 3 ] * ( s[ i + 3 ] - '0' );

            result += v0 + v1 + v2 + v3;
        }

        for ( ; i < STRING_SIZE; ++i )
            result += arrayPower10.array[ i ] * ( s[ i ] - '0' );

        return result;
    }
}

/*
 * @brief details tests (lazy to implement explicit functor)
 */
namespace
{
    template < size_t STRING_SIZE >
    void    ensure_result_match( const StringType< STRING_SIZE >& s )
    {
        std::string strRef(s.begin(), s.end());
        strRef += '\0';

        auto resultRef = (uint32_t)atoi(strRef.c_str());
        if ( likely( resultRef == atoi_naive(s)
                     && resultRef == atoi_vectorization(s) )
                     && resultRef == atoi_vectorization_unroll(s) )
            return;

        std::cerr << "Incorrect conversion for input: " << strRef << '\n'
                  << "atoi_naive:                     " << atoi_naive(s) << '\n'
                  << "atoi_vectorization:             " << atoi_vectorization(s) << '\n'
                  << "atoi_vectorization_unroll:      " << atoi_vectorization_unroll(s) << std::endl;
        std::exit(1);
    }

//    auto lambda_array_traversal = [] ( auto& state, const auto& inputs )
//    {
//        size_t i = 0;
//        while ( state.KeepRunning() )
//            __builtin_prefetch(inputs[ i++ % inputs.size() ].data()); // since inputs.size() not so big, the traversal won't have a huge impact
//    };

    auto lambda_atoi = [] ( auto& state, auto& inputs )
    {
        size_t i = 0;
        while (state.KeepRunning())
        {
            auto& input = inputs[ i++ % inputs.size() ];
            input[ input.size() - 1 ] = 0; // Not quite fair but atoi only handle null terminated string (size is evaluated at compile time, but we will need an extra write)
            benchmark::DoNotOptimize( atoi( input.data() ) ); // avoid warning + can't inline atoi anyway
        }
    };

    auto lambda_atoi_naive = [] ( auto& state, const auto& inputs )
    {
        size_t i = 0;
        while (state.KeepRunning())
        {
            auto& input = inputs[ i++ % inputs.size() ];
            benchmark::DoNotOptimize( atoi_naive( input ) );
        }
    };

    auto lambda_atoi_vectorization = [] ( auto& state, const auto& inputs )
    {
        size_t i = 0;
        while (state.KeepRunning())
        {
            auto& input = inputs[ i++ % inputs.size() ];
            benchmark::DoNotOptimize( atoi_vectorization( input ) );
        }
    };

    auto lambda_atoi_vectorization_unroll = [] ( auto& state, const auto& inputs )
    {
        size_t i = 0;
        while (state.KeepRunning())
        {
            auto& input = inputs[ i++ % inputs.size() ];
            benchmark::DoNotOptimize( atoi_vectorization_unroll( input ) );
        }
    };
}

/*
 * @brief declare tests
 */
#define DECLARE_FIXTURE_ATOI( N ) \
    using FixtureATOI_##N = TFixtureATOI< N >; \
    BENCHMARK_F( FixtureATOI_##N, atoi )( benchmark::State& state ) { lambda_atoi( state, inputs ); } \
    BENCHMARK_F( FixtureATOI_##N, atoi_naive )( benchmark::State& state ) { lambda_atoi_naive( state, inputs ); } \
    BENCHMARK_F( FixtureATOI_##N, atoi_vectorization )( benchmark::State& state ) { lambda_atoi_vectorization( state, inputs ); } \
    BENCHMARK_F( FixtureATOI_##N, atoi_vectorization_unroll )( benchmark::State& state ) { lambda_atoi_vectorization_unroll( state, inputs ); }

CALL_MACRO_FOR_EACH(DECLARE_FIXTURE_ATOI, 4, 6, 8, 10, 12, 14, 16, 18, 20, 24, 30, 42, 64);

#undef DECLARE_FIXTURE_ATOI


