#pragma once

#include <allocators/memory_block.h>
#include <cstdint>
#include <utils/macros.h>

namespace experimental
{
    template <std::size_t BlockSize>
    class MemoryPoolFixedSize
    {
    public:
        MemoryPoolFixedSize();
        DELETE_COPY_MOVE_CONSTRUCTOR(MemoryPoolFixedSize);
        ~MemoryPoolFixedSize() = default;

        void ResetMemoryPool(std::size_t iNumberOfBlocks);

        void* TakeBlock(std::size_t iRequestedSize);
        bool ReturnBlock(void* iBlock);

    private:
        struct Node
        {
            Node* next;
        };

        Node* m_Head;
        MemoryBlock<BlockSize> m_MemoryBlock;
    };
}

#include <allocators/memory_pool_fixed_size.hxx>
