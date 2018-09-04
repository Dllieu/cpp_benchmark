#include <algorithm>
#include <benchmark/benchmark.h>
#include <limits>
#include <random>
#include <utils/cache_information.h>

namespace
{
    template <typename F>
    void RunBenchmark(benchmark::State& iState, F&& iFunctor)
    {
        std::size_t size = iState.range(0);

        for ([[maybe_unused]] auto handler : iState)
        {
            std::size_t n = 0;

            for (std::size_t i = 0; i < size; ++i)
            {
                benchmark::DoNotOptimize(n += iFunctor(i));
            }
        }
    }

    void CacheStructureAndArray_ArrayOfStructure(benchmark::State& iState)
    {
        struct Structure
        {
            Structure()
                : m_X(0)
                , m_Y(0)
                , m_Z(0)
            {
            }

            std::int64_t m_X;
            std::int64_t m_Y;
            std::int64_t m_Z;
        };

        std::vector<Structure> arrayOfStructure(iState.range(0));
        RunBenchmark(iState, [&](std::size_t i) { return arrayOfStructure[i].m_X + arrayOfStructure[i].m_Y + arrayOfStructure[i].m_Z; });
    }

    // Half ot the fetched data is wasted due to the unused members
    void CacheStructureAndArray_ArrayOfStructureWithUnusedMembers(benchmark::State& iState)
    {
        struct StructureWithUnusedMembers
        {
            StructureWithUnusedMembers()
                : m_X(0)
                , m_Y(0)
                , m_Z(0)
                , m_DX(0)
                , m_DY(0)
                , m_DZ(0)
            {
            }

            std::int64_t m_X;
            std::int64_t m_Y;
            std::int64_t m_Z;
            std::int64_t m_DX;
            std::int64_t m_DY;
            std::int64_t m_DZ;
        };

        std::vector<StructureWithUnusedMembers> arrayOfStructure(iState.range(0));
        RunBenchmark(iState, [&](std::size_t i) { return arrayOfStructure[i].m_X + arrayOfStructure[i].m_Y + arrayOfStructure[i].m_Z; });
    }

    void CacheStructureAndArray_StructureOfArrays(benchmark::State& iState)
    {
        struct StructureOfArrays
        {
            explicit StructureOfArrays(std::size_t n)
                : m_X(n)
                , m_Y(n)
                , m_Z(n)
            {
            }

            std::vector<std::int64_t> m_X;
            std::vector<std::int64_t> m_Y;
            std::vector<std::int64_t> m_Z;
        } structureOfArrays(iState.range(0));
        RunBenchmark(iState, [&](std::size_t i) { return structureOfArrays.m_X[i] + structureOfArrays.m_Y[i] + structureOfArrays.m_Z[i]; });
    }

    // Unused member do not affect the fetching
    void CacheStructureAndArray_StructureOfArraysWithUnusedMembers(benchmark::State& iState)
    {
        struct StructureOfArraysWithUnusedMembers
        {
            explicit StructureOfArraysWithUnusedMembers(std::size_t n)
                : m_X(n)
                , m_Y(n)
                , m_Z(n)
                , m_DX(n)
                , m_DY(n)
                , m_DZ(n)
            {
            }

            std::vector<std::int64_t> m_X;
            std::vector<std::int64_t> m_Y;
            std::vector<std::int64_t> m_Z;
            std::vector<std::int64_t> m_DX;
            std::vector<std::int64_t> m_DY;
            std::vector<std::int64_t> m_DZ;
        } structureOfArrays(iState.range(0));
        RunBenchmark(iState, [&](std::size_t i) { return structureOfArrays.m_X[i] + structureOfArrays.m_Y[i] + structureOfArrays.m_Z[i]; });
    }

    void BenchmarkArguments(benchmark::internal::Benchmark* iBenchmark)
    {
        for (std::size_t i = 2_KB; i <= 40_KB; i += 2_KB)
        {
            iBenchmark->Arg(i);
        }
    }
}

BENCHMARK(CacheStructureAndArray_ArrayOfStructure)->Apply(BenchmarkArguments);                   // NOLINT
BENCHMARK(CacheStructureAndArray_ArrayOfStructureWithUnusedMembers)->Apply(BenchmarkArguments);  // NOLINT
BENCHMARK(CacheStructureAndArray_StructureOfArrays)->Apply(BenchmarkArguments);                  // NOLINT
BENCHMARK(CacheStructureAndArray_StructureOfArraysWithUnusedMembers)->Apply(BenchmarkArguments); // NOLINT
