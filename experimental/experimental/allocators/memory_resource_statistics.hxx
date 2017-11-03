#include <allocators/memory_resource_memory_pool_fixed_size.h>

namespace experimental
{
    template <typename StatisticsHandler>
    MemoryResourceStatistics<StatisticsHandler>::MemoryResourceStatistics(StatisticsHandler& iStatisticsHandler, pmr::memory_resource* iDefaultResource)
        : m_StatisticsHandler(iStatisticsHandler),
          m_DefaultResource(iDefaultResource)
    {
    }

    template <typename StatisticsHandler>
    void* MemoryResourceStatistics<StatisticsHandler>::do_allocate(std::size_t iBytes, std::size_t iAlignment)
    {
        this->m_StatisticsHandler.RecordAllocate(iBytes);

        return this->m_DefaultResource->allocate(iBytes, iAlignment);
    }

    template <typename StatisticsHandler>
    void MemoryResourceStatistics<StatisticsHandler>::do_deallocate(void* iPointer, std::size_t iBytes, std::size_t iAlignment)
    {
        this->m_StatisticsHandler.RecordDeallocate(iBytes);

        this->m_DefaultResource->deallocate(iPointer, iBytes, iAlignment);
    }

    template <typename StatisticsHandler>
    bool MemoryResourceStatistics<StatisticsHandler>::do_is_equal(const pmr::memory_resource& iMemoryResourceStatistics) const noexcept
    {
        return this == &iMemoryResourceStatistics;
    }
}
