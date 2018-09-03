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
#include <utils/cache_information.h>
#include <utils/macros.h>
#include <utils/traits.h>
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

    template <typename T>
    std::size_t NumberOfContainerToOverflowL3Cache(std::size_t iNumberOfElements)
    {
        return std::max(std::size_t(2), (3u * experimental::enum_cast(experimental::CacheSize::L3)) / (sizeof(T) * iNumberOfElements));
    }

    template <typename HashTableT, typename PostInitF>
    std::vector<HashTableT> CreateContainerHashTable(std::size_t iNumberElements, PostInitF&& iPostInitFunctor)
    {
        std::size_t numberOfContainerToOverflowL3Cache = NumberOfContainerToOverflowL3Cache<typename HashTableT::value_type>(iNumberElements);
        std::vector<HashTableT> hashTables;
        HashTableT hashTable = CreateHashTable<HashTableT>(iNumberElements, iPostInitFunctor);

        for (std::size_t i = 0; i < numberOfContainerToOverflowL3Cache; ++i)
        {
            hashTables.push_back(hashTable);
        }

        return hashTables;
    }

    template <typename HashTableT, typename PostInitF>
    void HashTableCacheMissFailLookup_RunBenchmark(benchmark::State& iState, PostInitF&& iPostInitFunctor)
    {
        std::vector<HashTableT> hashTables = CreateContainerHashTable<HashTableT>(iState.range(0), iPostInitFunctor);

        std::vector<typename HashTableT::key_type> dataToLookup; // generate millionof random number with != generator for seeding random
        dataToLookup.reserve(1'000'000);

        std::uniform_int_distribution<typename HashTableT::key_type> randomDistribution(std::numeric_limits<typename HashTableT::key_type>::min(), std::numeric_limits<typename HashTableT::key_type>::max());
        std::mt19937_64 generator(3425789328);

        for (std::size_t i = 0; i < 1'000'000; ++i)
        {
            dataToLookup.emplace_back(randomDistribution(generator));
        }

        std::size_t hashTableIndex = 0;
        std::size_t i = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            benchmark::DoNotOptimize(hashTables[hashTableIndex].find(dataToLookup[i]));

            if (unlikely(hashTables.size() == ++hashTableIndex))
            {
                hashTableIndex = 0;
            }

            if (unlikely(dataToLookup.size() == ++i))
            {
                i = 0;
            }
        }
    }

    template <typename HashTableT>
    void HashTableCacheMissFailLookup_RunBenchmark(benchmark::State& iState)
    {
        return HashTableCacheMissFailLookup_RunBenchmark<HashTableT>(iState, [](HashTableT& iHashTable) {});
    }

    void HashTableCacheMissFailLookup_DenseHashMapBenchmark(benchmark::State& iState)
    {
        HashTableCacheMissFailLookup_RunBenchmark<google::dense_hash_map<std::int64_t, std::int64_t>>(iState, [](auto& iHashTable) { iHashTable.set_empty_key(-1); });
    }

    void HashTableCacheMissFailLookup_FlatHashMapBenchmark(benchmark::State& iState)
    {
        HashTableCacheMissFailLookup_RunBenchmark<ska::flat_hash_map<std::int64_t, std::int64_t>>(iState);
    }

    void HashTableCacheMissFailLookup_FlatHashMapPower2Benchmark(benchmark::State& iState)
    {
        HashTableCacheMissFailLookup_RunBenchmark<ska::flat_hash_map<std::int64_t, std::int64_t, ska::power_of_two_std_hash<std::int64_t>>>(iState);
    }

    void HashTableCacheMissFailLookup_UnorderedMapBenchmark(benchmark::State& iState)
    {
        HashTableCacheMissFailLookup_RunBenchmark<std::unordered_map<std::int64_t, std::int64_t>>(iState);
    }

    void HashTableCacheMissFailLookup_Arguments(benchmark::internal::Benchmark* iBenchmark)
    {
        for (double i = 10; i <= 120'000; i *= 1.4) // NOLINT
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
BENCHMARK(HashTableCacheMissFailLookup_DenseHashMapBenchmark)->Apply(HashTableCacheMissFailLookup_Arguments);      // NOLINT
BENCHMARK(HashTableCacheMissFailLookup_FlatHashMapBenchmark)->Apply(HashTableCacheMissFailLookup_Arguments);       // NOLINT
BENCHMARK(HashTableCacheMissFailLookup_FlatHashMapPower2Benchmark)->Apply(HashTableCacheMissFailLookup_Arguments); // NOLINT
BENCHMARK(HashTableCacheMissFailLookup_UnorderedMapBenchmark)->Apply(HashTableCacheMissFailLookup_Arguments);      // NOLINT
