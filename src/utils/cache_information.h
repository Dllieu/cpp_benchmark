#pragma once

#include "utils.h"

namespace cache
{
    constexpr const auto PAGE_SIZE = 4'096;
    constexpr auto operator""_KB( unsigned long long int s ) { return s * 1'024; }
    constexpr auto operator""_MB( unsigned long long int s ) { return s * 1'024 * 1'000; }

    // Max number of segment in L1 = 32KB / 64 = 512
    enum class CacheSize
    {
        L1 = 32_KB,
        L2 = 256_KB,
        L3 = 6_MB,
        DRAM
    };

    template < typename T >
    CacheSize   byteToAppropriateCacheSize( std::size_t numberElements )
    {
        auto byteSize = numberElements * sizeof( T );
        if ( byteSize < utils::enum_cast( CacheSize::L1 ) )
            return CacheSize::L1;

        if ( byteSize < utils::enum_cast( CacheSize::L2 ) )
            return CacheSize::L2;

        if ( byteSize < utils::enum_cast( CacheSize::L3 ) )
            return CacheSize::L3;

        return CacheSize::DRAM;
    }

    const char* to_string( CacheSize cacheSize );
}
