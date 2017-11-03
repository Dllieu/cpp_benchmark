#include <allocators/allocator_statistics.h>

namespace experimental
{
    template <typename T, typename StatisticsHandler, typename DefaultAllocator>
    AllocatorStatistics<T, StatisticsHandler, DefaultAllocator>::AllocatorStatistics(StatisticsHandler& iStatisticsHandler) noexcept
        : m_StatisticsHandler(iStatisticsHandler)
    {
    }

    template <typename T, typename StatisticsHandler, typename DefaultAllocator>
    template <typename U>
    AllocatorStatistics<T, StatisticsHandler, DefaultAllocator>::AllocatorStatistics(const AllocatorStatistics<U, StatisticsHandler, typename DefaultAllocator::template rebind<U>::other>& iAllocatorStatistics) noexcept
        : m_StatisticsHandler(iAllocatorStatistics.m_StatisticsHandler)
    {
    }

    template <typename T, typename StatisticsHandler, typename DefaultAllocator>
    auto AllocatorStatistics<T, StatisticsHandler, DefaultAllocator>::allocate(std::size_t iNumberOfElements) -> value_type*
    {
        this->m_StatisticsHandler.RecordAllocate(iNumberOfElements * sizeof(T));

        return DefaultAllocator::allocate(iNumberOfElements);
    }

    template <typename T, typename StatisticsHandler, typename DefaultAllocator>
    void AllocatorStatistics<T, StatisticsHandler, DefaultAllocator>::deallocate(value_type* iPointer, std::size_t iNumberOfElements) noexcept
    {
        this->m_StatisticsHandler.RecordDeallocate(iNumberOfElements * sizeof(T));

        DefaultAllocator::deallocate(iPointer, iNumberOfElements);
    }

    template <typename T, typename U, typename StatisticsHandler, typename DefaultAllocator>
    bool operator==(const AllocatorStatistics<T, StatisticsHandler, DefaultAllocator>&, const AllocatorStatistics<U, StatisticsHandler, DefaultAllocator>&) noexcept
    {
        return true;
    }

    template <typename T, typename U, typename StatisticsHandler, typename DefaultAllocator>
    bool operator!=(const AllocatorStatistics<T, StatisticsHandler, DefaultAllocator>& iAlloc1, const AllocatorStatistics<U, StatisticsHandler, DefaultAllocator>& iAlloc2) noexcept
    {
        return false == (iAlloc1 == iAlloc2);
    }
}
