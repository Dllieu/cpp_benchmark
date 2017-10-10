#include <gtest/gtest.h>
#include <allocators/memory_pool_resource.h>
#include <vector>
#include <cstddef>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <list>
#include <queue>
#include <unordered_map>

namespace
{
    class MemoryChecker
    {
    public:
        MemoryChecker() noexcept
            : m_ExpectedAllocatedBytes(),
              m_ExpectedDeallocatedBytes(),
              m_IgnoreCheck(false)
        {
        }

        void IgnoreChecks()
        {
            this->m_IgnoreCheck = true;
        }

        void ExpectAllocate(std::size_t iExpectedAllocatedBytes)
        {
            this->m_ExpectedAllocatedBytes.push(iExpectedAllocatedBytes);
        }

        void ExpectDeallocate(std::size_t iExpectedDeallocatedBytes)
        {
            this->m_ExpectedDeallocatedBytes.push(iExpectedDeallocatedBytes);
        }

        void CheckAllocate(std::size_t iAllocatedBytes)
        {
            if (true == this->m_IgnoreCheck)
            {
                return;
            }

            EXPECT_EQ(false, this->m_ExpectedAllocatedBytes.empty());
            EXPECT_EQ(iAllocatedBytes, this->m_ExpectedAllocatedBytes.front());

            this->m_ExpectedAllocatedBytes.pop();
        }

        void CheckDeallocate(std::size_t iDeallocatedBytes)
        {
            if (true == this->m_IgnoreCheck)
            {
                return;
            }

            EXPECT_EQ(false, this->m_ExpectedDeallocatedBytes.empty());
            EXPECT_EQ(iDeallocatedBytes, this->m_ExpectedDeallocatedBytes.front());

            this->m_ExpectedDeallocatedBytes.pop();
        }

    private:
        std::queue<std::size_t> m_ExpectedAllocatedBytes;
        std::queue<std::size_t> m_ExpectedDeallocatedBytes;
        bool m_IgnoreCheck;
    };

    template <typename T>
    class StatsAllocator
    {
    public:
        using value_type = T;

        StatsAllocator(MemoryChecker& iMemoryStats) noexcept
            : m_MemoryChecker(iMemoryStats)
        {
        }

        template <typename U>
        explicit StatsAllocator(const StatsAllocator<U>& iAllocator)
            : m_MemoryChecker(iAllocator.m_MemoryChecker)
        {
            // Must be careful in case of stateful allocator used in node based container as this constructor might be called for each node allocation
        }

        value_type* allocate(std::size_t iNumber)
        {
            this->m_MemoryChecker.CheckAllocate(iNumber * sizeof(value_type));

            return static_cast<value_type*>(::operator new (iNumber * sizeof(value_type)));
        }

        void deallocate(value_type* iPointer, std::size_t iNumber) noexcept
        {
            this->m_MemoryChecker.CheckDeallocate(iNumber * sizeof(value_type));

            ::operator delete(iPointer);
        }

        MemoryChecker& m_MemoryChecker;
    };

    template <typename T, typename U>
    bool operator==(const StatsAllocator<T>&, const StatsAllocator<U>&) noexcept
    {
        return true;
    }

    template <typename T, typename U>
    bool operator!=(StatsAllocator<T> const& x, StatsAllocator<U> const& y) noexcept
    {
        return false == (x == y);
    }
}

TEST(AllocatorTest, AllocatorVectorCpp11)
{
    // struct : allocator { ptr* start ; ptr* finish ; ptr* endOfStorage }
    static_assert(24 == sizeof(std::vector<char>)); // because default allocator is stateless

    MemoryChecker memoryChecker;
    std::vector<char, StatsAllocator<char>> v(memoryChecker);

    memoryChecker.ExpectAllocate(10);
    v.reserve(10);

    std::size_t i = 0;
    for (; i < 10; ++i)
    {
        v.emplace_back(i);
    }

    // Call internally _M_check_len to know how much we should allocate which do in this case return size() + std::max(size(), 1)
    memoryChecker.ExpectAllocate(20);
    memoryChecker.ExpectDeallocate(10);

    for (; i < 20; ++i)
    {
        v.emplace_back(i);
    }

    memoryChecker.ExpectAllocate(40);
    memoryChecker.ExpectDeallocate(20);
    v.emplace_back(20);

    memoryChecker.IgnoreChecks();
}

namespace
{
    // struct { node* next ; node* prev ; T };
    template <typename T>
    static constexpr const std::size_t ListNodeSize = sizeof(std::_List_node<T>);
}

TEST(AllocatorTest, AllocatorListCpp11)
{
    // struct : allocator { __gnu_cxx::__aligned_membuf<std::size_t> size; node* next ; node* prev ; }
    static_assert(24 == sizeof(std::list<char>)); // because default allocator is stateless

    constexpr const std::size_t nodeSize = ListNodeSize<char>;
    static_assert(24u == nodeSize);

    MemoryChecker memoryChecker;
    std::list<char, StatsAllocator<char>> v(memoryChecker);

    memoryChecker.ExpectAllocate(nodeSize);
    v.push_back(1);

    memoryChecker.ExpectAllocate(nodeSize);
    v.push_back(2);

    memoryChecker.IgnoreChecks();
}

namespace
{
    // IF __fast_hash and __is_noexcept_hash
    //// struct { node* next ; buffer[sizeof(std::pair<const Key, Value)] };
    // ELSE
    //// struct { node* next ; std::size_t hashCode ; buffer[sizeof(std::pair<const Key, Value)] };
    template <typename Key, typename Value, typename Hash>
    static constexpr const std::size_t UnorderedMapNodeSize = sizeof(std::__detail::_Hash_node<std::pair<const Key, Value>, false == std::__is_fast_hash<Hash>::value || false == std::__detail::__is_noexcept_hash<Key, Hash>::value>);

    // std::__details::_Prime_rehash_policy
    template <typename RehashPolicy>
    std::size_t GetBucketRequired(std::size_t iN, RehashPolicy&& iRehashPolicy)
    {
        return iRehashPolicy._M_next_bkt(iN);
    }
}

TEST(AllocatorTest, AllocatorUnorderedMapCpp11)
{
    constexpr const std::size_t nodeSize = UnorderedMapNodeSize<char, char, std::hash<char>>;
    static_assert(16u == nodeSize);

    MemoryChecker memoryChecker;
    // No allocation is required at construction, unordered_map use a pre-allocated default node, this is to avoid to add branch (e.g. to avoid branching to check if n != 0 in order to perform modulo)
    std::unordered_map<char, char, std::hash<char>, std::equal_to<char>, StatsAllocator<std::pair<const char, char>>> v(memoryChecker);

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
    memoryChecker.ExpectAllocate(implicitBucketsCreated * sizeof(void*));
    v.reserve(10);

    // Always two methods to keep in mind
    // - _M_bucket_index : return the bkt given the hash of a key
    // - _M_find_node : iterate on all the node until the bkt is different or if the node key value is equal to the key

    std::size_t i = 0;
    for (; i < 10; ++i)
    {
        memoryChecker.ExpectAllocate(nodeSize);
        v.emplace(i, i);
    }

    memoryChecker.ExpectAllocate(nodeSize);
    memoryChecker.ExpectDeallocate(nodeSize);
    // Emplacing will allocate a node upfront, call _M_bucket_index, then call _M_find_node, if _M_find_node doesn't return nullptr, node will be deallocated
    // i.e. if the key is not unique, it will perform an allocation + deallocation + bkt traversal
    auto [it, couldEmplace] = v.emplace(0, 1);

    EXPECT_EQ(false, couldEmplace);
    EXPECT_EQ(0, it->second);

    memoryChecker.ExpectAllocate(nodeSize);
    memoryChecker.ExpectAllocate(primeRehashPolicy._M_next_bkt(implicitBucketsCreated * primeRehashPolicy._S_growth_factor) * sizeof(void*));
    memoryChecker.ExpectDeallocate(implicitBucketsCreated * sizeof(void*));
    v.emplace(10, 10);

    memoryChecker.IgnoreChecks();
}
