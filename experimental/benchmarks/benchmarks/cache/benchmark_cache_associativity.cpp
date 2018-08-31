#include <benchmark/benchmark.h>
#include <cassert>
#include <unistd.h>
#include <utils/cache_information.h>

// TODO: Add comments to explain the result
namespace
{
    void CacheAssociativity_CacheAssociativityBenchmark(benchmark::State& iState)
    {
        std::vector<std::int8_t> v(32_MB);
        std::size_t step = iState.range(0);

        for ([[maybe_unused]] auto handler : iState)
        {
            for (std::size_t repeat = 0, i = 0; repeat < 10000; ++repeat)
            {
                v[i]++;
                i = (i + step) % 32_MB;
            }
        }
    }

    void CacheAssociativity_Arguments(benchmark::internal::Benchmark* iBenchmark)
    {
        std::size_t cacheLineSize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
        assert(64u == cacheLineSize);

        for (std::size_t i = cacheLineSize; i <= cacheLineSize * 66; i += cacheLineSize)
        {
            iBenchmark->Arg(i);
        }
    }
}

BENCHMARK(CacheAssociativity_CacheAssociativityBenchmark)->Apply(CacheAssociativity_Arguments); // NOLINT
