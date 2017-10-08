#include <allocators/memory_pool_resource.h>

namespace allocators
{
    template <std::size_t BlockSize>
    MemoryPoolResource<BlockSize>::MemoryPoolResource(std::size_t iNumberOfBlocks, pmr::memory_resource* iDefaultResource)
        : m_MemoryPool(iNumberOfBlocks),
          m_DefaultResource(iDefaultResource)
    {
    }

    template <std::size_t BlockSize>
    void* MemoryPoolResource<BlockSize>::do_allocate(std::size_t iBytes, std::size_t iAligment)
    {
        if (likely(iBytes <= BlockSize))
        {
            std::byte* pBlock = this->m_MemoryPool.GetFreeBlock();

            if (likely(nullptr != pBlock))
            {
                return pBlock;
            }

            iBytes = BlockSize;
        }

        return this->m_DefaultResource->do_allocate(iBytes, iAligment);
    }

    template <std::size_t BlockSize>
    void MemoryPoolResource<BlockSize>::do_deallocate(void* iPointer, std::size_t iBytes, std::size_t iAligment)
    {
        if (likely(iBytes <= BlockSize))
        {
            this->m_MemoryPool.AddFreeBlock(reinterpret_cast<std::byte*>(iPointer));
        }
        else
        {
            this->m_DefaultResource->deallocate(iPointer, iBytes, iAligment);
        }
    }

    template <std::size_t BlockSize>
    bool MemoryPoolResource<BlockSize>::do_is_equal(const pmr::memory_resource& iMemoryResource)
    {
        return this == &iMemoryResource;
    }
}
