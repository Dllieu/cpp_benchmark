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
    template <typename ContainerT>
    ContainerT MakeContainerWithRandomValues(std::size_t iSize)
    {
        using T = typename ContainerT::value_type;

        std::uniform_int_distribution<T> randomDistribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        std::mt19937 generator;

        ContainerT container(iSize / sizeof(T));
        std::generate(std::begin(container), std::end(container), [&randomDistribution, &generator] { return randomDistribution(generator); });

        return container;
    }

    template <typename ContainerT>
    ContainerT MakeSetWithRandomValues(std::size_t iSize)
    {
        using T = typename ContainerT::value_type;

        std::uniform_int_distribution<T> randomDistribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        std::mt19937 generator;

        ContainerT container;

        // Quick and dirty
        while (container.size() != iSize / sizeof(T))
        {
            container.emplace(randomDistribution(generator));
        }

        return container;
    }

    template <typename T>
    void StlContainerTraversal_RunBenchmark(benchmark::State& iState, T&& iContainer)
    {
        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            benchmark::DoNotOptimize(n += std::accumulate(std::cbegin(iContainer), std::cend(iContainer), 0));
        }
    }

    void StlContainerTraversal_DequeBenchmark(benchmark::State& iState)
    {
        StlContainerTraversal_RunBenchmark(iState, MakeContainerWithRandomValues<std::deque<std::int32_t>>(iState.range(0)));
    }

    void StlContainerTraversal_ListBenchmark(benchmark::State& iState)
    {
        StlContainerTraversal_RunBenchmark(iState, MakeContainerWithRandomValues<std::list<std::int32_t>>(iState.range(0)));
    }

    void StlContainerTraversal_ListShuffleBenchmark(benchmark::State& iState)
    {
        auto sl = MakeContainerWithRandomValues<std::list<std::int32_t>>(iState.range(0));
        sl.sort();

        StlContainerTraversal_RunBenchmark(iState, sl);
    }

    void StlContainerTraversal_SetBenchmark(benchmark::State& iState)
    {
        StlContainerTraversal_RunBenchmark(iState, MakeSetWithRandomValues<std::set<std::int32_t>>(iState.range(0)));
    }

    void StlContainerTraversal_UnorderedSetBenchmark(benchmark::State& iState)
    {
        StlContainerTraversal_RunBenchmark(iState, MakeSetWithRandomValues<std::unordered_set<std::int32_t>>(iState.range(0)));
    }

    void StlContainerTraversal_VectorBenchmark(benchmark::State& iState)
    {
        StlContainerTraversal_RunBenchmark(iState, MakeContainerWithRandomValues<std::vector<std::int32_t>>(iState.range(0)));
    }

    void StlContainerTraversal_Arguments(benchmark::internal::Benchmark* iBenchmark)
    {
        for (std::size_t i = 2_KB; i <= 22_KB; i += 2_KB)
        {
            iBenchmark->Arg(i);
        }
    }
}

BENCHMARK(StlContainerTraversal_DequeBenchmark)->Apply(StlContainerTraversal_Arguments);        // NOLINT
BENCHMARK(StlContainerTraversal_ListBenchmark)->Apply(StlContainerTraversal_Arguments);         // NOLINT
BENCHMARK(StlContainerTraversal_ListShuffleBenchmark)->Apply(StlContainerTraversal_Arguments);  // NOLINT
BENCHMARK(StlContainerTraversal_SetBenchmark)->Apply(StlContainerTraversal_Arguments);          // NOLINT
BENCHMARK(StlContainerTraversal_UnorderedSetBenchmark)->Apply(StlContainerTraversal_Arguments); // NOLINT
BENCHMARK(StlContainerTraversal_VectorBenchmark)->Apply(StlContainerTraversal_Arguments);       // NOLINT
