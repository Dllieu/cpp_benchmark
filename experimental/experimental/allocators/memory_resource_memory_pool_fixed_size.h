#pragma once

#include <experimental/memory_resource>
#include <allocators/memory_pool_fixed_size.h>
#include <cstdint>

// Do not care to make the namespace global
namespace pmr = std::experimental::fundamentals_v2::pmr;

namespace experimental
{
    template <std::size_t BlockSize>
    class MemoryResourceMemoryPoolFixedSize : public pmr::memory_resource
    {
    public:
        explicit MemoryResourceMemoryPoolFixedSize(MemoryPoolFixedSize<BlockSize>& iMemoryPool, pmr::memory_resource* iDefaultResource = pmr::get_default_resource());

        void ResetMemoryPool(std::size_t iNumberOfBlocks);

    protected:
         void* do_allocate(std::size_t iBytes, std::size_t iAlignment) override;
         void do_deallocate(void* iPointer, std::size_t iBytes, std::size_t iAlignment) override;
         bool do_is_equal(const pmr::memory_resource& iMemoryResourceMemoryPoolFixedSize) const noexcept override;

    private:
         MemoryPoolFixedSize<BlockSize>& m_MemoryPool;
         pmr::memory_resource* m_DefaultResource;
    };
}

#include <allocators/memory_resource_memory_pool_fixed_size.hxx>
