#pragma once

#include <allocators/allocator_statistics.h>
#include <utils/statistics_checker.h>

namespace tests
{
    template <typename T, typename DefaultAllocator = std::allocator<T>>
    using AllocatorStatisticsChecker = experimental::AllocatorStatistics<T, StatisticsChecker, DefaultAllocator>;
}
