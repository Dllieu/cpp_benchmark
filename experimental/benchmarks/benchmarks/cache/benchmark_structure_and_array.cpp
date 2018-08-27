#include <algorithm>
#include <benchmark/benchmark.h>
#include <limits>
#include <random>
#include <utils/cache_information.h>

namespace
{
    struct NakedStructure
    {
        NakedStructure()
            : m_X(0), m_Y(0), m_Z(0)
        {
        }

        std::size_t m_X, m_Y, m_Z;
    };

    // 64 / ( 6 / 3 ) / sizeof( std::uint8_t ) = 32 useful values per fetch average (6 / 3 as we only need x, y, z)
    void CacheStructureAndArray_ArrayOfStructure(benchmark::State& iState)
    {
        std::size_t size = iState.range(0);
        std::vector<NakedStructure> arrayOfStructure(size);

        for ([[maybe_unused]] auto handler : iState)
        {
            std::size_t n = 0;

            for (std::size_t i = 0; i < size; ++i)
            {
                benchmark::DoNotOptimize(n += arrayOfStructure[i].m_X + arrayOfStructure[i].m_Y + arrayOfStructure[i].m_Z);
            }

            benchmark::DoNotOptimize(n);
        }
    }

    struct NakedStructureWithUnusedMembers
    {
        NakedStructureWithUnusedMembers()
            : m_X(0), m_Y(0), m_Z(0), m_DX(0), m_DY(0), m_DZ(0)
        {
        }

        std::size_t m_X, m_Y, m_Z;
        [[maybe_unused]] std::size_t m_DX, m_DY, m_DZ;
    };

    // 64 / ( 6 / 3 ) / sizeof( std::uint8_t ) = 32 useful values per fetch average (6 / 3 as we only need x, y, z)
    void CacheStructureAndArray_ArrayOfStructureWithUnusedMembers(benchmark::State& iState)
    {
        std::size_t size = iState.range(0);
        std::vector<NakedStructureWithUnusedMembers> arrayOfStructure(size);

        for ([[maybe_unused]] auto handler : iState)
        {
            std::size_t n = 0;

            for (std::size_t i = 0; i < size; ++i)
            {
                benchmark::DoNotOptimize(n += arrayOfStructure[i].m_X + arrayOfStructure[i].m_Y + arrayOfStructure[i].m_Z);
            }

            benchmark::DoNotOptimize(n);
        }
    }

    struct StructureOfArrays
    {
        explicit StructureOfArrays(std::size_t n)
            : m_X(n), m_Y(n), m_Z(n)
        {
        }

        std::vector<std::size_t> m_X, m_Y, m_Z;
    };

    // 64 useful values per fetch
    void CacheStructureAndArray_StructureOfArrays(benchmark::State& iState)
    {
        std::size_t size = iState.range(0);
        StructureOfArrays structureOfArrays(size);

        for ([[maybe_unused]] auto handler : iState)
        {
            std::size_t n = 0;

            for (std::size_t i = 0; i < size; ++i)
            {
                benchmark::DoNotOptimize(n += structureOfArrays.m_X[i] + structureOfArrays.m_Y[i] + structureOfArrays.m_Z[i]);
            }

            benchmark::DoNotOptimize(n);
        }
    }

    struct StructureOfArraysWithUnusedMembers
    {
        explicit StructureOfArraysWithUnusedMembers(std::size_t n)
            : m_X(n), m_Y(n), m_Z(n), m_DX(n), m_DY(n), m_DZ(n)
        {
        }

        std::vector<std::size_t> m_X, m_Y, m_Z;
        [[maybe_unused]] std::vector<std::size_t> m_DX, m_DY, m_DZ;
    };

    // 64 useful values per fetch
    void CacheStructureAndArray_StructureOfArraysWithUnusedMembers(benchmark::State& iState)
    {
        std::size_t size = iState.range(0);
        StructureOfArraysWithUnusedMembers structureOfArrays(size);

        for ([[maybe_unused]] auto handler : iState)
        {
            std::size_t n = 0;

            for (std::size_t i = 0; i < size; ++i)
            {
                benchmark::DoNotOptimize(n += structureOfArrays.m_X[i] + structureOfArrays.m_Y[i] + structureOfArrays.m_Z[i]);
            }

            benchmark::DoNotOptimize(n);
        }
    }
}

BENCHMARK(CacheStructureAndArray_ArrayOfStructure)->Range(2_KB, 8_MB); // NOLINT
BENCHMARK(CacheStructureAndArray_ArrayOfStructureWithUnusedMembers)->Range(2_KB, 8_MB); // NOLINT
BENCHMARK(CacheStructureAndArray_StructureOfArrays)->Range(2_KB, 8_MB); // NOLINT
BENCHMARK(CacheStructureAndArray_StructureOfArraysWithUnusedMembers)->Range(2_KB, 8_MB); // NOLINT
