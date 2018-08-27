#include <algorithm>
#include <benchmark/benchmark.h>
#include <list>
#include <numeric>
#include <random>
#include <utils/cache_information.h>
#include <vector>

// |oooooooooooooooooooo|-----------------------|
// | current cache line | prefetched cache line |
// Two aspects to watch out for
// - Locality
// - Prefetching
// Hardware speculatively prefetches cache lines, thus can generally prefetch for you as long as the access is consistent (i.e. i++ or i += 4 or even i--)
// - std::vector / std::array excels at both
// - std::list sequentially allocated nodes provide some sort of non-guaranteed locality
// - shuffled nodes is the worst scenario
namespace
{
    template <typename T>
    T MakeContainerWithRandomValues(std::size_t iSize)
    {
        std::uniform_int_distribution<> rnd(0, static_cast<int>(iSize) - 1);
        std::mt19937 gen;

        T t(iSize);
        std::generate(std::begin(t), std::end(t), [&rnd, &gen] { return rnd(gen); });

        return t;
    }

    void CacheContainerTraversal_VectorBenchmark(benchmark::State& state)
    {
        auto v = MakeContainerWithRandomValues<std::vector<std::int8_t>>(state.range(0));

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : state)
        {
            benchmark::DoNotOptimize(n += std::accumulate(std::cbegin(v), std::cend(v), 0));
        }

        benchmark::DoNotOptimize(n);
    }

    void CacheContainerTraversal_ListBenchmark(benchmark::State& state)
    {
        auto l = MakeContainerWithRandomValues<std::list<std::int8_t>>(state.range(0));

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : state)
        {
            benchmark::DoNotOptimize(n += std::accumulate(std::cbegin(l), std::cend(l), 0));
        }

        benchmark::DoNotOptimize(n);
    }

    void CacheContainerTraversal_ListShuffleBenchmark(benchmark::State& state)
    {
        auto sl = MakeContainerWithRandomValues<std::list<std::int8_t>>(state.range(0));
        sl.sort();

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : state)
        {
            benchmark::DoNotOptimize(n += std::accumulate(std::cbegin(sl), std::cend(sl), 0));
        }

        benchmark::DoNotOptimize(n);
    }
}

BENCHMARK(CacheContainerTraversal_VectorBenchmark)->Range(2_KB, 8_MB);      // NOLINT
BENCHMARK(CacheContainerTraversal_ListBenchmark)->Range(2_KB, 8_MB);        // NOLINT
BENCHMARK(CacheContainerTraversal_ListShuffleBenchmark)->Range(2_KB, 8_MB); // NOLINT
