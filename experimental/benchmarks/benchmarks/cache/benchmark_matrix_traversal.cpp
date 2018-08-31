#include <benchmark/benchmark.h>
#include <cmath>
#include <numeric>
#include <set>
#include <unordered_set>
#include <utils/cache_information.h>

namespace
{
    void CacheMatrixTraversal_ArrayOfArrayColumnWiseBenchmark(benchmark::State& iState)
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

    void CacheMatrixTraversal_ArrayOfArrayRowWiseBenchmark(benchmark::State& iState)
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

    void CacheMatrixTraversal_ArrayOfArrayHashComputationColumnWiseBenchmark(benchmark::State& iState)
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

    void CacheMatrixTraversal_ArrayOfArrayHashComputationRowWiseBenchmark(benchmark::State& iState)
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

    void CacheMatrixTraversal_FlatArrayColumnWiseBenchmark(benchmark::State& iState)
    {
        std::size_t dimension = iState.range(0);
        auto flatArray = std::vector<std::int8_t>(dimension * dimension);
        std::iota(std::begin(flatArray), std::end(flatArray), 0);

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            for (std::size_t column = 0; column < dimension; ++column)
            {
                for (std::size_t row = 0; row < dimension; ++row)
                {
                    benchmark::DoNotOptimize(n += flatArray[row * dimension + column]);
                }
            }
        }

        benchmark::DoNotOptimize(n);
    }

    void CacheMatrixTraversal_FlatArrayRowWiseBenchmark(benchmark::State& iState)
    {
        std::size_t dimension = iState.range(0);
        auto flatArray = std::vector<std::int8_t>(dimension * dimension);
        std::iota(std::begin(flatArray), std::end(flatArray), 0);

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            for (std::size_t row = 0; row < dimension; ++row)
            {
                for (std::size_t column = 0; column < dimension; ++column)
                {
                    benchmark::DoNotOptimize(n += flatArray[row * dimension + column]);
                }
            }
        }

        benchmark::DoNotOptimize(n);
    }
}

BENCHMARK(CacheMatrixTraversal_ArrayOfArrayColumnWiseBenchmark)->RangeMultiplier(2)->Range(64, 8_KB);                // NOLINT
BENCHMARK(CacheMatrixTraversal_ArrayOfArrayRowWiseBenchmark)->RangeMultiplier(2)->Range(64, 8_KB);                   // NOLINT
BENCHMARK(CacheMatrixTraversal_ArrayOfArrayHashComputationColumnWiseBenchmark)->RangeMultiplier(2)->Range(64, 8_KB); // NOLINT
BENCHMARK(CacheMatrixTraversal_ArrayOfArrayHashComputationRowWiseBenchmark)->RangeMultiplier(2)->Range(64, 8_KB);    // NOLINT
BENCHMARK(CacheMatrixTraversal_FlatArrayColumnWiseBenchmark)->RangeMultiplier(2)->Range(64, 8_KB);                   // NOLINT
BENCHMARK(CacheMatrixTraversal_FlatArrayRowWiseBenchmark)->RangeMultiplier(2)->Range(64, 8_KB);                      // NOLINT
