#include <allocators/allocator_memory_pool_fixed_size.h>
#include <allocators/memory_pool_fixed_size.h>
#include <utils/macros.h>

namespace experimental
{
    template <typename T, std::size_t BlockSize, typename DefaultAllocator>
    AllocatorMemoryPoolFixedSize<T, BlockSize, DefaultAllocator>::AllocatorMemoryPoolFixedSize(MemoryPoolFixedSize<BlockSize>& iMemoryPool, DefaultAllocator&& iDefaultAllocator) noexcept
        : DefaultAllocator(std::move(iDefaultAllocator)),
          m_MemoryPool(iMemoryPool)
    {
        static_assert(sizeof(T) <= BlockSize, "T is too large for this memory pool");
    }

    template <typename T, std::size_t BlockSize, typename DefaultAllocator>
    template <typename U>
    AllocatorMemoryPoolFixedSize<T, BlockSize, DefaultAllocator>::AllocatorMemoryPoolFixedSize(const AllocatorMemoryPoolFixedSize<U, BlockSize, typename DefaultAllocator::template rebind<U>::other>& iAllocatorMemoryPoolFixedSize) noexcept
        : DefaultAllocator(iAllocatorMemoryPoolFixedSize),
          m_MemoryPool(iAllocatorMemoryPoolFixedSize.m_MemoryPool)
    {
        static_assert(sizeof(U) <= BlockSize, "U is too large for this memory pool");
    }

    template <typename T, std::size_t BlockSize, typename DefaultAllocator>
    void AllocatorMemoryPoolFixedSize<T, BlockSize, DefaultAllocator>::ResetMemoryPool(std::size_t iNumberOfBlocks)
    {
        this->m_MemoryPool.ResetMemoryPool(iNumberOfBlocks);
    }

    template <typename T, std::size_t BlockSize, typename DefaultAllocator>
    auto AllocatorMemoryPoolFixedSize<T, BlockSize, DefaultAllocator>::allocate(std::size_t iNumberOfElements) -> value_type*
    {
        std::size_t sizeToAllocate = iNumberOfElements * sizeof(T);

        value_type* pResult = reinterpret_cast<value_type*>(this->m_MemoryPool.TakeBlock(sizeToAllocate));

        if (likely(nullptr != pResult))
        {
            return pResult;
        }

        return DefaultAllocator::allocate(iNumberOfElements);
    }

    template <typename T, std::size_t BlockSize, typename DefaultAllocator>
    void AllocatorMemoryPoolFixedSize<T, BlockSize, DefaultAllocator>::deallocate(value_type* iPointer, std::size_t iNumberOfElements) noexcept
    {
        if (likely(true == this->m_MemoryPool.ReturnBlock(iPointer)))
        {
            return;
        }

        DefaultAllocator::deallocate(iPointer, iNumberOfElements);
    }

    template <typename T, typename U, std::size_t BlockSize, typename DefaultAllocator>
    bool operator==(const AllocatorMemoryPoolFixedSize<T, BlockSize, DefaultAllocator>& /*unused*/, const AllocatorMemoryPoolFixedSize<U, BlockSize, DefaultAllocator>& /*unused*/) noexcept
    {
        return true;
    }

    template <typename T, typename U, std::size_t BlockSize, typename DefaultAllocator>
    bool operator!=(const AllocatorMemoryPoolFixedSize<T, BlockSize, DefaultAllocator>& iAlloc1, const AllocatorMemoryPoolFixedSize<U, BlockSize, DefaultAllocator>& iAlloc2) noexcept
    {
        return false == (iAlloc1 == iAlloc2);
    }
}
