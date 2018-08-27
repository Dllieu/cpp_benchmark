#include <numeric>
#include <benchmark/benchmark.h>
#include <set>
#include <unordered_set>
#include <utils/cache_information.h>

namespace
{
    void CacheMatrixTraversal_ColumnWiseBenchmark(benchmark::State& iState)
    {
        std::size_t dimension = iState.range(0);
        auto matrix = std::vector<std::int8_t>(dimension * dimension);
        std::generate(std::begin(matrix), std::end(matrix), [n = 0] () mutable { return n++; });

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            for (std::size_t row = 0; row < dimension; ++row)
            {
                for (std::size_t column = 0; column < dimension; ++column)
                {
                    benchmark::DoNotOptimize(n += matrix[row + column * dimension]);
                }
            }
        }

        benchmark::DoNotOptimize(n);
    }

    void CacheMatrixTraversal_RowWiseBenchmark(benchmark::State& iState)
    {
        std::size_t dimension = iState.range(0);
        auto matrix = std::vector<std::int8_t>(dimension * dimension);
        std::generate(std::begin(matrix), std::end(matrix), [n = 0] () mutable { return n++; });

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            for (std::size_t row = 0; row < dimension; ++row)
            {
                for (std::size_t column = 0; column < dimension; ++column)
                {
                    benchmark::DoNotOptimize(n += matrix[row * dimension + column]);
                }
            }
        }

        benchmark::DoNotOptimize(n);
    }
}

BENCHMARK(CacheMatrixTraversal_ColumnWiseBenchmark)->RangeMultiplier(2)->Range(64, 8_KB); // NOLINT
BENCHMARK(CacheMatrixTraversal_RowWiseBenchmark)->RangeMultiplier(2)->Range(64, 8_KB); // NOLINT
