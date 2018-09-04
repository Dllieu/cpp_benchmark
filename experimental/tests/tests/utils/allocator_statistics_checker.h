#pragma once

#include <allocators/allocator_statistics.h>
#include <allocators/memory_resource_statistics.h>
#include <tests/utils/statistics_checker.h>

namespace tests
{
    template <typename T, typename DefaultAllocator = std::allocator<T>>
    using AllocatorStatisticsChecker = experimental::AllocatorStatistics<T, StatisticsChecker, DefaultAllocator>;

    using MemoryResourceStatisticsChecker = experimental::MemoryResourceStatistics<StatisticsChecker>;
}
