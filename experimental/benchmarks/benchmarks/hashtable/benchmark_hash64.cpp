#include <benchmark/benchmark.h>
#include <cstddef>
#include <functional>
#include <random>
#include <utils/macros.h>
#include <vector>

namespace
{
    template <typename HashTableT>
    HashTableT CreateHashTable(std::size_t iNumberElements)
    {
        using T = typename HashTableT::key_type;

        std::uniform_int_distribution<T> randomDistribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        std::mt19937_64 generator;

        HashTableT hashTable;

        while (hashTable.size() != iNumberElements)
        {
            hashTable.emplace(randomDistribution(generator), hashTable.size());
        }

        return hashTable;
    }

    template <typename HashT>
    void Hash64_RunBenchmark(benchmark::State& iState)
    {
        using HashTableT = std::unordered_map<std::int64_t, std::int64_t, HashT>;

        HashTableT hashTable = CreateHashTable<HashTableT>(iState.range(0));

        std::vector<typename HashTableT::key_type> dataToLookup;
        dataToLookup.reserve(hashTable.size());

        for (const auto& element : hashTable)
        {
            dataToLookup.emplace_back(element.first);
        }

        std::mt19937_64 randomGenerator(235432876);
        std::shuffle(std::begin(dataToLookup), std::end(dataToLookup), randomGenerator);

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

    void Hash64_Murmur3MixerBenchmark(benchmark::State& iState)
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

        Hash64_RunBenchmark<Murmur3MixerHash>(iState);
    }

    void Hash64_StdHashBenchmark(benchmark::State& iState)
    {
        Hash64_RunBenchmark<std::hash<std::int64_t>>(iState);
    }

    void Hash64_ThomasWangHashBenchmark(benchmark::State& iState)
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

        Hash64_RunBenchmark<ThomasWangHash>(iState);
    }

    void Hash64_Arguments(benchmark::internal::Benchmark* iBenchmark)
    {
        for (double i = 10; i <= 200'000; i *= 2.4) // NOLINT
        {
            iBenchmark->Arg(i);
        }
    }
}

BENCHMARK(Hash64_Murmur3MixerBenchmark)->Apply(Hash64_Arguments);   // NOLINT
BENCHMARK(Hash64_StdHashBenchmark)->Apply(Hash64_Arguments);        // NOLINT
BENCHMARK(Hash64_ThomasWangHashBenchmark)->Apply(Hash64_Arguments); // NOLINT
