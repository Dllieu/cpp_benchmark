#include <benchmark/benchmark.h>
#include <cmath>
#include <numeric>
#include <set>
#include <unordered_set>
#include <utils/cache_information.h>

namespace
{
    void CacheArrayOfArrayTraversal_ColumnWiseBenchmark(benchmark::State& iState)
    {
        std::size_t dimension = iState.range(0);
        std::vector<std::vector<std::int8_t>> arrayOfArray(dimension, std::vector<std::int8_t>(dimension));
        for (std::size_t i = 0; i < arrayOfArray.size(); ++i)
        {
            std::iota(std::begin(arrayOfArray[i]), std::end(arrayOfArray[i]), i * dimension);
        }

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            for (std::size_t column = 0; column < dimension; ++column)
            {
                for (std::size_t row = 0; row < dimension; ++row)
                {
                    benchmark::DoNotOptimize(n += arrayOfArray[row][column]);
                }
            }
        }

        benchmark::DoNotOptimize(n);
    }

    void CacheArrayOfArrayTraversal_RowWiseBenchmark(benchmark::State& iState)
    {
        std::size_t dimension = iState.range(0);
        std::vector<std::vector<std::int8_t>> arrayOfArray(dimension, std::vector<std::int8_t>(dimension));
        for (std::size_t i = 0; i < arrayOfArray.size(); ++i)
        {
            std::iota(std::begin(arrayOfArray[i]), std::end(arrayOfArray[i]), i * dimension);
        }

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            for (std::size_t row = 0; row < dimension; ++row)
            {
                for (std::size_t column = 0; column < dimension; ++column)
                {
                    benchmark::DoNotOptimize(n += arrayOfArray[row][column]);
                }
            }
        }

        benchmark::DoNotOptimize(n);
    }

    void CacheArrayOfArrayTraversal_HashComputationColumnWiseBenchmark(benchmark::State& iState)
    {
        std::size_t dimension = iState.range(0);
        std::vector<std::vector<std::int8_t>> arrayOfArray(dimension, std::vector<std::int8_t>(dimension));
        for (std::size_t i = 0; i < arrayOfArray.size(); ++i)
        {
            std::iota(std::begin(arrayOfArray[i]), std::end(arrayOfArray[i]), i * dimension);
        }

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            for (std::size_t column = 0; column < dimension; ++column)
            {
                for (std::size_t row = 0; row < dimension; ++row)
                {
                    benchmark::DoNotOptimize(n += arrayOfArray[row][column] + std::sqrt(std::hash<std::int8_t>()(row * dimension + column)));
                }
            }
        }

        benchmark::DoNotOptimize(n);
    }

    void CacheArrayOfArrayTraversal_HashComputationRowWiseBenchmark(benchmark::State& iState)
    {
        std::size_t dimension = iState.range(0);
        std::vector<std::vector<std::int8_t>> arrayOfArray(dimension, std::vector<std::int8_t>(dimension));
        for (std::size_t i = 0; i < arrayOfArray.size(); ++i)
        {
            std::iota(std::begin(arrayOfArray[i]), std::end(arrayOfArray[i]), i * dimension);
        }

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            for (std::size_t row = 0; row < dimension; ++row)
            {
                for (std::size_t column = 0; column < dimension; ++column)
                {
                    benchmark::DoNotOptimize(n += arrayOfArray[row][column] + std::sqrt(std::hash<std::int8_t>()(row * dimension + column)));
                }
            }
        }

        benchmark::DoNotOptimize(n);
    }
}

BENCHMARK(CacheArrayOfArrayTraversal_ColumnWiseBenchmark)->RangeMultiplier(2)->Range(512, 64_KB);                // NOLINT
BENCHMARK(CacheArrayOfArrayTraversal_RowWiseBenchmark)->RangeMultiplier(2)->Range(512, 64_KB);                   // NOLINT
BENCHMARK(CacheArrayOfArrayTraversal_HashComputationColumnWiseBenchmark)->RangeMultiplier(2)->Range(512, 64_KB); // NOLINT
BENCHMARK(CacheArrayOfArrayTraversal_HashComputationRowWiseBenchmark)->RangeMultiplier(2)->Range(512, 64_KB);    // NOLINT
