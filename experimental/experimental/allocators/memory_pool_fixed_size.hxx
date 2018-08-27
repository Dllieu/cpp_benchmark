#include <allocators/memory_pool_fixed_size.h>
#include <utils/macros.h>

namespace experimental
{
    template <std::size_t BlockSize>
    MemoryPoolFixedSize<BlockSize>::MemoryPoolFixedSize()
        : m_Head(nullptr)
        , m_MemoryBlock()
    {
    }

    template <std::size_t BlockSize>
    void MemoryPoolFixedSize<BlockSize>::ResetMemoryPool(std::size_t iNumberOfBlocks)
    {
        this->m_MemoryBlock.Allocate(iNumberOfBlocks);

        auto it = this->m_MemoryBlock.begin();

        this->m_Head = reinterpret_cast<Node*>(it++);
        Node* pNode = this->m_Head;

        for (; this->m_MemoryBlock.end() != it; ++it)
        {
            pNode->next = reinterpret_cast<Node*>(it);
            pNode = pNode->next;
        }

        pNode->next = nullptr;
    }

    template <std::size_t BlockSize>
    void* MemoryPoolFixedSize<BlockSize>::TakeBlock(std::size_t iRequestedSize)
    {
        if (likely((iRequestedSize <= BlockSize) && (nullptr != this->m_Head)))
        {
            Node* pNode = this->m_Head;

            this->m_Head = this->m_Head->next;

            return pNode;
        }
        else
        {
            return nullptr;
        }
    }

    template <std::size_t BlockSize>
    bool MemoryPoolFixedSize<BlockSize>::ReturnBlock(void* iBlock)
    {
        if (likely((this->m_MemoryBlock.begin() <= iBlock) && (iBlock <= this->m_MemoryBlock.end())))
        {
            Node* pPreviousHead = this->m_Head;

            this->m_Head = reinterpret_cast<Node*>(iBlock);
            this->m_Head->next = pPreviousHead;

            return true;
        }
        else
        {
            return false;
        }
    }
}
