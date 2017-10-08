#include <allocators/memory_pool_linear_growth.h>
#include <utils/macros.h>

namespace allocators
{
    template <std::size_t BlockSize>
    MemoryPoolLinearGrowth<BlockSize>::MemoryPoolLinearGrowth(std::size_t iNumberOfBlocks)
        : m_Arena(new std::byte[iNumberOfBlocks * BlockSize]),
          m_ArenaSize(iNumberOfBlocks * BlockSize),
          m_FreeBlock(reinterpret_cast<FreeList*>(this->m_Arena)),
          m_FreeBlockNotOwned(nullptr)
    {
        FreeList* pNode = this->m_FreeBlock;

        for (std::size_t i = 1; i < iNumberOfBlocks; ++i)
        {
            pNode->m_Next = reinterpret_cast<FreeList*>(this->m_Arena + BlockSize * i);
            pNode = pNode->m_Next;
        }

        pNode->m_Next = nullptr;
    }

    template <std::size_t BlockSize>
    MemoryPoolLinearGrowth<BlockSize>::~MemoryPoolLinearGrowth()
    {
        delete [] this->m_Arena;

        while (nullptr != this->m_FreeBlockNotOwned)
        {
            FreeList* pNode = this->m_FreeBlockNotOwned->m_Next;

            delete this->m_FreeBlockNotOwned;

            this->m_FreeBlockNotOwned = pNode;
        }
    }

    template <std::size_t BlockSize>
    std::byte* MemoryPoolLinearGrowth<BlockSize>::GetFreeBlock()
    {
        FreeList* pNode = nullptr;

        if (likely(nullptr != this->m_FreeBlock))
        {
            pNode = this->m_FreeBlock;
            this->m_FreeBlock = this->m_FreeBlock->m_Next;
        }
        else if (likely(nullptr != this->m_FreeBlockNotOwned))
        {
            pNode = this->m_FreeBlockNotOwned;
            this->m_FreeBlockNotOwned = this->m_FreeBlockNotOwned->m_Next;
        }

        return reinterpret_cast<std::byte*>(pNode);
    }

    template <std::size_t BlockSize>
    void MemoryPoolLinearGrowth<BlockSize>::AddFreeBlock(std::byte* iBlock)
    {
        if (likely(this->m_Arena <= iBlock && (iBlock < this->m_Arena + this->m_ArenaSize)))
        {
            FreeList* pNode = this->m_FreeBlock;

            this->m_FreeBlock = reinterpret_cast<FreeList*>(iBlock);
            this->m_FreeBlock->m_Next = pNode;
        }
        else
        {
            FreeList* pNode = this->m_FreeBlockNotOwned;

            this->m_FreeBlockNotOwned = reinterpret_cast<FreeList*>(iBlock);
            this->m_FreeBlockNotOwned->m_Next = pNode;
        }
    }
}
