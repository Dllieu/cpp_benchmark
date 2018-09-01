#include <algorithm>
#include <benchmark/benchmark.h>
#include <deque>
#include <limits>
#include <list>
#include <numeric>
#include <random>
#include <set>
#include <unordered_set>
#include <utils/cache_information.h>
#include <vector>

// |oooooooooooooooooooo|-----------------------|
// | current cache line | prefetched cache line |
// Two aspects to watch out for
// - Locality
// - Prefetching
// Hardware speculatively prefetches cache lines, thus can generatorerally prefetch for you as long as the access is consistent (i.e. i++ or i += 4 or even i--)
// - std::vector / std::array excels at both
// - std::list sequentially allocated nodes provide some sort of non-guaranteed locality
// - shuffled nodes is the worst scenario
namespace
{
    template <template <typename> typename ContainerT, typename T>
    ContainerT<T> MakeContainerWithRandomValues(std::size_t iSize)
    {
        std::uniform_int_distribution<> randomDistribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        std::mt19937 generator;

        ContainerT<T> container(iSize / sizeof(T));
        std::generate(std::begin(container), std::end(container), [&randomDistribution, &generator] { return randomDistribution(generator); });

        return container;
    }

    template <template <typename> typename ContainerT, typename T>
    ContainerT<T> MakeSetWithRandomValues(std::size_t iSize)
    {
        std::uniform_int_distribution<> randomDistribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        std::mt19937 generator;

        ContainerT<T> container;

        // Quick and dirty
        while (container.size() != iSize / sizeof(T))
        {
            container.emplace(randomDistribution(generator));
        }

        return container;
    }

    template <typename T>
    void StlCacheContainerTraversal_RunBenchmark(benchmark::State& iState, T&& iContainer)
    {
        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            benchmark::DoNotOptimize(n += std::accumulate(std::cbegin(iContainer), std::cend(iContainer), 0));
        }
    }

    void StlCacheContainerTraversal_DequeBenchmark(benchmark::State& iState)
    {
        StlCacheContainerTraversal_RunBenchmark(iState, MakeContainerWithRandomValues<std::deque, std::int8_t>(iState.range(0)));
    }

    void StlCacheContainerTraversal_ListBenchmark(benchmark::State& iState)
    {
        StlCacheContainerTraversal_RunBenchmark(iState, MakeContainerWithRandomValues<std::list, std::int8_t>(iState.range(0)));
    }

    void StlCacheContainerTraversal_ListShuffleBenchmark(benchmark::State& iState)
    {
        auto sl = MakeContainerWithRandomValues<std::list, std::int8_t>(iState.range(0));
        sl.sort();

        StlCacheContainerTraversal_RunBenchmark(iState, sl);
    }

    void StlCacheContainerTraversal_SetInt32Benchmark(benchmark::State& iState)
    {
        StlCacheContainerTraversal_RunBenchmark(iState, MakeSetWithRandomValues<std::set, std::int32_t>(iState.range(0)));
    }

    void StlCacheContainerTraversal_UnorderedSetInt32Benchmark(benchmark::State& iState)
    {
        StlCacheContainerTraversal_RunBenchmark(iState, MakeSetWithRandomValues<std::unordered_set, std::int32_t>(iState.range(0)));
    }

    void StlCacheContainerTraversal_VectorBenchmark(benchmark::State& iState)
    {
        StlCacheContainerTraversal_RunBenchmark(iState, MakeContainerWithRandomValues<std::vector, std::int8_t>(iState.range(0)));
    }

    void StlCacheContainerTraversal_Arguments(benchmark::internal::Benchmark* iBenchmark)
    {
        for (std::size_t i = 200_B; i <= 3_KB; i += 200_B)
        {
            iBenchmark->Arg(i);
        }
    }
}

BENCHMARK(StlCacheContainerTraversal_DequeBenchmark)->Apply(StlCacheContainerTraversal_Arguments);             // NOLINT
BENCHMARK(StlCacheContainerTraversal_ListBenchmark)->Apply(StlCacheContainerTraversal_Arguments);              // NOLINT
BENCHMARK(StlCacheContainerTraversal_ListShuffleBenchmark)->Apply(StlCacheContainerTraversal_Arguments);       // NOLINT
BENCHMARK(StlCacheContainerTraversal_SetInt32Benchmark)->Apply(StlCacheContainerTraversal_Arguments);          // NOLINT
BENCHMARK(StlCacheContainerTraversal_UnorderedSetInt32Benchmark)->Apply(StlCacheContainerTraversal_Arguments); // NOLINT
BENCHMARK(StlCacheContainerTraversal_VectorBenchmark)->Apply(StlCacheContainerTraversal_Arguments);            // NOLINT
