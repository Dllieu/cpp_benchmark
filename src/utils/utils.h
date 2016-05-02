#pragma once

#include <type_traits>

namespace utils
{
    template < typename ENUM_TYPE >
    constexpr auto enum_cast( ENUM_TYPE v )
    {
        return static_cast< std::underlying_type_t< ENUM_TYPE > >( v );
    }

    constexpr std::size_t    round_up_to_word( std::size_t size )
    {
        auto remainder = size % sizeof( std::size_t ); // sizeof( size_t ) == word size
        if ( ! remainder )
            return size;

        return size + sizeof( std::size_t ) - remainder;
    }
}
