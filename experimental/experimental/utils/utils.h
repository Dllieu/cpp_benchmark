#pragma once

namespace utils
{
    constexpr std::size_t    round_up_to_word( std::size_t size )
    {
        auto remainder = size % sizeof( std::size_t ); // sizeof( size_t ) == word size

        if ( ! remainder )
            return size;

        return size + sizeof( std::size_t ) - remainder;
    }
}

