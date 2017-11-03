#include <gtest/gtest.h>
#include <allocators/allocator_memory_pool_fixed_size.h>
#include <allocators/memory_pool_fixed_size.h>
#include <array>
#include <list>
#include <cstdint>

TEST(AllocatorMemoryPool, Basic)
{
    constexpr const std::size_t BlockSize = 32;
    experimental::MemoryPoolFixedSize<BlockSize> memoryPool;

    // e.g. Read a value from a file (like order statistics)
    //memoryPool.ResetMemoryPool(64);

    std::list<std::uint64_t, experimental::AllocatorMemoryPoolFixedSize<std::uint64_t, BlockSize>> l(memoryPool);

    std::size_t i = 0;

    // Reuse Block from the MemoryPool
    for (; i < 64; ++i)
    {
        l.emplace_back(i);
    }

    // Will call ::new (No block available)
    l.emplace_back(i);
}
