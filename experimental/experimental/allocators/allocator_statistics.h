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
        AllocatorStatistics(StatisticsHandler& iStatisticsHandler) noexcept; // NOLINT
        AllocatorStatistics(const AllocatorStatistics&) = default;
        AllocatorStatistics(AllocatorStatistics&&) noexcept = default; // NOLINT (bug)
        AllocatorStatistics& operator=(const AllocatorStatistics&) = delete;
        AllocatorStatistics& operator=(AllocatorStatistics&&) = delete;
        ~AllocatorStatistics() noexcept = default;

        template <typename U>
        using rebind_alloc = typename std::allocator_traits<DefaultAllocator>::template rebind_alloc<U>;

        template <typename U>
        struct rebind
        {
            using other = AllocatorStatistics<U, StatisticsHandler, rebind_alloc<U>>;
        };

        template <typename U>
        AllocatorStatistics(const AllocatorStatistics<U, StatisticsHandler, rebind_alloc<U>>& iAllocatorStatistics) noexcept;

        value_type* allocate(std::size_t iNumberOfElements);
        void deallocate(value_type* iPointer, std::size_t iNumberOfElements) noexcept;

    private:
        StatisticsHandler& m_StatisticsHandler;

        template <typename, typename, typename>
        friend class AllocatorStatistics;
    };
}

#include <allocators/allocator_statistics.hxx>
