#include <allocators/memory_block.h>

namespace experimental
{
    template <std::size_t BlockSize>
    MemoryBlock<BlockSize>::MemoryBlock() : m_Blocks(nullptr), m_NumberOfBlocks(0)
    {
    }

    template <std::size_t BlockSize>
    MemoryBlock<BlockSize>::~MemoryBlock()
    {
        delete[] this->m_Blocks;
    }

    template <std::size_t BlockSize>
    void MemoryBlock<BlockSize>::Allocate(std::size_t iNumberOfBlocks)
    {
        delete[] this->m_Blocks;

        this->m_NumberOfBlocks = iNumberOfBlocks;
        this->m_Blocks = new Block[this->m_NumberOfBlocks];
    }

    template <std::size_t BlockSize>
    auto MemoryBlock<BlockSize>::begin() const -> Block*
    {
        return this->m_Blocks;
    }

    template <std::size_t BlockSize>
    auto MemoryBlock<BlockSize>::end() const -> Block*
    {
        return this->m_Blocks + this->m_NumberOfBlocks;
    }
}
