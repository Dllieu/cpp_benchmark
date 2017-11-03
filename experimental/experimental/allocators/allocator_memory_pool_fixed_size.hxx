#include <allocators/allocator_memory_pool_fixed_size.h>
#include <allocators/memory_pool_fixed_size.h>
#include <utils/macros.h>

namespace experimental
{
    template <typename T, std::size_t BlockSize>
    AllocatorMemoryPoolFixedSize<T, BlockSize>::AllocatorMemoryPoolFixedSize(MemoryPoolFixedSize<BlockSize>& iMemoryPool) noexcept
        : m_MemoryPool(iMemoryPool)
    {
        static_assert(sizeof(T) <= BlockSize, "T is too large for this memory pool");
    }

    template <typename T, std::size_t BlockSize>
    template <typename U>
    AllocatorMemoryPoolFixedSize<T, BlockSize>::AllocatorMemoryPoolFixedSize(const AllocatorMemoryPoolFixedSize<U, BlockSize>& iAllocatorMemoryPoolFixedSize) noexcept
        : m_MemoryPool(iAllocatorMemoryPoolFixedSize.m_MemoryPool)
    {
        static_assert(sizeof(U) <= BlockSize, "U is too large for this memory pool");
    }

    template <typename T, std::size_t BlockSize>
    void AllocatorMemoryPoolFixedSize<T, BlockSize>::ResetMemoryPool(std::size_t iNumberOfBlocks)
    {
        this->m_MemoryPool.ResetMemoryPool(iNumberOfBlocks);
    }

    template <typename T, std::size_t BlockSize>
    auto AllocatorMemoryPoolFixedSize<T, BlockSize>::allocate(std::size_t iNumberOfElements) -> value_type*
    {
        std::size_t sizeToAllocate = iNumberOfElements * sizeof(T);

        value_type* pResult = reinterpret_cast<value_type*>(this->m_MemoryPool.TakeBlock(sizeToAllocate));

        if (likely(nullptr != pResult))
        {
            return pResult;
        }

        return static_cast<value_type*>(::operator new (sizeToAllocate));
    }

    template <typename T, std::size_t BlockSize>
    void AllocatorMemoryPoolFixedSize<T, BlockSize>::deallocate(value_type* iPointer, std::size_t iNumberOfElements) noexcept
    {
        if (likely(true == this->m_MemoryPool.ReturnBlock(iPointer)))
        {
            return;
        }

        ::operator delete (iPointer);
    }

    template <typename T, typename U, std::size_t BlockSize>
    bool operator==(const AllocatorMemoryPoolFixedSize<T, BlockSize>&, const AllocatorMemoryPoolFixedSize<U, BlockSize>&) noexcept
    {
        return true;
    }

    template <typename T, typename U, std::size_t BlockSize>
    bool operator!=(const AllocatorMemoryPoolFixedSize<T, BlockSize>& iAlloc1, const AllocatorMemoryPoolFixedSize<U, BlockSize>& iAlloc2) noexcept
    {
        return false == (iAlloc1 == iAlloc2);
    }
}
