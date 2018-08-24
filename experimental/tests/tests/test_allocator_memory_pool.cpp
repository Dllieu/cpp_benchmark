#include <gtest/gtest.h>
#include <allocators/allocator_memory_pool_fixed_size.h>
#include <allocators/memory_pool_fixed_size.h>
#include <allocators/memory_resource_memory_pool_fixed_size.h>
#include <utils/allocator_statistics_checker.h>
#include <array>
#include <list>
#include <cstdint>

namespace pmr = std::experimental::fundamentals_v2::pmr;

namespace
{
    template <typename T>
    static constexpr const std::size_t ListNodeSize = sizeof(std::_List_node<T>);

    constexpr const std::size_t BlockSize = 32;
}

TEST(AllocatorMemoryPool, Cpp11Way) // NOLINT
{
    tests::StatisticsChecker statisticsChecker;
    tests::AllocatorStatisticsChecker<std::uint64_t> alloc(statisticsChecker);

    experimental::MemoryPoolFixedSize<BlockSize> memoryPool;
    experimental::AllocatorMemoryPoolFixedSize<std::uint64_t, BlockSize, tests::AllocatorStatisticsChecker<std::uint64_t>> allocMemoryPool(memoryPool, statisticsChecker);

    memoryPool.ResetMemoryPool(64);

    std::list<std::uint64_t, decltype(allocMemoryPool)> l(allocMemoryPool);

    std::size_t i = 0;

    // Reuse Block from the MemoryPool
    for (; i < 64; ++i)
    {
        l.emplace_back(i);
    }

    statisticsChecker.ExpectAllocate(ListNodeSize<std::uint64_t>);

    // Will call ::new (No block available)
    l.emplace_back(i);

    statisticsChecker.IgnoreChecks();
}


namespace std::experimental // NOLINT
{
inline namespace fundamentals_v2
{
namespace pmr
{
    template <typename T>
    using list = std::list<T, pmr::polymorphic_allocator<T>>;
}
}
}

TEST(AllocatorMemoryPool, Cpp17Way) // NOLINT
{
    tests::StatisticsChecker statisticsChecker;
    tests::MemoryResourceStatisticsChecker memoryResourceStatisticsChecker(statisticsChecker);

    experimental::MemoryPoolFixedSize<BlockSize> memoryPool;
    experimental::MemoryResourceMemoryPoolFixedSize<BlockSize> memoryResourceMemoryPool(memoryPool, &memoryResourceStatisticsChecker);

    memoryPool.ResetMemoryPool(64);

    pmr::list<std::uint64_t> l(&memoryResourceMemoryPool);

    std::size_t i = 0;

    // Reuse Block from the MemoryPool
    for (; i < 64; ++i)
    {
        l.emplace_back(i);
    }

    statisticsChecker.ExpectAllocate(ListNodeSize<std::uint64_t>);

    // Will call ::new (No block available)
    l.emplace_back(i);

    statisticsChecker.IgnoreChecks();
}
