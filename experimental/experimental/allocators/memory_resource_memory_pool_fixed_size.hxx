#include <allocators/memory_resource_memory_pool_fixed_size.h>

namespace experimental
{
    template <std::size_t BlockSize>
    MemoryResourceMemoryPoolFixedSize<BlockSize>::MemoryResourceMemoryPoolFixedSize(MemoryPoolFixedSize<BlockSize>& iMemoryPool, pmr::memory_resource* iDefaultResource) : m_MemoryPool(iMemoryPool), m_DefaultResource(iDefaultResource)
    {
    }

    template <std::size_t BlockSize>
    void MemoryResourceMemoryPoolFixedSize<BlockSize>::ResetMemoryPool(std::size_t iNumberOfBlocks)
    {
        this->m_MemoryPool.ResetMemoryPool(iNumberOfBlocks);
    }

    template <std::size_t BlockSize>
    void* MemoryResourceMemoryPoolFixedSize<BlockSize>::do_allocate(std::size_t iBytes, std::size_t iAlignment)
    {
        void* pResult = this->m_MemoryPool.TakeBlock(iBytes);

        if (likely(nullptr != pResult))
        {
            return pResult;
        }

        return this->m_DefaultResource->allocate(iBytes, iAlignment);
    }

    template <std::size_t BlockSize>
    void MemoryResourceMemoryPoolFixedSize<BlockSize>::do_deallocate(void* iPointer, std::size_t iBytes, std::size_t iAlignment)
    {
        if (likely(true == this->m_MemoryPool.ReturnBlock(iPointer)))
        {
            return;
        }

        this->m_DefaultResource->deallocate(iPointer, iBytes, iAlignment);
    }

    template <std::size_t BlockSize>
    bool MemoryResourceMemoryPoolFixedSize<BlockSize>::do_is_equal(const pmr::memory_resource& iMemoryResourceMemoryPoolFixedSize) const noexcept
    {
        return this == &iMemoryResourceMemoryPoolFixedSize;
    }
}
