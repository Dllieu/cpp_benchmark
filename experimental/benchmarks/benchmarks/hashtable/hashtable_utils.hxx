#include <benchmarks/hashtable/hashtable_utils.h>
#include <cstddef>
#include <random>
#include <utils/cache_information.h>
#include <utils/traits.h>

namespace benchmarks
{
    template <typename HashTableT, typename PostInitF>
    HashTableT CreateHashTableWithRandomElements(std::size_t iNumberElements, PostInitF&& iPostInitFunctor)
    {
        using T = typename HashTableT::key_type;

        std::uniform_int_distribution<T> randomDistribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        std::mt19937_64 generator;

        HashTableT hashTable;
        iPostInitFunctor(hashTable);

        while (hashTable.size() != iNumberElements)
        {
            hashTable.emplace(randomDistribution(generator), hashTable.size());
        }

        return hashTable;
    }

    template <typename T>
    std::size_t NumberOfContainerToOverflowL3Cache(std::size_t iNumberOfElements)
    {
        return std::max(std::size_t(2), (3u * experimental::enum_cast(experimental::CacheSize::L3)) / (sizeof(T) * iNumberOfElements));
    }

    template <typename HashTableT, typename PostInitF>
    std::vector<HashTableT> CreateHashTablesWithRandomElementsToOverflowL3Cache(std::size_t iNumberElements, PostInitF&& iPostInitFunctor)
    {
        std::size_t numberOfContainerToOverflowL3Cache = NumberOfContainerToOverflowL3Cache<typename HashTableT::value_type>(iNumberElements);
        HashTableT hashTable = CreateHashTableWithRandomElements<HashTableT>(iNumberElements, iPostInitFunctor);

        std::vector<HashTableT> hashTables;
        for (std::size_t i = 0; i < numberOfContainerToOverflowL3Cache; ++i)
        {
            hashTables.push_back(hashTable);
        }

        return hashTables;
    }

    template <typename HashTableT>
    std::vector<typename HashTableT::key_type> CreateVectorFromHashTableKeysShuffled(const HashTableT& iHashTable)
    {
        std::vector<typename HashTableT::key_type> result(iHashTable.size());
        std::transform(std::cbegin(iHashTable), std::cend(iHashTable), std::begin(result), [](const auto& iPair) { return iPair.first; });

        std::shuffle(std::begin(result), std::end(result), std::mt19937_64(235432876));

        return result;
    }
}
