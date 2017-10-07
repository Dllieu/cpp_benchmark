#include <utils/utils.h>

#include <sstream>
#include <iomanip>
//#include <benchmark/benchmark.h>

#include <utils/cache_information.h>
#include <utils/perf_event.h>

namespace utils
{
//    template < typename ELEMENT_SIZE, typename F >
//    void benchmark_with_cache_miss( std::size_t numberElements, F&& f, benchmark::State& state )
//    {
//        // only the last counter is useful
//        perf::PerfEvent counter( PERF_COUNT_HW_CACHE_MISSES );
//        auto canCount = counter.start();
//
//        while ( state.KeepRunning() )
//            benchmark::DoNotOptimize( f() );
//
//        auto count = counter.stop();
//        std::stringstream ss;
//        ss << "Require " << std::setw( 4 ) << cache::to_string( cache::byteToAppropriateCacheSize< ELEMENT_SIZE >( numberElements ) );
//
//        ss << " | CACHE MISSES: ";
//        if ( canCount && state.iterations() > 0 )
//            ss << ( count / state.iterations() );
//        else
//            ss << "DEACTIVATE";
//
//        // Won't be accurate for node based container
//        state.SetLabel( ss.str() );
//    }
}
