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
    void    bench_syscall_gettid( benchmark::State& state )
    {
        while ( state.KeepRunning() )
            // returns the caller's thread ID
            syscall( SYS_gettid );
    }
}

BENCHMARK( bench_syscall_gettid );