#include <utils/cache_information.h>

namespace experimental
{
    const char* to_string( CacheSize cacheSize )
    {
        switch ( cacheSize )
        {
            case CacheSize::L1:
                return "L1";

            case CacheSize::L2:
                return "L2";

            case CacheSize::L3:
                return "L3";

            default:
                return "DRAM";
        }
    }
}
