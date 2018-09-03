#include <atomic>
#include <benchmark/benchmark.h>
#include <thread>

namespace
{
    void FalseSharing_RunBenchmark(benchmark::State& iState, std::atomic<std::int64_t>& iAtomicA, std::atomic<std::int64_t>& iAtomicB, std::atomic<std::int64_t>& iAtomicC, std::atomic<std::int64_t>& iAtomicD)
    {
        std::size_t numberIterations = iState.range(0);

        auto f = [numberIterations](std::atomic<std::int64_t>& iAtomic) {
            for (std::size_t i = 0; i < numberIterations; ++i)
            {
                ++iAtomic;
            }
        };

        for ([[maybe_unused]] auto handler : iState)
        {
            std::thread t1([&]() { f(iAtomicA); });
            std::thread t2([&]() { f(iAtomicB); });
            std::thread t3([&]() { f(iAtomicC); });
            std::thread t4([&]() { f(iAtomicD); });

            t1.join();
            t2.join();
            t3.join();
            t4.join();

            iAtomicA = 0;
            iAtomicB = 0;
            iAtomicC = 0;
            iAtomicD = 0;
        }
    }

    void FalseSharing_FalseSharing(benchmark::State& iState)
    {
        std::atomic<std::int64_t> a{0};
        std::atomic<std::int64_t> b{0};
        std::atomic<std::int64_t> c{0};
        std::atomic<std::int64_t> d{0};

        FalseSharing_RunBenchmark(iState, a, b, c, d);
    }

    void FalseSharing_Padding64(benchmark::State& iState)
    {
        alignas(64) std::atomic<std::int64_t> a{0};
        alignas(64) std::atomic<std::int64_t> b{0};
        alignas(64) std::atomic<std::int64_t> c{0};
        alignas(64) std::atomic<std::int64_t> d{0};

        FalseSharing_RunBenchmark(iState, a, b, c, d);
    }

    void FalseSharing_Arguments(benchmark::internal::Benchmark* iBenchmark)
    {
        iBenchmark->RangeMultiplier(10)->Range(1, 10'000);
    }
}

BENCHMARK(FalseSharing_FalseSharing)->Apply(FalseSharing_Arguments); // NOLINT
BENCHMARK(FalseSharing_Padding64)->Apply(FalseSharing_Arguments);    // NOLINT
