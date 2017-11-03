#include <gtest/gtest.h>
#include <allocators/allocator_memory_pool_fixed_size.h>
#include <allocators/memory_pool_fixed_size.h>
#include <allocators/memory_resource_memory_pool_fixed_size.h>
#include <utils/allocator_statistics_checker.h>
#include <array>
#include <list>
#include <cstdint>

namespace pmr = std::experimental::fundamentals_v2::pmr;
using namespace tests;

TEST(AllocatorMemoryPool, Cpp11Way)
{
    constexpr const std::size_t BlockSize = 32;
    experimental::MemoryPoolFixedSize<BlockSize> memoryPool;

    memoryPool.ResetMemoryPool(64);

    StatisticsChecker statisticsChecker;
    //std::list<std::uint64_t, experimental::AllocatorMemoryPoolFixedSize<std::uint64_t, BlockSize, AllocatorStatisticsChecker<std::uint64_t>>> l(memoryPool);
    std::list<std::uint64_t, experimental::AllocatorMemoryPoolFixedSize<std::uint64_t, BlockSize>> l(memoryPool);

    l.emplace_back(0);
}

namespace std::experimental::fundamentals_v2::pmr
{
    template <typename T>
    using list = std::list<T, pmr::polymorphic_allocator<T>>;
}

// TODO : Transform
TEST(AllocatorMemoryPool, Cpp17Way)
{
    constexpr const std::size_t BlockSize = 32;
    experimental::MemoryResourceMemoryPoolFixedSize<BlockSize> memoryPool;

    memoryPool.ResetMemoryPool(64);

    pmr::list<std::uint64_t> l(&memoryPool);

    std::size_t i = 0;

    // Reuse Block from the MemoryPool
    for (; i < 64; ++i)
    {
        l.emplace_back(i);
    }

    // Will call ::new (No block available)
    l.emplace_back(i);
}
