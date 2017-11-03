#pragma once

#include <cstdint>
#include <memory>

namespace experimental
{
    template <typename T, typename StatisticsHandler, typename DefaultAllocator = std::allocator<T>>
    class AllocatorStatistics : public DefaultAllocator
    {
    public:
        using value_type = typename DefaultAllocator::value_type;

        AllocatorStatistics() = delete;
        AllocatorStatistics(StatisticsHandler& iStatisticsHandler) noexcept;
        AllocatorStatistics(const AllocatorStatistics&) = default;
        AllocatorStatistics(AllocatorStatistics&&) noexcept = default;
        AllocatorStatistics& operator=(const AllocatorStatistics&) = delete;
        AllocatorStatistics& operator=(AllocatorStatistics&&) = delete;
        ~AllocatorStatistics() noexcept = default;

        template <typename U>
        struct rebind
        {
            using other = AllocatorStatistics<U, StatisticsHandler, typename DefaultAllocator::template rebind<U>::other>;
        };

        template <typename U>
        AllocatorStatistics(const AllocatorStatistics<U, StatisticsHandler, typename DefaultAllocator::template rebind<U>::other>& iAllocatorStatistics) noexcept;

        value_type* allocate(std::size_t iNumberOfElements);
        void deallocate(value_type* iPointer, std::size_t iNumberOfElements) noexcept;

    private:
        StatisticsHandler& m_StatisticsHandler;

        template <typename, typename, typename>
        friend class AllocatorStatistics;
    };
}

#include <allocators/allocator_statistics.hxx>