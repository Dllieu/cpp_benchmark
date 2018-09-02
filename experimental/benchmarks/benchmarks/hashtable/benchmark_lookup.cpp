#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#include <flat_hash_map.hpp>
#include <sparsehash/dense_hash_map>
#pragma GCC diagnostic pop
#include <algorithm>
#include <benchmark/benchmark.h>
#include <random>
#include <unordered_map>
#include <utils/macros.h>
#include <vector>

namespace
{
    template <typename HashTableT, typename PostInitF>
    HashTableT CreateHashTable(std::size_t iNumberElements, PostInitF&& iPostInitFunctor)
    {
        using T = typename HashTableT::key_type;

        std::uniform_int_distribution<T> randomDistribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        std::mt19937_64 generator;

        HashTableT hashTable;
        iPostInitFunctor(hashTable);

        while (hashTable.size() != iNumberElements)
        {
            hashTable.emplace(randomDistribution(generator), hashTable.size());
        }

        return hashTable;
    }

    template <typename HashTableT, typename PostInitF>
    void HashTableLookup_RunBenchmark(benchmark::State& iState, PostInitF&& iPostInitFunctor)
    {
        HashTableT hashTable = CreateHashTable<HashTableT>(iState.range(0), std::forward<PostInitF>(iPostInitFunctor));

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

    template <typename HashTableT>
    void HashTableLookup_RunBenchmark(benchmark::State& iState)
    {
        return HashTableLookup_RunBenchmark<HashTableT>(iState, [](HashTableT& iHashTable) {});
    }

    void HashTableLookup_DenseHashMapBenchmark(benchmark::State& iState)
    {
        HashTableLookup_RunBenchmark<google::dense_hash_map<std::int64_t, std::int64_t>>(iState, [](auto& iHashTable) { iHashTable.set_empty_key(-1); });
    }

    void HashTableLookup_FlatHashMapBenchmark(benchmark::State& iState)
    {
        HashTableLookup_RunBenchmark<ska::flat_hash_map<std::int64_t, std::int64_t>>(iState);
    }

    void HashTableLookup_UnorderedMapBenchmark(benchmark::State& iState)
    {
        HashTableLookup_RunBenchmark<std::unordered_map<std::int64_t, std::int64_t>>(iState);
    }

    void HashTableLookup_Arguments(benchmark::internal::Benchmark* iBenchmark)
    {
        for (double i = 10; i <= 120'000; i *= 12) // NOLINT
        {
            iBenchmark->Arg(i);
        }
    }
}

// All of the graphs are spiky. This is because all hashtables have different performance depending on the current load factor.
// Meaning depending on how full they are. When a table is 25% full lookups will be faster than when it’s 50% full.
// The reason for this is that there are more hash collisions when the table is more full.
// So you can see the cost go up until at some point the table decides that it’s too full and that it should reallocate, which makes lookups fast again.

// Another thing that we notice is that all the graphs are essentially flat on the left half of the screen.
// This is because the table fits entirely into the cache. Only when we get to the point where the data doesn’t fit into the L3 cache do we see the different graphs really diverge.
// You will only get the numbers on the left if the element you’re looking for is already in the cache.
BENCHMARK(HashTableLookup_DenseHashMapBenchmark)->Apply(HashTableLookup_Arguments); // NOLINT
BENCHMARK(HashTableLookup_FlatHashMapBenchmark)->Apply(HashTableLookup_Arguments);  // NOLINT
BENCHMARK(HashTableLookup_UnorderedMapBenchmark)->Apply(HashTableLookup_Arguments); // NOLINT
