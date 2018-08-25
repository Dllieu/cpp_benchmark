#pragma once

#include <allocators/memory_pool_fixed_size.h>
#include <cstdint>
#include <experimental/memory_resource>

// Do not care to make the namespace global
namespace pmr = std::experimental::fundamentals_v2::pmr;

namespace experimental
{
    template <typename StatisticsHandler>
    class MemoryResourceStatistics : public pmr::memory_resource
    {
    public:
        explicit MemoryResourceStatistics(StatisticsHandler& iStatisticsHandler, pmr::memory_resource* iDefaultResource = pmr::get_default_resource());

    protected:
        void* do_allocate(std::size_t iBytes, std::size_t iAlignment) override;
        void do_deallocate(void* iPointer, std::size_t iBytes, std::size_t iAlignment) override;
        bool do_is_equal(const pmr::memory_resource& iMemoryResourceStatistics) const noexcept override;

    private:
        StatisticsHandler& m_StatisticsHandler;
        pmr::memory_resource* m_DefaultResource;
    };
}

#include <allocators/memory_resource_statistics.hxx>
