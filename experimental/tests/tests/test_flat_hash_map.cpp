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

    // 1 - Reserve

    std::size_t elementToReserve = 10;
    std::size_t numberOfBuckets = static_cast<size_t>(std::ceil(elementToReserve / std::min(0.5, static_cast<double>(fhm.max_load_factor()))));
    EXPECT_EQ(20u, numberOfBuckets);

    // 2 - Then call rehash(numberOfBuckets)

    std::int8_t newPrimeIndex = primeNumberHashPolicy.next_size_over(numberOfBuckets);
    EXPECT_EQ(8, newPrimeIndex); // 8 -> &mod23 : next time we request the index (i.e. mod_table[newPrimeIndex](hash(key)))
    EXPECT_EQ(23u, numberOfBuckets);

    // Not sure what the purpose of maximumLookup
    std::int8_t maximumLookup = std::max(static_cast<std::int8_t>(4), ska::detailv3::log2(numberOfBuckets));
    EXPECT_EQ(4, maximumLookup);

    std::size_t allocatedBucketNumber = maximumLookup + numberOfBuckets;
    memoryChecker.ExpectAllocate(allocatedBucketNumber * sizeof(BucketEntry));
    // Init all the BucketEntry.distance_from_desired to -1, except the last one that is reserved and init to 0 (so real BucketEntry available is allocatedBucketNumber - 1, but BufferEntry + maximumLookup)
    // BucketEntry not used : -1 == distance_from_desired
    fhm.reserve(10); // Around L647

    std::size_t previousAllocatedBucketNumber = allocatedBucketNumber;
    EXPECT_EQ(numberOfBuckets, fhm.bucket_count());

    std::size_t i = 0;
    for (; i < 11; ++i)
    {
        // EntryPointer + index
        // search if key exist -> for (int i = 0; i <= entry->distance_from_desired; ++i, ++entry) // distance_from_desired init with -1, entry cannot dangle as the last entry finish have distance_from_desired 0
        // if not -> if entry is empty (distance_from_desired = -1) : set the data within entry and return
        fhm.emplace(i, i);
    }

    // if key do  not exist -> if (numberOfElement + 1) / numberOfBuckets > max_load_factor -> grow()
    // (e.g. 12 / 23 > 0.5 -> grow())
    // rehash with elementToReserve = 2 * bucket_count();
    numberOfBuckets = 2 * numberOfBuckets;
    EXPECT_EQ(46u, numberOfBuckets);

    newPrimeIndex = primeNumberHashPolicy.next_size_over(numberOfBuckets);
    EXPECT_EQ(47u, numberOfBuckets);

    maximumLookup = std::max(static_cast<std::int8_t>(4), ska::detailv3::log2(numberOfBuckets));
    EXPECT_EQ(5, maximumLookup);

    allocatedBucketNumber = maximumLookup + numberOfBuckets;
    memoryChecker.ExpectAllocate(allocatedBucketNumber * sizeof(BucketEntry));
    memoryChecker.ExpectDeallocate(previousAllocatedBucketNumber * sizeof(BucketEntry));
    fhm.emplace(i, i);

    memoryChecker.IgnoreChecks();
}

TEST(FlatHashMapTest, Retrieval)
{
    ska::flat_hash_map<std::uint64_t, std::uint64_t> f;

    std::size_t maximum = 1000000;
    for (std::uint64_t i = 0; i < maximum; ++i)
    {
        f.emplace(i, i);
    }

    for (std::uint64_t i = 0; i < maximum; ++i)
    {
        EXPECT_EQ(i, f[i]);
    }
}
