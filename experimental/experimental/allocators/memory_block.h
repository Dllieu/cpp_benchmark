#pragma once

#include <cstddef>
#include <cstdint>

namespace experimental
{
    template <std::size_t BlockSize>
    class MemoryBlock
    {
    public:
        using Block = std::byte[BlockSize];

        MemoryBlock();
        MemoryBlock(MemoryBlock&&) = delete;
        MemoryBlock(const MemoryBlock&) = delete;
        MemoryBlock& operator=(MemoryBlock&&) = delete;
        MemoryBlock& operator=(const MemoryBlock&) = delete;
        ~MemoryBlock();

        void Allocate(std::size_t iNumberOfBlocks);

        Block* begin() const;
        Block* end() const;

    private:
        Block* m_Blocks;
        std::size_t m_NumberOfBlocks;
    };
}

#include <allocators/memory_block.hxx>
