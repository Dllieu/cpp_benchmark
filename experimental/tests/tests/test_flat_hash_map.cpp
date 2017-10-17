#include <gtest/gtest.h>
#include <utils/stats_allocator.h>
#include <utils/memory_checker.h>
#include <flat_hash_map.hpp>
#include <cstddef>
#include <iostream>

using namespace tests;

namespace
{
    // Carefull as that map store std::pair<K, V> and not std::pair<const K, V> (not sure why yet)
    template <typename K, typename V>
    using FlatHashMapWithStatsAllocator = ska::flat_hash_map<K, V, std::hash<K>, std::equal_to<K>, StatsAllocator<std::pair<K, V>>>;
}

TEST(FlatHashMapTest, AllocationPattern)
{
    ska::flat_hash_map<std::uint64_t, std::uint64_t> f;
    static_assert(32 == sizeof(f));

    // struct { int8_t distance_from_desired ; union { T value; } }
    using BucketEntry = ska::detailv3::sherwood_v3_entry<std::pair<std::uint64_t, std::uint64_t>>;
    static_assert(24 == sizeof(BucketEntry));

    MemoryChecker memoryChecker;

    // Default one : Return index for the hash, or return next size which return the closest prime number in an hardcoded list (superior or =)
    ska::prime_number_hash_policy primeNumberHashPolicy;

    // Default one : Return index for the hash, or return next size which return the closest power of 2 in an hardcoded list (superior or =)
    //ska::power_of_two_hash_policy powerOf2HashPolicy;

    FlatHashMapWithStatsAllocator<std::uint64_t, std::uint64_t> fhm(memoryChecker);
    static_assert(40 == sizeof(fhm));
    EXPECT_EQ(0.5, fhm.max_load_factor());

    std::size_t elementToReserve = 10;
    std::size_t elementToReserveNormalized = static_cast<size_t>(std::ceil(elementToReserve / std::min(0.5, static_cast<double>(fhm.max_load_factor()))));
    EXPECT_EQ(20u, elementToReserveNormalized);

    primeNumberHashPolicy.next_size_over(elementToReserveNormalized);
    EXPECT_EQ(23u, elementToReserveNormalized);

    std::int8_t maximumLookup = std::max(static_cast<std::int8_t>(4), ska::detailv3::log2(elementToReserveNormalized));
    EXPECT_EQ(4, maximumLookup);

    memoryChecker.ExpectAllocate((maximumLookup + elementToReserveNormalized) * sizeof(BucketEntry));
    fhm.reserve(10);

    memoryChecker.IgnoreChecks();
}
