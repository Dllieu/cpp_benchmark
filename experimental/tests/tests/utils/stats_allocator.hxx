#include <utils/stats_allocator.h>
#include <utils/memory_checker.h>
#include <cstddef>

namespace tests
{
    template <typename T>
    StatsAllocator<T>::StatsAllocator(MemoryChecker& iMemoryStats) noexcept
        : m_MemoryChecker(iMemoryStats)
    {
    }

    template <typename T>
    template <typename U>
    StatsAllocator<T>::StatsAllocator(const StatsAllocator<U>& iAllocator)
        : m_MemoryChecker(iAllocator.m_MemoryChecker)
    {
        // Must be careful in case of stateful allocator used in node based container as this constructor might be called for each node allocation
    }

    template <typename T>
    auto StatsAllocator<T>::allocate(std::size_t iNumber) -> value_type*
    {
        this->m_MemoryChecker.CheckAllocate(iNumber * sizeof(value_type));

        return static_cast<value_type*>(::operator new (iNumber * sizeof(value_type)));
    }

    template <typename T>
    void StatsAllocator<T>::deallocate(value_type* iPointer, std::size_t iNumber) noexcept
    {
        this->m_MemoryChecker.CheckDeallocate(iNumber * sizeof(value_type));

        ::operator delete(iPointer);
    }

    template <typename T, typename U>
    bool operator==(const StatsAllocator<T>&, const StatsAllocator<U>&) noexcept
    {
        return true;
    }

    template <typename T, typename U>
    bool operator!=(StatsAllocator<T> const& x, StatsAllocator<U> const& y) noexcept
    {
        return false == (x == y);
    }
}
