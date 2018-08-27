#include <benchmark/benchmark.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace
{
    /*
     * @brief system_call
     *        function that invokes the system call whose assembly language interface has the specified number with the specified arguments
     *        useful when invoking a system call that has no wrapper function
     *        syscall() saves CPU registers before making the system call, restores the registers upon return from the system call, and stores any error code returned by the system call in errno if an error occurs
     *
     *        system calls don't actually cause a full context switch anymore nowadays, the kernel can get away with a "mode switch" (go from user mode to kernel mode, then back to user mode)
     */
    void SystemCall_GettidBenchmark(benchmark::State& state)
    {
        for ([[maybe_unused]] auto handler : state)
        {
            // returns the caller's thread ID
            syscall(SYS_gettid);
        }
    }
}

BENCHMARK(SystemCall_GettidBenchmark); // NOLINT
