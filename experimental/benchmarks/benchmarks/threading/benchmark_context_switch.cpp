#include <atomic>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <linux/futex.h>
#include <memory>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <utils/cache_information.h>

namespace
{
    constexpr const auto SHARED_MEMORY_DEFAULT_VALUE = -2;
}

namespace
{
    void ContextSwitch_SingleThreadMemsetBenchmark(benchmark::State& state)
    {
        int ws_pages = state.range(0);
        std::unique_ptr<std::byte[]> buf(new std::byte[ws_pages * experimental::PAGE_SIZE]);

        int numberOfPages = 0;
        for ([[maybe_unused]] auto handler : state)
        {
            std::memset(buf.get(), numberOfPages++, ws_pages * experimental::PAGE_SIZE);
        }

        state.SetItemsProcessed(state.iterations());
        state.SetLabel(experimental::to_string(experimental::byteToAppropriateCacheSize<char>(ws_pages * experimental::PAGE_SIZE)));
    }
}

BENCHMARK(ContextSwitch_SingleThreadMemsetBenchmark)->RangeMultiplier(2)->Range(1, 2_KB); // NOLINT

namespace
{
    // i.e. unsafe but that's ok
    void FutexThreadOne(benchmark::State& state, std::atomic<int>& sharedMemoryId)
    {
        sharedMemoryId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);

        volatile auto futex = static_cast<int*>(shmat(sharedMemoryId, nullptr, 0));
        for ([[maybe_unused]] auto handler : state)
        {
            *futex = 0xA;
            while (0 == syscall(SYS_futex, futex, FUTEX_WAKE, 1, nullptr, nullptr, 42))
            {
                std::this_thread::yield();
            }

            std::this_thread::yield();
            while (0 != syscall(SYS_futex, futex, FUTEX_WAIT, 0xB, nullptr, nullptr, 42))
            {
                std::this_thread::yield();
            }
        }

        sharedMemoryId = SHARED_MEMORY_DEFAULT_VALUE;
        wait(futex);
    }

    void FutexThreadTwo(benchmark::State& state, std::atomic<int>& sharedMemoryId)
    {
        while (sharedMemoryId == SHARED_MEMORY_DEFAULT_VALUE)
        {
            std::this_thread::yield();
        }

        volatile auto futex = static_cast<int*>(shmat(sharedMemoryId, nullptr, 0));
        for ([[maybe_unused]] auto handler : state)
        {
            std::this_thread::yield();
            while (0 != syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, nullptr, nullptr, 42))
            {
                std::this_thread::yield();
            }

            *futex = 0xB;
            while (0 == syscall(SYS_futex, futex, FUTEX_WAKE, 1, nullptr, nullptr, 42))
            {
                std::this_thread::yield();
            }
        }

        while (sharedMemoryId != SHARED_MEMORY_DEFAULT_VALUE)
        {
            std::this_thread::yield();
        }
    }

    void ContextSwitch_TwoThreadAccessSharedMemoryBenchmark(benchmark::State& state)
    {
        static std::atomic<int> sharedMemoryId(SHARED_MEMORY_DEFAULT_VALUE);
        if (state.thread_index == 0)
        {
            FutexThreadOne(state, sharedMemoryId);
        }
        else
        {
            FutexThreadTwo(state, sharedMemoryId);
        }
    }
}

BENCHMARK(ContextSwitch_TwoThreadAccessSharedMemoryBenchmark)->Threads(2); // NOLINT

namespace
{
    void SetBytesProcessed(benchmark::State& state)
    {
        state.SetBytesProcessed(state.iterations() * ((state.range(0) + 1) * experimental::PAGE_SIZE) + state.bytes_processed());
    }

    void FutexThreadOneMemset(benchmark::State& state, std::atomic<int>& sharedMemoryId)
    {
        // 1 experimental::PAGE_SIZE dedicated to futex / the others just for invalidating the cache
        sharedMemoryId = shmget(IPC_PRIVATE, (state.range(0) + 1) * experimental::PAGE_SIZE, IPC_CREAT | 0666);

        volatile auto futex = static_cast<int*>(shmat(sharedMemoryId, nullptr, 0));
        auto workingSet = reinterpret_cast<std::byte*>(futex) + experimental::PAGE_SIZE;
        for ([[maybe_unused]] auto handler : state)
        {
            std::memset(workingSet, state.iterations(), state.range(0) * experimental::PAGE_SIZE);

            *futex = 0xA;
            while (0 == syscall(SYS_futex, futex, FUTEX_WAKE, 1, nullptr, nullptr, 42))
            {
                std::this_thread::yield();
            }

            std::this_thread::yield();
            while (0 != syscall(SYS_futex, futex, FUTEX_WAIT, 0xB, nullptr, nullptr, 42))
            {
                std::this_thread::yield();
            }
        }

        SetBytesProcessed(state);
        sharedMemoryId = SHARED_MEMORY_DEFAULT_VALUE;
        wait(futex);
    }

    void FutexThreadTwoMemset(benchmark::State& state, std::atomic<int>& sharedMemoryId)
    {
        while (sharedMemoryId == SHARED_MEMORY_DEFAULT_VALUE)
        {
            std::this_thread::yield();
        }

        volatile auto futex = static_cast<int*>(shmat(sharedMemoryId, nullptr, 0));
        auto workingSet = reinterpret_cast<std::byte*>(futex) + experimental::PAGE_SIZE;
        for ([[maybe_unused]] auto handler : state)
        {
            std::this_thread::yield();
            while (0 != syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, nullptr, nullptr, 42))
            {
                std::this_thread::yield();
            }

            std::memset(workingSet, state.iterations(), state.range(0) * experimental::PAGE_SIZE);

            *futex = 0xB;
            while (0 == syscall(SYS_futex, futex, FUTEX_WAKE, 1, nullptr, nullptr, 42))
            {
                std::this_thread::yield();
            }
        }

        SetBytesProcessed(state);
        while (sharedMemoryId != SHARED_MEMORY_DEFAULT_VALUE)
        {
            std::this_thread::yield();
        }
    }

    void ContextSwitch_TwoThreadAccessAndMemsetSharedMemoryBenchmark(benchmark::State& state)
    {
        static std::atomic<int> sharedMemoryId(SHARED_MEMORY_DEFAULT_VALUE);

        if (state.thread_index == 0)
        {
            FutexThreadOneMemset(state, sharedMemoryId);
        }
        else
        {
            FutexThreadTwoMemset(state, sharedMemoryId);
        }
    }
}

BENCHMARK(ContextSwitch_TwoThreadAccessAndMemsetSharedMemoryBenchmark)->Threads(2)->RangeMultiplier(2)->Range(1, 2_KB); // NOLINT
