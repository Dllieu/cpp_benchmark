#pragma once

#include <type_traits>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

namespace utils
{
    template < typename ENUM_TYPE >
    constexpr auto enum_cast( ENUM_TYPE v )
    {
        return static_cast< std::underlying_type_t< ENUM_TYPE > >( v );
    }

    constexpr size_t    round_up_to_word( size_t size )
    {
        auto remainder = size % sizeof( size_t ); // sizeof( size_t ) == word size
        if ( ! remainder )
            return size;

        return size + sizeof( size_t ) - remainder;
    }
}
