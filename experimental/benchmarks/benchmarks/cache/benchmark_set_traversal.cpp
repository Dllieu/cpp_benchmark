#include <numeric>
#include <benchmark/benchmark.h>
#include <set>
#include <unordered_set>
#include <utils/cache_information.h>

// Unordered maps can be implemented in a variety of ways, with implications for the memory usage.The fundamental expectation is that there'll be a contiguous array of key/value "buckets",
// but in real-world implementations the basic design tradeoffs may involve:
//
// - two or more contiguous regions to reduce the performance cost when growing the container capacity, and separately
// - when there's a collision, an implementation may
//      (A) use an algorithm to select a sequence of alternative buckets or
//      (B) they may have each bucket be / point - to a resizable container of the key / value pairs.
//
//  Trying to make this latter choice more tangible : at its academic simplest, you can imagine :
//      (A) the hash table finding alternative "buckets" - as an array containing of key / value pairs, with empty / unused values scattered amongst the meaningful ones, akin to vector<optional<pair<key, value>>>.
//      (B) the hash table that instead uses containers is like a vector<list<pair<key, value>>> where every vector element is populated,
//          but getting from the vector elements to the lists involves extra pointers and discontiguous memory regions : this will be a little slower to deallocate as there are more distinct memory areas to delete.
//
// If the ratio of used to unused buckets is kept low, then there will be less collisions but more wasted memory.
// Another consideration : as the size of key / value pairs increase, the memory allocation overheads and pointers become less significant in comparison,
// so maps tend to use less memory than a hash map with low utilisation and values stored directly in the buckets.But, you can often create a hash map of key / pointers - to - value which mitigates that problem.
// So, there's the potential for a hash map to use less overall memory (particularly with small key/value types and a high used-to-unusued bucket ratio)
// and do less distinct allocations and deallocations as well as working better with caches, but it's far from guaranteed.
namespace
{
    void CacheSetTraversal_OrderedSetBenchmark(benchmark::State& state)
    {
        std::set<std::int32_t> set;
        for (auto i = 0; i < state.range(0); ++i)
        {
            set.insert(i);
        }

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : state)
        {
            benchmark::DoNotOptimize(n += std::accumulate(std::cbegin(set), std::cend(set), 0));
        }

        benchmark::DoNotOptimize(n);
    }

    void CacheSetTraversal_UnorderedSetBenchmark(benchmark::State& state)
    {
        std::unordered_set<std::int32_t> unorderedSet;
        for (auto i = 0; i < state.range(0); ++i)
        {
            unorderedSet.insert(i);
        }

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : state)
        {
            benchmark::DoNotOptimize(n += std::accumulate(std::cbegin(unorderedSet), std::cend(unorderedSet), 0));
        }

        benchmark::DoNotOptimize(n);
    }
}

BENCHMARK(CacheSetTraversal_OrderedSetBenchmark)->RangeMultiplier(2)->Range(1_KB, 64_KB); // NOLINT
BENCHMARK(CacheSetTraversal_UnorderedSetBenchmark)->RangeMultiplier(2)->Range(1_KB, 64_KB); // NOLINT
