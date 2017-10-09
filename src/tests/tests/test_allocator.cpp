#include <gtest/gtest.h>
#include <allocators/memory_pool_resource.h>
#include <vector>
#include <cstddef>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <optional>
#include <list>
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

        void ExpectNextAllocate(std::size_t iExpectedAllocatedBytes)
        {
            this->m_ExpectedAllocatedBytes.emplace(iExpectedAllocatedBytes);
        }

        void ExpectNextDeallocate(std::size_t iExpectedDeallocatedBytes)
        {
            this->m_ExpectedDeallocatedBytes.emplace(iExpectedDeallocatedBytes);
        }

        void CheckAllocate(std::size_t iAllocatedBytes)
        {
            if (true == this->m_IgnoreCheck)
            {
                return;
            }

            EXPECT_EQ(true, this->m_ExpectedAllocatedBytes.has_value());
            EXPECT_EQ(iAllocatedBytes, this->m_ExpectedAllocatedBytes.value());

            this->m_ExpectedAllocatedBytes.reset();
        }

        void CheckDeallocate(std::size_t iDeallocatedBytes)
        {
            if (true == this->m_IgnoreCheck)
            {
                return;
            }

            EXPECT_EQ(true, this->m_ExpectedDeallocatedBytes.has_value());
            EXPECT_EQ(iDeallocatedBytes, this->m_ExpectedDeallocatedBytes.value());

            this->m_ExpectedDeallocatedBytes.reset();
        }

    private:
        std::optional<std::size_t> m_ExpectedAllocatedBytes;
        std::optional<std::size_t> m_ExpectedDeallocatedBytes;
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
    MemoryChecker memoryChecker;
    std::vector<char, StatsAllocator<char>> v(memoryChecker);

    memoryChecker.ExpectNextAllocate(10);
    v.reserve(10);

    std::size_t i = 0;
    for (; i < 10; ++i)
    {
        v.emplace_back(i);
    }

    memoryChecker.ExpectNextAllocate(20);
    memoryChecker.ExpectNextDeallocate(10);

    for (; i < 20; ++i)
    {
        v.emplace_back(i);
    }

    memoryChecker.ExpectNextAllocate(40);
    memoryChecker.ExpectNextDeallocate(20);
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
    constexpr const std::size_t nodeSize = ListNodeSize<char>;
    static_assert(24u == nodeSize);

    MemoryChecker memoryChecker;
    std::list<char, StatsAllocator<char>> v(memoryChecker);

    memoryChecker.ExpectNextAllocate(nodeSize);
    v.push_back(1);

    memoryChecker.ExpectNextAllocate(nodeSize);
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
}

TEST(AllocatorTest, AllocatorUnorderedMapCpp11)
{
    constexpr const std::size_t nodeSize = UnorderedMapNodeSize<char, char, std::hash<char>>;
    static_assert(16u == nodeSize);

    MemoryChecker memoryChecker;
    std::unordered_map<char, char, std::hash<char>, std::equal_to<char>, StatsAllocator<std::pair<const char, char>>> v(memoryChecker);

    // TODO : CHECK HOW THE TABLE IS ALLOCATED
    memoryChecker.ExpectNextAllocate(88); // ??????????????????? WHY 88 (array + 1st block per bucket), i.e. array of 10 + 10 bucket (just ptr to next sizeof(8)) ?!? (CHECK)
    v.reserve(10);

    std::size_t i = 0;
    for (; i < 10; ++i)
    {
        memoryChecker.ExpectNextAllocate(nodeSize);
        v.emplace(i, i);
    }

    // TODO
    // - insert node when rehash is needed
    // - insert node with emplace on the same index (temporary node will be created to use find then deleted afterwards)

    memoryChecker.IgnoreChecks();
}
