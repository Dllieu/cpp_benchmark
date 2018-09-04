#include <benchmark/benchmark.h>
#include <benchmarks/hashtable/hashtable_utils.h>
#include <cstddef>
#include <functional>
#include <random>
#include <utils/macros.h>
#include <vector>

namespace
{
    template <typename HashT>
    void RunBenchmark(benchmark::State& iState)
    {
        using HashTableT = std::unordered_map<std::int64_t, std::int64_t, HashT>;

        HashTableT hashTable = benchmarks::CreateHashTableWithRandomElements<HashTableT>(iState.range(0), [](const auto&) {});
        std::vector<typename HashTableT::key_type> dataToLookup = benchmarks::CreateVectorFromHashTableKeysShuffled(hashTable);

        std::size_t i = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            benchmark::DoNotOptimize(hashTable.find(dataToLookup[i]));
            if (unlikely(dataToLookup.size() == ++i))
            {
                i = 0;
            }
        }
    }

    void Hash64_Murmur3Mixer(benchmark::State& iState)
    {
        struct Murmur3MixerHash
        {
            std::int64_t operator()(std::int64_t iKey) const
            {
                iKey ^= (iKey >> 33);
                iKey *= 0xff51afd7ed558ccd;
                iKey ^= (iKey >> 33);
                iKey *= 0xc4ceb9fe1a85ec53;
                iKey ^= (iKey >> 33);

                return iKey;
            }
        };

        RunBenchmark<Murmur3MixerHash>(iState);
    }

    void Hash64_StdHash(benchmark::State& iState)
    {
        RunBenchmark<std::hash<std::int64_t>>(iState);
    }

    void Hash64_ThomasWangHash(benchmark::State& iState)
    {
        struct ThomasWangHash
        {
            std::int64_t operator()(std::int64_t iKey) const
            {
                iKey = (~iKey) + (iKey << 21);
                iKey = iKey ^ (iKey >> 24);
                iKey = (iKey + (iKey << 3)) + (iKey << 8);
                iKey = iKey ^ (iKey >> 14);
                iKey = (iKey + (iKey << 2)) + (iKey << 4);
                iKey = iKey ^ (iKey >> 28);
                iKey = iKey + (iKey << 31);

                return iKey;
            }
        };

        RunBenchmark<ThomasWangHash>(iState);
    }

    void BenchmarkArguments(benchmark::internal::Benchmark* iBenchmark)
    {
        for (double i = 10; i <= 200'000; i *= 2.4) // NOLINT
        {
            iBenchmark->Arg(i);
        }
    }
}

BENCHMARK(Hash64_Murmur3Mixer)->Apply(BenchmarkArguments);   // NOLINT
BENCHMARK(Hash64_StdHash)->Apply(BenchmarkArguments);        // NOLINT
BENCHMARK(Hash64_ThomasWangHash)->Apply(BenchmarkArguments); // NOLINT
