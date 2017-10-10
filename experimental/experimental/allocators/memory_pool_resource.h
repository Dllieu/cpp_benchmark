#pragma once

#include <experimental/memory_resource>
#include <cstddef>
#include <allocators/memory_pool_linear_growth.h>

// Do not care to make the namespace global
namespace pmr = std::experimental::fundamentals_v2::pmr;

namespace allocators
{
    template <std::size_t BlockSize>
    class MemoryPoolResource : pmr::memory_resource
    {
    public:
        explicit MemoryPoolResource(std::size_t iNumberOfBlocks, pmr::memory_resource* iDefaultResource = pmr::get_default_resource());

    protected:
        virtual void* do_allocate(std::size_t iBytes, std::size_t iAligment) override;
        virtual void do_deallocate(void* iPointer, std::size_t iBytes, std::size_t iAligment) override;
        virtual bool do_is_equal(const pmr::memory_resource& iMemoryResource) override;

    private:
        MemoryPoolLinearGrowth<BlockSize> m_MemoryPool;
        pmr::memory_resource* m_DefaultResource;
    };
}

#include <allocators/memory_pool_resource.hxx>
