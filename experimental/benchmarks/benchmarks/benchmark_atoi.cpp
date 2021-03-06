#include <algorithm>
#include <array>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <random>
#include <utils/macros.h>

namespace
{
    template <typename T>
    constexpr T PowerOf10(size_t iN)
    {
        if (iN > std::numeric_limits<T>::digits10)
        {
            return 0;
        }

        T result = 1;

        while (iN--)
        {
            result *= 10;
        }

        return result;
    }

    template <std::size_t STRING_SIZE, typename T>
    struct PowerOf10Generator
    {
        constexpr PowerOf10Generator()
            : m_Array{}
        {
            for (std::size_t i = 0; i < STRING_SIZE; ++i)
            {
                m_Array[STRING_SIZE - 1 - i] = PowerOf10<T>(i);
            }
        }

        // can't replace by std::array as operator[] isn't constexpr
        T m_Array[STRING_SIZE];
    };

    template <size_t STRING_SIZE, typename T = std::size_t>
    force_inline T AtoiNaive(const char* s)
    {
        T result = 0;
        for (size_t i = 0; i < STRING_SIZE; ++i)
        {
            result = result * 10 + (s[i] - '0'); // computation is dependant of result : no vectorization possible
        }
        return result;
    }

    template <size_t STRING_SIZE, typename T = std::size_t>
    force_inline T AtoiVectorization(const char* s)
    {
        static constexpr const auto powerOf10 = PowerOf10Generator<STRING_SIZE, T>();

        T result = 0;
        for (size_t i = 0; i < STRING_SIZE; ++i)
        {
            result += powerOf10.m_Array[i] * (s[i] - '0'); // no dependencies, vectorization friendly
        }

        return result;
    }

    template <size_t STRING_SIZE, typename T = std::size_t>
    force_inline T AtoiVectorizationExplicitUnroll(const char* s)
    {
        static constexpr const auto powerOf10 = PowerOf10Generator<STRING_SIZE, T>();

        T result = 0;
        size_t i = 0;
        for (; i + 4 < STRING_SIZE; i += 4)
        {
            const T v0 = powerOf10.m_Array[i] * (s[i] - '0');
            const T v1 = powerOf10.m_Array[i + 1] * (s[i + 1] - '0');
            const T v2 = powerOf10.m_Array[i + 2] * (s[i + 2] - '0');
            const T v3 = powerOf10.m_Array[i + 3] * (s[i + 3] - '0');

            result += v0 + v1 + v2 + v3;
        }

        for (; i < STRING_SIZE; ++i)
        {
            result += powerOf10.m_Array[i] * (s[i] - '0');
        }

        return result;
    }

    std::string GetNumberAsString(std::size_t iStringSize)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<std::uint64_t> rnd(0, std::numeric_limits<std::uint64_t>::max());

        std::string unpaddedNumberString = std::to_string(rnd(gen));

        if (unpaddedNumberString.size() >= iStringSize)
        {
            return std::string(unpaddedNumberString.begin(), unpaddedNumberString.begin() + iStringSize);
        }
        else
        {
            std::string tmp(iStringSize, '0');
            std::copy(unpaddedNumberString.begin(), unpaddedNumberString.end(), tmp.end() - unpaddedNumberString.size());

            return tmp;
        }
    }

    template <typename F>
    void RunBenchmark(benchmark::State& iState, std::size_t iStringLength, F&& iFunctor)
    {
        std::string numberAsString = GetNumberAsString(iStringLength);

        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            benchmark::DoNotOptimize(n += iFunctor(numberAsString.c_str()));
        };
    }

    template <std::size_t N>
    void Atoi_Atoll(benchmark::State& iState)
    {
        RunBenchmark(iState, N, [](const char* iString) { return std::atoll(iString); }); // NOLINT
    }

    template <std::size_t N>
    void Atoi_StaticSizeNaive(benchmark::State& iState)
    {
        RunBenchmark(iState, N, [](const char* iString) { return AtoiNaive<N>(iString); });
    }

    template <std::size_t N>
    void Atoi_StaticSizeVectorization(benchmark::State& iState)
    {
        RunBenchmark(iState, N, [](const char* iString) { return AtoiVectorization<N>(iString); });
    }

    template <std::size_t N>
    void Atoi_StaticSizeVectorizationExplicitUnroll(benchmark::State& iState)
    {
        RunBenchmark(iState, N, [](const char* iString) { return AtoiVectorizationExplicitUnroll<N>(iString); });
    }
}

#define DECLARE_ATOI_BENCHMARK(N)                        \
    BENCHMARK_TEMPLATE(Atoi_Atoll, N);                   \
    BENCHMARK_TEMPLATE(Atoi_StaticSizeNaive, N);         \
    BENCHMARK_TEMPLATE(Atoi_StaticSizeVectorization, N); \
    BENCHMARK_TEMPLATE(Atoi_StaticSizeVectorizationExplicitUnroll, N);

CALL_MACRO_FOR_EACH(DECLARE_ATOI_BENCHMARK, 4, 5, 6, 8, 10, 12, 16, 18, 32, 64) // NOLINT

#undef DECLARE_ATOI_BENCHMARK
