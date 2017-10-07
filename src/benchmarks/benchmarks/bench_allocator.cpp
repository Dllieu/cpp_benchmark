#include <benchmark/benchmark.h>

#include <experimental/memory_resource>

namespace pmr = std::experimental::fundamentals_v2::pmr;

namespace
{
    static void bench_pmr(benchmark::State& state)
    {
        while (state.KeepRunning())
        {
            std::string empty_string;
            benchmark::DoNotOptimize(empty_string);
        }
    }
}

BENCHMARK(bench_pmr);
