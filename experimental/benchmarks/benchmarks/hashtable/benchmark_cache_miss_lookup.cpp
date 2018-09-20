#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#include <flat_hash_map.hpp>
#include <sparsehash/dense_hash_map>
#pragma GCC diagnostic pop
#include <algorithm>
#include <benchmark/benchmark.h>
#include <benchmarks/hashtable/hashtable_utils.h>
#include <random>
#include <unordered_map>
#include <utils/cache_information.h>
#include <utils/macros.h>
#include <utils/traits.h>
#include <vector>

namespace
{
    template <typename HashTableT, typename PostInitF>
    void RunBenchmark(benchmark::State& iState, PostInitF&& iPostInitFunctor)
    {
        std::vector<HashTableT> hashTables = benchmarks::CreateHashTablesWithRandomElementsToOverflowL3Cache<HashTableT>(iState.range(0), iPostInitFunctor);
        std::vector<typename HashTableT::key_type> dataToLookup = benchmarks::CreateVectorFromHashTableKeysShuffled(hashTables.front());

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
    void RunBenchmark(benchmark::State& iState)
    {
        return RunBenchmark<HashTableT>(iState, [](HashTableT& iHashTable) {});
    }

    void HashTableCacheMissLookup_DenseHashMap(benchmark::State& iState)
    {
        RunBenchmark<google::dense_hash_map<std::int64_t, std::int64_t>>(iState, [](auto& iHashTable) { iHashTable.set_empty_key(-1); });
    }

    void HashTableCacheMissLookup_FlatHashMap(benchmark::State& iState)
    {
        RunBenchmark<ska::flat_hash_map<std::int64_t, std::int64_t>>(iState);
    }

    void HashTableCacheMissLookup_FlatHashMapPower2(benchmark::State& iState)
    {
        RunBenchmark<ska::flat_hash_map<std::int64_t, std::int64_t, ska::power_of_two_std_hash<std::int64_t>>>(iState);
    }

    void HashTableCacheMissLookup_UnorderedMap(benchmark::State& iState)
    {
        RunBenchmark<std::unordered_map<std::int64_t, std::int64_t>>(iState);
    }

    void BenchmarkArguments(benchmark::internal::Benchmark* iBenchmark)
    {
        for (double i = 10; i <= 120'000; i *= 1.4) // NOLINT
        {
            iBenchmark->Arg(i);
        }
    }
}

BENCHMARK(HashTableCacheMissLookup_DenseHashMap)->Apply(BenchmarkArguments);      // NOLINT
BENCHMARK(HashTableCacheMissLookup_FlatHashMap)->Apply(BenchmarkArguments);       // NOLINT
BENCHMARK(HashTableCacheMissLookup_FlatHashMapPower2)->Apply(BenchmarkArguments); // NOLINT
BENCHMARK(HashTableCacheMissLookup_UnorderedMap)->Apply(BenchmarkArguments);      // NOLINT
