#include <gtest/gtest.h>
#include <utils/allocator_statistics_checker.h>
#include <vector>
#include <cstddef>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <list>
#include <queue>
#include <unordered_map>
#include <string>
#include <deque>

using namespace tests;

TEST(AllocationPatternStlGccTest, Vector)
{
    // struct : allocator { ptr* start ; ptr* finish ; ptr* endOfStorage }
    static_assert(24 == sizeof(std::vector<char>)); // empty base optimization (stateless allocator)

    StatisticsChecker statisticsChecker;
    std::vector<char, AllocatorStatisticsChecker<char>> v(statisticsChecker);

    statisticsChecker.ExpectAllocate(10);
    v.reserve(10);

    std::size_t i = 0;
    for (; i < 10; ++i)
    {
        v.emplace_back(i);
    }

    // Call internally _M_check_len to know how much we should allocate which do in this case return size() + std::max(size(), 1)
    statisticsChecker.ExpectAllocate(20);
    statisticsChecker.ExpectDeallocate(10);

    for (; i < 20; ++i)
    {
        v.emplace_back(i);
    }

    statisticsChecker.ExpectAllocate(40);
    statisticsChecker.ExpectDeallocate(20);
    v.emplace_back(20);

    statisticsChecker.IgnoreChecks();
}

TEST(AllocationPatternStlGccTest, String)
{
    // struct { alloc_hider : allocator { ptr* p; } ; std::size_t length ; union { std::size_t capacity ; char[15 + 1] localBuffer } }
    std::string s;
    static_assert(32 == sizeof(s)); // empty base optimization (stateless allocator)

    StatisticsChecker statisticsChecker;
    std::basic_string<char, std::char_traits<char>, AllocatorStatisticsChecker<char>> bs(statisticsChecker);
    static_assert(40 == sizeof(bs));

    // Small string optimization
    // - GCC support string of size 15 (union, p point to localBuffer)
    // - if p == &localBuffer : capacity = 15
    std::size_t i = 0;
    for (; i < 16; ++i)
    {
        bs = std::string(i, '@');
    }

    statisticsChecker.ExpectAllocate(2 * bs.capacity() + 1);
    bs = std::string(i, '@');

    statisticsChecker.IgnoreChecks();
}

TEST(AllocationPatternStlGccTest, Deque)
{
    // struct : alloc { elt** _M_map ; std::size_t mapSize; iterator start ; iterator finish } (i.e. _M_map[mapSize]* store mapSize ptr to chunk of memory (they call it node))
    //// with iterator { elt* current ; elt* first ; elt* last ; map_elt* node ; } (i.e. last not used to store an element but to store the address of the next chunk of memory (they call it node))
    std::deque<char> deque;
    static_assert(80 == sizeof(deque));

    StatisticsChecker statisticsChecker;

    using value_type = std::uint64_t;

    // Default constructor always start by allocation _M_map with mapSize == _S_initial_map_size (8) (_M_allocate_map)
    statisticsChecker.ExpectAllocate(8 * sizeof(void*));

    // Number of element per chunk, if value_type too big, we will only able to store one element per chunk
    std::size_t dequeBufferSize = std::__deque_buf_size(sizeof(value_type));

    // Then it allocate the nodes (_M_create_nodes), by default it create only one node
    statisticsChecker.ExpectAllocate(dequeBufferSize * sizeof(value_type)); // i.e. 512

    std::deque<value_type, AllocatorStatisticsChecker<value_type>> d(statisticsChecker);

    // First element can always be inserted in the pre-allocated chunk
    std::size_t i = 0;
    d.push_back(i++);

    // If dequeBufferSize > 1, _M_last is set to dequeBufferSize - 1
    for (; i < dequeBufferSize - 1; ++i)
    {
        d.push_back(i);
    }

    statisticsChecker.ExpectAllocate(dequeBufferSize * sizeof(value_type));
    d.push_back(i);

    statisticsChecker.IgnoreChecks();
}

namespace
{
    // struct { node* next ; node* prev ; T };
    template <typename T>
    static constexpr const std::size_t ListNodeSize = sizeof(std::_List_node<T>);
}

TEST(AllocationPatternStlGccTest, List)
{
    // struct : allocator { __gnu_cxx::__aligned_membuf<std::size_t> size; node* next ; node* prev ; }
    static_assert(24 == sizeof(std::list<char>)); // empty base optimization (stateless allocator)

    constexpr const std::size_t nodeSize = ListNodeSize<char>;
    static_assert(24u == nodeSize);

    StatisticsChecker statisticsChecker;
    std::list<char, AllocatorStatisticsChecker<char>> v(statisticsChecker);

    statisticsChecker.ExpectAllocate(nodeSize);
    v.push_back(1);

    statisticsChecker.ExpectAllocate(nodeSize);
    v.push_back(2);

    statisticsChecker.IgnoreChecks();
}

namespace
{
    // IF __fast_hash and __is_noexcept_hash
    //// struct { node* next ; buffer[sizeof(std::pair<const Key, Value)] };
    // ELSE
    //// struct { node* next ; std::size_t hashCode ; buffer[sizeof(std::pair<const Key, Value)] };
    template <typename Key, typename Value, typename Hash>
    static constexpr const std::size_t UnorderedMapNodeSize = sizeof(std::__detail::_Hash_node<std::pair<const Key, Value>, false == std::__is_fast_hash<Hash>::value || false == std::__detail::__is_noexcept_hash<Key, Hash>::value>);
}

TEST(AllocationPatternStlGccTest, UnorderedMap)
{
    constexpr const std::size_t nodeSize = UnorderedMapNodeSize<char, char, std::hash<char>>;
    static_assert(16u == nodeSize);

    StatisticsChecker statisticsChecker;
    // No allocation is required at construction, unordered_map use a pre-allocated default node, this is to avoid to add branch (e.g. to avoid branching to check if n != 0 in order to perform modulo)
    std::unordered_map<char, char, std::hash<char>, std::equal_to<char>, AllocatorStatisticsChecker<std::pair<const char, char>>> v(statisticsChecker);

    // Max element per bucket ratio (e.g. if max_load_factor / bucket_count >= 1 it will involve a rehash)
    EXPECT_EQ(1.0, v.max_load_factor());

    // Default rehashing policy used by unordered_map, _M_next_bkt will return the next prime from std::__detail::__prime_list[256]
    //                                                 _M_need_rehash will return (if rehash is needed) __n_bkt * _S_growth_factor (2)
    // Default range hashing policy used by unordered_map is _Mod_range_hashing, it allow to find the right bucket (_M_bucket_index) by doing HashKey % _M_bucket_count
    std::__detail::_Prime_rehash_policy primeRehashPolicy(v.max_load_factor());

    // New rehashing policy, can be used manually by declaring a _Hashtable, _M_next_bkt will return the next power of 2
    //                                                                       _M_need_rehash will return (if rehash is needed) __n_bkt * _S_growth_factor (2)
    // The compatible range hashing policy is _Mask_range_hashing, it allow to find the right bucket (_M_bucket_index) by doing HashKey & (_M_bucket_count - 1) (which is much more performant as it doesn't include any modulo)
    //std::__detail::_Power2_rehash_policy power2RehashPolicy(v.max_load_factor());

    // Allocate the bucket array (buckets are stored as a pointer)
    std::size_t implicitBucketsCreated = primeRehashPolicy._M_next_bkt(10);
    statisticsChecker.ExpectAllocate(implicitBucketsCreated * sizeof(void*));
    v.reserve(10);

    // Always two methods to keep in mind
    // - _M_bucket_index : return the bkt given the hash of a key
    // - _M_find_node : iterate on all the node until the bkt is different or if the node key value is equal to the key

    std::size_t i = 0;
    for (; i < 10; ++i)
    {
        statisticsChecker.ExpectAllocate(nodeSize);
        v.emplace(i, i);
    }

    statisticsChecker.ExpectAllocate(nodeSize);
    statisticsChecker.ExpectDeallocate(nodeSize);
    // Emplacing will allocate a node upfront, call _M_bucket_index, then call _M_find_node, if _M_find_node doesn't return nullptr, node will be deallocated
    // i.e. if the key is not unique, it will perform an allocation + deallocation + bkt traversal
    auto [it, couldEmplace] = v.emplace(0, 1);

    EXPECT_EQ(false, couldEmplace);
    EXPECT_EQ(0, it->second);

    statisticsChecker.ExpectAllocate(nodeSize);
    statisticsChecker.ExpectAllocate(primeRehashPolicy._M_next_bkt(implicitBucketsCreated * primeRehashPolicy._S_growth_factor) * sizeof(void*));
    statisticsChecker.ExpectDeallocate(implicitBucketsCreated * sizeof(void*));
    v.emplace(10, 10);

    statisticsChecker.IgnoreChecks();
}
