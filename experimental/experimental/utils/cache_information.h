#pragma once

#include <utils/user_defined_literals.h>
#include <bits/wordsize.h>
#include <cstddef>

namespace experimental
{
    constexpr const auto PAGE_SIZE = 4'096;
    // In bytes
    constexpr const auto WORD_SIZE = __WORDSIZE / 8;

    // Max number of segment in L1 = 32KB / 64 = 512
    // Specific Intel Core i5-4460
    enum class CacheSize
    {
        L1 = 32_KB,
        L2 = 256_KB,
        L3 = 6_MB,
        DRAM
    };

    const char* to_string( CacheSize cacheSize );

    template < typename T >
    CacheSize   byteToAppropriateCacheSize( std::size_t numberElements );
}

#include <utils/cache_information.hxx>
