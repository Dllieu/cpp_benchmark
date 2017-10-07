#pragma once

namespace benchmark
{
    class State;
}

namespace utils
{
    constexpr std::size_t    round_up_to_word( std::size_t size )
    {
        auto remainder = size % sizeof( std::size_t ); // sizeof( size_t ) == word size

        if ( ! remainder )
            return size;

        return size + sizeof( std::size_t ) - remainder;
    }

//    template < typename ELEMENT_SIZE, typename F >
//    void    benchmark_with_cache_miss( std::size_t numberElements, F&& f, benchmark::State& state );
}

#include <utils/utils.hxx>
