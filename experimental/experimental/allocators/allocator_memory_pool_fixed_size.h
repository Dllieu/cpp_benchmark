#pragma once

#include <cstdint>

namespace experimental
{
    template <std::size_t BlockSize>
    class MemoryPoolFixedSize;

    template <typename T, std::size_t BlockSize>
    class AllocatorMemoryPoolFixedSize
    {
    public:
        using value_type = T;

        AllocatorMemoryPoolFixedSize() = delete;
        AllocatorMemoryPoolFixedSize(MemoryPoolFixedSize<BlockSize>& iMemoryPool) noexcept;
        AllocatorMemoryPoolFixedSize(const AllocatorMemoryPoolFixedSize&) = default;
        AllocatorMemoryPoolFixedSize(AllocatorMemoryPoolFixedSize&&) noexcept = default;
        AllocatorMemoryPoolFixedSize& operator=(const AllocatorMemoryPoolFixedSize&) = delete;
        AllocatorMemoryPoolFixedSize& operator=(AllocatorMemoryPoolFixedSize&&) = delete;
        ~AllocatorMemoryPoolFixedSize() noexcept = default;

        template <typename U>
        struct rebind
        {
            using other = AllocatorMemoryPoolFixedSize<U, BlockSize>;
        };

        template <typename U>
        AllocatorMemoryPoolFixedSize(const AllocatorMemoryPoolFixedSize<U, BlockSize>& iAllocatorMemoryPoolFixedSize) noexcept;

        void ResetMemoryPool(std::size_t iNumberOfBlocks);

        value_type* allocate(std::size_t iNumberOfElements);
        void deallocate(value_type* iPointer, std::size_t iNumberOfElements) noexcept;

    private:
        MemoryPoolFixedSize<BlockSize>& m_MemoryPool;

        template <typename U, std::size_t>
        friend class AllocatorMemoryPoolFixedSize;
    };

    template <typename T, typename U, std::size_t BlockSize>
    bool operator==(const AllocatorMemoryPoolFixedSize<T, BlockSize>&, const AllocatorMemoryPoolFixedSize<U, BlockSize>&) noexcept;

    template <typename T, typename U, std::size_t BlockSize>
    bool operator!=(const AllocatorMemoryPoolFixedSize<T, BlockSize>&, const AllocatorMemoryPoolFixedSize<U, BlockSize>&) noexcept;
}

#include <allocators/allocator_memory_pool_fixed_size.hxx>
