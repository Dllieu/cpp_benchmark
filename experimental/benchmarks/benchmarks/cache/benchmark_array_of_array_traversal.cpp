#include <benchmark/benchmark.h>
#include <cmath>
#include <numeric>
#include <utils/cache_information.h>
#include <vector>

namespace
{
    template <typename F>
    void RunBenchmark(benchmark::State& iState, F&& iFunctor)
    {
        std::size_t bytesRequested = iState.range(0);
        std::size_t dimension = std::sqrt(bytesRequested);

        std::vector<std::vector<std::int8_t>> arrayOfArray(dimension, std::vector<std::int8_t>(dimension));
        for (std::size_t i = 0; i < arrayOfArray.size(); ++i)
        {
            std::iota(std::begin(arrayOfArray[i]), std::end(arrayOfArray[i]), i * dimension);
        }

        for ([[maybe_unused]] auto handler : iState)
        {
            iFunctor(arrayOfArray, dimension);
        }
    }

    void CacheArrayOfArrayTraversal_ColumnWise(benchmark::State& iState)
    {
        RunBenchmark(iState, [](std::vector<std::vector<std::int8_t>>& iArrayOfArray, std::size_t iDimension) {
            for (std::size_t column = 0; column < iDimension; ++column)
            {
                for (std::size_t row = 0; row < iDimension; ++row)
                {
                    iArrayOfArray[row][column] += row;
                }
            }
        });
    }

    void CacheArrayOfArrayTraversal_RowWise(benchmark::State& iState)
    {
        RunBenchmark(iState, [](std::vector<std::vector<std::int8_t>>& iArrayOfArray, std::size_t iDimension) {
            for (std::size_t row = 0; row < iDimension; ++row)
            {
                for (std::size_t column = 0; column < iDimension; ++column)
                {
                    iArrayOfArray[row][column] += row;
                }
            }
        });
    }

    void CacheArrayOfArrayTraversal_ColumnWiseHashComputation(benchmark::State& iState)
    {
        RunBenchmark(iState, [](std::vector<std::vector<std::int8_t>>& iArrayOfArray, std::size_t iDimension) {
            for (std::size_t column = 0; column < iDimension; ++column)
            {
                for (std::size_t row = 0; row < iDimension; ++row)
                {
                    iArrayOfArray[row][column] += std::sqrt(std::hash<std::int8_t>()(row * iDimension + column));
                }
            }
        });
    }

    void CacheArrayOfArrayTraversal_RowWiseHashComputation(benchmark::State& iState)
    {
        RunBenchmark(iState, [](std::vector<std::vector<std::int8_t>>& iArrayOfArray, std::size_t iDimension) {
            for (std::size_t row = 0; row < iDimension; ++row)
            {
                for (std::size_t column = 0; column < iDimension; ++column)
                {
                    iArrayOfArray[row][column] += std::sqrt(std::hash<std::int8_t>()(row * iDimension + column));
                }
            }
        });
    }

    void BenchmarkArguments(benchmark::internal::Benchmark* iBenchmark)
    {
        for (std::size_t i = 2_MB; i <= 10_MB; i += 100_KB)
        {
            iBenchmark->Arg(i);
        }
    }
}

BENCHMARK(CacheArrayOfArrayTraversal_ColumnWise)->Apply(BenchmarkArguments);                // NOLINT
BENCHMARK(CacheArrayOfArrayTraversal_ColumnWiseHashComputation)->Apply(BenchmarkArguments); // NOLINT
BENCHMARK(CacheArrayOfArrayTraversal_RowWise)->Apply(BenchmarkArguments);                   // NOLINT
BENCHMARK(CacheArrayOfArrayTraversal_RowWiseHashComputation)->Apply(BenchmarkArguments);    // NOLINT
