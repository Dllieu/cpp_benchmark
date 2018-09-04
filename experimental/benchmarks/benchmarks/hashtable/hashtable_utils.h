#pragma once

#include <cstddef>
#include <vector>

namespace benchmarks
{
    template <typename HashTableT, typename PostInitF>
    HashTableT CreateHashTableWithRandomElements(std::size_t iNumberElements, PostInitF&& iPostInitFunctor);

    template <typename T>
    std::size_t NumberOfContainerToOverflowL3Cache(std::size_t iNumberOfElements);

    template <typename HashTableT, typename PostInitF>
    std::vector<HashTableT> CreateHashTablesWithRandomElementsToOverflowL3Cache(std::size_t iNumberElements, PostInitF&& iPostInitFunctor);

    template <typename HashTableT>
    std::vector<typename HashTableT::key_type> CreateVectorFromHashTableKeysShuffled(const HashTableT& iHashTable);
}

#include <benchmarks/hashtable/hashtable_utils.hxx>
