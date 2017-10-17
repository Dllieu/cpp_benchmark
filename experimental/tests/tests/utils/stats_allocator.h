#pragma once

#include <cstddef>

namespace tests
{
    class MemoryChecker;

    template <typename T>
    class StatsAllocator
    {
    public:
        using value_type = T;

        StatsAllocator(MemoryChecker& iMemoryStats) noexcept;

        template <typename U>
        explicit StatsAllocator(const StatsAllocator<U>& iAllocator);

        value_type* allocate(std::size_t iNumber);
        void deallocate(value_type* iPointer, std::size_t iNumber) noexcept;

        MemoryChecker& m_MemoryChecker;
    };

    template <typename T, typename U>
    bool operator==(const StatsAllocator<T>&, const StatsAllocator<U>&) noexcept;

    template <typename T, typename U>
    bool operator!=(StatsAllocator<T> const& x, StatsAllocator<U> const& y) noexcept;
}

#include <utils/stats_allocator.hxx>
