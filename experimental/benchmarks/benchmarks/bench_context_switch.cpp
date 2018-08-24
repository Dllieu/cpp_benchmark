#include <benchmark/benchmark.h>

#include <thread>
#include <atomic>
#include <iostream>
#include <cstddef>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/futex.h>
#include <cstring>
#include <memory>
#include <utils/cache_information.h>

namespace
{
    constexpr const auto SHARED_MEMORY_DEFAULT_VALUE = -2;
    constexpr const auto MAX_THREADS = 2;

    void    power2_argument( benchmark::internal::Benchmark* b )
    {
        for ( size_t i = 1; i <= 2_KB; i <<= 1 )
        {
            b->Arg( i );
        }
    }
}

// TODO: run with and without affinity
namespace
{
    // i.e. unsafe but that's ok
    void    futex_thread_one( benchmark::State& state, std::atomic< int >& sharedMemoryId )
    {
        sharedMemoryId = shmget( IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666 );

        volatile auto futex = static_cast<int*>(shmat(sharedMemoryId, nullptr, 0));
        while ( state.KeepRunning() )
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

    void    futex_thread_two( benchmark::State& state, std::atomic< int >& sharedMemoryId )
    {
        while ( sharedMemoryId == SHARED_MEMORY_DEFAULT_VALUE )
        {
            std::this_thread::yield();
        }

        volatile auto futex = static_cast<int*>(shmat(sharedMemoryId, nullptr, 0));
        while ( state.KeepRunning() )
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

        while ( sharedMemoryId != SHARED_MEMORY_DEFAULT_VALUE )
        {
            std::this_thread::yield();
        }
    }

    // Does nothing but context switching. In practice context switching is expensive because it screws up the CPU caches (L1, L2, L3 and the TLB)
    void    bench_context_switch_raw( benchmark::State& state )
    {
        static std::atomic< int > sharedMemoryId( SHARED_MEMORY_DEFAULT_VALUE );
        if ( state.thread_index == 0 )
        {
            futex_thread_one( state, sharedMemoryId );
        }
        else
        {
            futex_thread_two( state, sharedMemoryId );
        }
    }
}

BENCHMARK( bench_context_switch_raw )->Threads( MAX_THREADS ); // NOLINT

namespace
{
    void    bench_context_switch_memset( benchmark::State& state )
    {
        int ws_pages = state.range(0);
        std::unique_ptr<std::byte[]> buf(new std::byte[ws_pages * experimental::PAGE_SIZE]);

        // i.e. number of pages
        int i = 0;
        while ( state.KeepRunning() )
        {
            memset( buf.get(), i++, ws_pages * experimental::PAGE_SIZE );
        }

        state.SetItemsProcessed( state.iterations() );
        state.SetLabel( experimental::to_string( experimental::byteToAppropriateCacheSize< char >( ws_pages * experimental::PAGE_SIZE ) ) );
    }
}

BENCHMARK( bench_context_switch_memset )->Apply( power2_argument ); // NOLINT

namespace
{
    void    set_bytes_processed( benchmark::State& state )
    {
        state.SetBytesProcessed( state.iterations() * ( ( state.range(0) + 1 ) * experimental::PAGE_SIZE ) + state.bytes_processed() );
    }

    void    futex_thread_one_cache( benchmark::State& state, std::atomic< int >& sharedMemoryId )
    {
        // 1 experimental::PAGE_SIZE dedicated to futex / the others just for invalidating the cache
        sharedMemoryId = shmget( IPC_PRIVATE, ( state.range(0) + 1 ) * experimental::PAGE_SIZE, IPC_CREAT | 0666 );

        volatile auto futex = static_cast<int*>(shmat(sharedMemoryId, nullptr, 0));
        auto workingSet = reinterpret_cast<std::byte*>(futex) + experimental::PAGE_SIZE;
        while ( state.KeepRunning() )
        {
            memset( workingSet, state.iterations(), state.range(0) * experimental::PAGE_SIZE );

            *futex = 0xA;
            while ( 0 == syscall(SYS_futex, futex, FUTEX_WAKE, 1, nullptr, nullptr, 42 ) )
            {
                std::this_thread::yield();
            }

            std::this_thread::yield();
            while ( 0 != syscall( SYS_futex, futex, FUTEX_WAIT, 0xB, nullptr, nullptr, 42 ) )
            {
                std::this_thread::yield();
            }
        }

        set_bytes_processed( state );
        sharedMemoryId = SHARED_MEMORY_DEFAULT_VALUE;
        wait( futex );
    }

    void    futex_thread_two_cache( benchmark::State& state, std::atomic< int >& sharedMemoryId )
    {
        while ( sharedMemoryId == SHARED_MEMORY_DEFAULT_VALUE )
        {
            std::this_thread::yield();
        }

        volatile auto futex = static_cast<int*>(shmat(sharedMemoryId, nullptr, 0));
        auto workingSet = reinterpret_cast<std::byte*>(futex) + experimental::PAGE_SIZE;
        while ( state.KeepRunning() )
        {
            std::this_thread::yield();
            while ( 0 != syscall( SYS_futex, futex, FUTEX_WAIT, 0xA, nullptr, nullptr, 42 ) )
            {
                std::this_thread::yield();
            }

            memset( workingSet, state.iterations(), state.range(0) * experimental::PAGE_SIZE );

            *futex = 0xB;
            while ( 0 == syscall( SYS_futex, futex, FUTEX_WAKE, 1, nullptr, nullptr, 42 ) )
            {
                std::this_thread::yield();
            }
        }

        set_bytes_processed( state );
        while ( sharedMemoryId != SHARED_MEMORY_DEFAULT_VALUE )
        {
            std::this_thread::yield();
        }
    }

    void    bench_context_switch_cache( benchmark::State& state )
    {
        static std::atomic< int > sharedMemoryId( SHARED_MEMORY_DEFAULT_VALUE );

        if ( state.thread_index == 0 )
        {
            futex_thread_one_cache( state, sharedMemoryId );
        }
        else
        {
            futex_thread_two_cache( state, sharedMemoryId );
        }
    }
}

BENCHMARK( bench_context_switch_cache )->Threads( MAX_THREADS )->Apply( power2_argument ); // NOLINT
