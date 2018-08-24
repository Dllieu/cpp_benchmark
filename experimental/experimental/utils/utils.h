#pragma once

#include <utils/macros.h>

namespace experimental
{
    constexpr force_inline std::size_t    round_up_to_word( std::size_t size )
    {
        std::size_t remainder = size % sizeof( std::size_t ); // sizeof( size_t ) == word size

        if ( 0u == remainder )
        {
            return size;
        }

        return size + sizeof( std::size_t ) - remainder;
    }
}
