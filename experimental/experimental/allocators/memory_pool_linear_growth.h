#pragma once

#include <cstddef>

namespace allocators
{
    template <std::size_t BlockSize>
    class MemoryPoolLinearGrowth
    {
        struct FreeList
        {
            FreeList* m_Next;
        };

    public:
        explicit MemoryPoolLinearGrowth(std::size_t iNumberOfBlocks);
        ~MemoryPoolLinearGrowth();

        std::byte* GetFreeBlock();
        void AddFreeBlock(std::byte* iBlock);

    private:
        std::byte* m_Arena;
        std::size_t m_ArenaSize;

        FreeList* m_FreeBlock;
        FreeList* m_FreeBlockNotOwned;
    };
}

#include <allocators/memory_pool_linear_growth.hxx>
