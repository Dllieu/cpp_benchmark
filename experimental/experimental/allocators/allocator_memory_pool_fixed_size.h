#pragma once

#include <cstdint>
#include <memory>

namespace experimental
{
    template <std::size_t BlockSize>
    class MemoryPoolFixedSize;

    template <typename T, std::size_t BlockSize, typename DefaultAllocator = std::allocator<T>>
    class AllocatorMemoryPoolFixedSize : public DefaultAllocator
    {
    public:
        using value_type = typename DefaultAllocator::value_type;

        AllocatorMemoryPoolFixedSize() = delete;
        AllocatorMemoryPoolFixedSize(MemoryPoolFixedSize<BlockSize>& iMemoryPool, DefaultAllocator&& iDefaultAllocator = DefaultAllocator()) noexcept; // NOLINT
        AllocatorMemoryPoolFixedSize(const AllocatorMemoryPoolFixedSize&) = default;
        AllocatorMemoryPoolFixedSize(AllocatorMemoryPoolFixedSize&&) noexcept = default; // NOLINT (bug)
        AllocatorMemoryPoolFixedSize& operator=(const AllocatorMemoryPoolFixedSize&) = delete;
        AllocatorMemoryPoolFixedSize& operator=(AllocatorMemoryPoolFixedSize&&) = delete;
        ~AllocatorMemoryPoolFixedSize() noexcept = default;

        template <typename U>
        struct rebind
        {
            using other = AllocatorMemoryPoolFixedSize<U, BlockSize, typename std::allocator_traits<DefaultAllocator>::template rebind_alloc<U>>;
        };

        template <typename U>
        AllocatorMemoryPoolFixedSize(const AllocatorMemoryPoolFixedSize<U, BlockSize, typename std::allocator_traits<DefaultAllocator>::template rebind_alloc<U>>& iAllocatorMemoryPoolFixedSize) noexcept; // NOLINT

        void ResetMemoryPool(std::size_t iNumberOfBlocks);

        value_type* allocate(std::size_t iNumberOfElements);
        void deallocate(value_type* iPointer, std::size_t iNumberOfElements) noexcept;

    private:
        MemoryPoolFixedSize<BlockSize>& m_MemoryPool;

        template <typename, std::size_t, typename>
        friend class AllocatorMemoryPoolFixedSize;
    };

    template <typename T, typename U, std::size_t BlockSize, typename DefaultAllocator>
    bool operator==(const AllocatorMemoryPoolFixedSize<T, BlockSize, DefaultAllocator>& /*unused*/, const AllocatorMemoryPoolFixedSize<U, BlockSize, DefaultAllocator>& /*unused*/) noexcept;

    template <typename T, typename U, std::size_t BlockSize, typename DefaultAllocator>
    bool operator!=(const AllocatorMemoryPoolFixedSize<T, BlockSize, DefaultAllocator>& iAlloc1, const AllocatorMemoryPoolFixedSize<U, BlockSize, DefaultAllocator>& iAlloc2) noexcept;
}

#include <allocators/allocator_memory_pool_fixed_size.hxx>
