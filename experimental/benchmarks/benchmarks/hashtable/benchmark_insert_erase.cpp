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
#include <utils/macros.h>
#include <vector>

namespace
{
    template <typename HashTableT, typename PostInitF>
    void HashTableInsertErase_RunBenchmark(benchmark::State& iState, PostInitF&& iPostInitFunctor)
    {
        HashTableT hashTable = benchmarks::CreateHashTableWithRandomElements<HashTableT>(iState.range(0), std::forward<PostInitF>(iPostInitFunctor));

        constexpr std::size_t numberReplicateElements = 15;
        std::vector<typename HashTableT::key_type> dataToLookup(hashTable.size() * numberReplicateElements);
        auto dataToLookupIterator = std::begin(dataToLookup);

        for (std::size_t i = 0; i < numberReplicateElements; ++i)
        {
            dataToLookupIterator = std::transform(std::begin(hashTable), std::end(hashTable), dataToLookupIterator, [](const auto& iPair) { return iPair.first; });
        }

        std::mt19937_64 randomGenerator(235432876);
        std::shuffle(std::begin(dataToLookup), std::end(dataToLookup), randomGenerator);
        hashTable.clear();

        std::size_t i = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            auto [iterator, result] = hashTable.emplace(dataToLookup[i], i);

            if (false == result)
            {
                hashTable.erase(iterator);
            }

            if (unlikely(dataToLookup.size() == ++i))
            {
                i = 0;
            }
        }
    }

    template <typename HashTableT>
    void HashTableInsertErase_RunBenchmark(benchmark::State& iState)
    {
        return HashTableInsertErase_RunBenchmark<HashTableT>(iState, [](HashTableT& iHashTable) {});
    }

    void HashTableInsertErase_DenseHashMapBenchmark(benchmark::State& iState)
    {
        HashTableInsertErase_RunBenchmark<google::dense_hash_map<std::int64_t, std::int64_t>>(iState, [](auto& iHashTable) {
            iHashTable.set_empty_key(-1);
            iHashTable.set_deleted_key(-2);
        });
    }

    void HashTableInsertErase_FlatHashMapBenchmark(benchmark::State& iState)
    {
        HashTableInsertErase_RunBenchmark<ska::flat_hash_map<std::int64_t, std::int64_t>>(iState);
    }

    void HashTableInsertErase_FlatHashMapPower2Benchmark(benchmark::State& iState)
    {
        HashTableInsertErase_RunBenchmark<ska::flat_hash_map<std::int64_t, std::int64_t, ska::power_of_two_std_hash<std::int64_t>>>(iState);
    }

    void HashTableInsertErase_UnorderedMapBenchmark(benchmark::State& iState)
    {
        HashTableInsertErase_RunBenchmark<std::unordered_map<std::int64_t, std::int64_t>>(iState);
    }

    void HashTableInsertErase_Arguments(benchmark::internal::Benchmark* iBenchmark)
    {
        for (double i = 10; i <= 120'000; i *= 1.9) // NOLINT
        {
            iBenchmark->Arg(i);
        }
    }
}

BENCHMARK(HashTableInsertErase_DenseHashMapBenchmark)->Apply(HashTableInsertErase_Arguments);      // NOLINT
BENCHMARK(HashTableInsertErase_FlatHashMapBenchmark)->Apply(HashTableInsertErase_Arguments);       // NOLINT
BENCHMARK(HashTableInsertErase_FlatHashMapPower2Benchmark)->Apply(HashTableInsertErase_Arguments); // NOLINT
BENCHMARK(HashTableInsertErase_UnorderedMapBenchmark)->Apply(HashTableInsertErase_Arguments);      // NOLINT
