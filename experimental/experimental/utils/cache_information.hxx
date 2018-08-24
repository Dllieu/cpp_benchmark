#include <utils/cache_information.h>
#include <utils/traits.h>
#include <utils/macros.h>

namespace experimental
{
    template <typename T>
    force_inline CacheSize   byteToAppropriateCacheSize(std::size_t numberElements)
    {
        auto byteSize = numberElements * sizeof( T );

        if ( byteSize <= experimental::enum_cast( CacheSize::L1 ) )
        {
            return CacheSize::L1;
        }

        if ( byteSize <= experimental::enum_cast( CacheSize::L2 ) )
        {
            return CacheSize::L2;
        }

        if ( byteSize <= experimental::enum_cast( CacheSize::L3 ) )
        {
            return CacheSize::L3;
        }

        return CacheSize::DRAM;
    }
}
