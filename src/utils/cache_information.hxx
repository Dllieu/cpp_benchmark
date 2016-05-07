#include "cache_information.h"

#include "traits.h"

namespace cache
{
    template < typename T >
    CacheSize   byteToAppropriateCacheSize( std::size_t numberElements )
    {
        auto byteSize = numberElements * sizeof( T );
        if ( byteSize <= traits::enum_cast( CacheSize::L1 ) )
            return CacheSize::L1;

        if ( byteSize <= traits::enum_cast( CacheSize::L2 ) )
            return CacheSize::L2;

        if ( byteSize <= traits::enum_cast( CacheSize::L3 ) )
            return CacheSize::L3;

        return CacheSize::DRAM;
    }
}
