#include <benchmark/benchmark.h>
#include <cassert>
#include <unistd.h>
#include <utils/cache_information.h>

namespace
{
    void CacheAssociativity_CacheAssociativity(benchmark::State& iState)
    {
        std::vector<std::int8_t> dynamicContiguousArray(32_MB);
        std::size_t step = iState.range(0);

        for ([[maybe_unused]] auto handler : iState)
        {
            for (std::size_t repeat = 0, i = 0; repeat < 1'000; ++repeat)
            {
                ++dynamicContiguousArray[i];
                i = (i + step) % 32_MB;
            }
        }
    }

    void BenchmarkArguments(benchmark::internal::Benchmark* iBenchmark)
    {
        std::size_t cacheLineSize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
        assert(64u == cacheLineSize); // NOLINT

        for (std::size_t i = cacheLineSize; i <= cacheLineSize * 66; i += cacheLineSize)
        {
            iBenchmark->Arg(i);
        }
    }
}

// All the spike comes from power of 2 steps
// Cpu cache are (most likely) using two ways set associativity, for instance:
// - 128 KB L2 cache
// - 8 way associativity (8 sets with each set being 16 KB)
// When storing the cache line corresponding to the value we are looping on, we will use that value address
// if the address are a multiple of 16 appart, all those values will use the same slot position in the set
// although there's 8 set of 16 KB, if the address is a multiple of 2, we will only be able to store the cache line in a single slot per set (so only 8 cache line can be stored)
// That's the worst situation possible as it will inccur massive cache misses
BENCHMARK(CacheAssociativity_CacheAssociativity)->Apply(BenchmarkArguments); // NOLINT
