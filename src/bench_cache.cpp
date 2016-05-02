#include <benchmark/benchmark.h>

#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <random>
#include <list>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>

#include "utils/cache_information.h"
#include "utils/perf_event.h"

// Intel Core i5-4460 (4 cores) (give a general idea - most of the benchmark might give different result depending of the architecture (e.g. if running on a VM with appveyor))
//
// Translation Lookaside Buffer (TLB)
// CPU cache that memory management hardware uses to improve virtual address translation speed, it has a fixed number of slots that contain page table entries, which map virtual addresses to physical addresses.
// It is typically a content-addressable memory (CAM), in which the search key is the virtual address and the search result is a physical address.
// - 2MB pages mode
//   - L1  (32 entries), Miss Penalty = 16 cycles. Parallel miss: 20 cycles per access
// - 4KB page mode
//   - L1  (64 entries), 4-WAY
//                   * miss penalty : 7 cycles, parallel miss: 1 cycle per access
//   - L2 (512 entries), 4-WAY
//                   * miss penalty : 9 cycles, parallel miss: 21 cycle per access
//
//
// Instruction cache
// - L1  (32KB), 8-WAY, 64B / line
//
// Data Cache (8-WAY, 64B / line)
// - L1  (32KB):    1   ns /   4 cycles
//             * 1 per core (hyperthreading delivers two processing threads per physical core, shared by two HW thread)
//             * 4 cycles for simple access via pointer (p), 5 cycles for access with complex address calculation (p[n])
// - L2 (256KB):    3.1 ns /  12 cycles (per processor, shared by 2 HW threads)
// - L3   (6MB):    7.7 ns /  30 cycles (share among all the core (4 core -> 8 HW threads))
//             * 29.5 cycles for cores (1, 2)
//             * 30.5 cycles for cores (0, 3)
// - DRAM  (XX):   60   ns / 237 cycles
//
// cache size = cache line size * associativity * number of sets
//
// DRAM (dynamic ram) SRAM (static ram)
//
// Caches are small, assume 100MB program at runtime (code + data).
// - 8% fits in core-i79xx's L3 cache.
//   -> L3 cache shared by every running process (incl. OS).
// - 0.25% fits in each L2 cache.
// - 0.03% fits in each L1 cache.
//
// Check for more infos: http://www.7-cpu.com/

// Main memory to cache
// - Memory is transferred from the main memory into the caches in blocks which are smaller than the cache line size.Today 64 bits are transferred at once and the cache line size is (usually) 64 bytes (This means 8 or 16 transfers per cache line are needed)
// - The DRAM chips can transfer those 64-byte blocks in burst mode.This can fill the cache line without any further commands from the memory controller and the possibly associated delays. If the processor prefetches cache lines this is probably the best way to operate
// - The memory controller is free to request the words of the cache line in a different order.The processor can communicate which word the program is waiting on, the critical word, and the memory controller can request this word first.
//   Once the word arrives the program can continue while the rest of the cache line arrives and the cache is not yet in a consistent state.
//   This technique is called Critical Word First
//   Processors nowadays implement this technique but there are situations when that is not possible.If the processor prefetches data the critical word is not known.Should the processor request the cache line during the time
//   the prefetch operation is in flight it will have to wait until the critical word arrives without being able to influence the order.

// When does a cache line transfer have to happen from a processor to another? when one processor needs a cache line which is dirty in another processor's cache for reading or writing ("easily" implemented with MESI)
// MESI Protocol Transitions:
//   * Modified: The local processor has modified the cache line. This also implies it is the only copy in any cache.
//   * Exclusive: The cache line is not modified but known to not be loaded into any other processor’s cache.
//   * Shared: The cache line is not modified and might exist in another processor’s cache.
//   * Invalid: The cache line is invalid, i.e., unused
//
// About Modified transition:
//   - If a Modified cache line is read from or written to on the local processor, the instruction can use the current cache content and the state does not change.
//   - If a second processor wants to read from the cache line the first processor has to send the content of its cache to the second processor and then it can change the state to Shared.
//   - The data sent to the second processor is also received and processed by the memory controller which stores the content in memory.
//   - If this did not happen the cache line could not be marked as Shared.If the second processor wants to write to the cache line the first processor sends the cache line content and marks the cache line locally as invalid
//   - This is the infamous "Request For Ownership" (RFO) operation. Performing this operation in the last level cache, just like the I->M transition is comparatively expensive.
//   - For write-through caches we also have to add the time it takes to write the new cache line content to the next higher-level cache or the main memory, further increasing the cost

// About cache misses
//  A cache miss refers to a failed attempt to read or write a piece of data in the cache, which results in a main memory access with much longer latency.
//  There are three kinds of cache misses: instruction read miss, data read miss, and data write miss.
//  - Cache read misses from an instruction cache generally cause the largest delay, because the processor, or at least the thread of execution, has to wait (stall) until the instruction is fetched from main memory.
//  - Cache read misses from a data cache usually cause a smaller delay, because instructions not dependent on the cache read can be issued and continue execution until the data is returned from main memory,
//    and the dependent instructions can resume execution.
//  - Cache write misses to a data cache generally cause the shortest delay, because the write can be queued and there are few limitations on the execution of subsequent instructions; the processor can continue until the queue is full
//  - could possibly have cache miss on unlinked data (two static data that are put on the same cache line, N global on different translation unit could also be put in same cache line, same thing for different dynamic allocation, or stack for that matter)
//  - As data cache is 8-way associative, we can know in which block a cache line is depending of the address of the data (i.e. beginning_adress_cache_line(address) % 8)
//    theoretically we could only work with data that are contains in the same cache block which could result in horrible performance as we would only use 1/8 of the cache available

// About Instruction Cache friendly
// Any code that changes the flow of execution affects the Instruction Cache. This includes function calls and loops as well as dereferencing function pointers.
// - When a branch or jump instruction is executed, the processor has to spend extra time deciding if the code is already in the instruction cache or whether it needs to reload the instruction cache( from the destination of the branch ).
// - For example, some processors may have a large enough instruction cache to hold the execution code for small loops.Some processors don't have a large instruction cache and simple reload it. Reloading of the instruction cache takes time that could be spent executing instructions.
// What can help
// - Reduce "if" statements
// - Define small functions as inline or macros
//     -> There is an overhead associated with calling functions, such as storing the return location and reloading the instruction cache.
//     -> it's not as straightforward though, this could incurr code bloat, specially if the function inlined is unlikely called (in branch) and that the function is inlined in several place in the code (code duplication reduces effective cache size)
// - Unroll loops
// - Use table lookups, not "if" statements
// - Change data or data structures (For example, a program handling message packets could base its operations based on the packet IDs (think array of function pointers (cache miss?)). Functions would be optimized for packet processing)

// Classical big-O algorithmic complexity analysis proves insufficient to estimate program performance for modern computer architectures,
// current processors are equipped with several low-level components (hierarchical cache structures, pipelining, branch prediction)
// that greatly favor certain code and data layout patterns not taken into account by naive computation models.

// About Hyper-threading
// - Hyper-Threads are implemented by the CPU and are a special case since the individual threads cannot really run concurrently.They all share almost all the processing resources except for the register set
// - The real advantage is that the CPU can schedule another hyper-thread and take advantage of available resources such as arithmetic logic units (ALUs) when the currently running hyper-thread is delayed
// - In most cases this is a delay caused by memory accesses
// - If two threads are running on one hyper-threaded core the program is only more efficient than the single-threaded code if the combined runtime of both threads is lower than the runtime of the single-threaded code
// - Might only be achievable if single thread have a low cache hit rate (need to take into account the overhead for parallelizing the code)
// - Could be used as a mere thread to prefetch data into L2 or L1d for the "main" working thread (as they share the same cache) (imply proper affinity)

// Take advantage of
// - PGO (Profile-guided optimization) (i.e. -fprofile-generate in gcc), It works by placing extra code to count the number of times each codepath is taken. The profiling test must be representative to the production
//   When you compile a second time the compiler uses the knowledge gained about execution of your program that it could only guess at before (frequency statemants, branching, ...). There are a couple things PGO can work toward:
//     Deciding which functions should be inlined or not depending on how often they are called.
//     Deciding how to place hints about which branch of an "if" statement should be predicted on based on the percentage of calls going one way or the other.
//     Deciding how to optimize loops based on how many iterations get taken each time that loop is called.
//   It help the compiler to be less reliant on heuristics when making compilation decisions
//   Requires important representative use cases. Tends to be most helpful for large, non - loop - bound applications.
//     Hundreds to thousands of functions.
//     Most time spent in branches, calls / returns.
//   Designed for use after source code freeze., By default, source code changes invalidate instrumentation data. Resource - intensive during builds and instrumented runs (e.g. Instrumentation insertion / execution / analysis not cheap)
// - WPO (whole program optimization (O3 + unsafe unloop + omit frame ptr + specific target arch instructions + ...))
namespace
{
    template < typename T >
    T   create_container_random_values( size_t size )
    {
        std::uniform_int_distribution<> rnd( 0, static_cast< int >( size ) - 1 );
        std::mt19937 gen;

        T t( size );
        std::generate( std::begin( t ), std::end( t ), [ &rnd, &gen ] { return rnd( gen ); } );

        return t;
    }

    auto init_perf_event()
    {
        perf_event_attr pe;

        memset(&pe, 0, sizeof(perf_event_attr));
        pe.type = PERF_TYPE_HARDWARE;
        pe.size = sizeof(perf_event_attr);
        //pe.config = PERF_COUNT_HW_INSTRUCTIONS;
        pe.config = PERF_COUNT_HW_CACHE_MISSES;

        //pe.type = PERF_TYPE_HW_CACHE;
        //pe.config = PERF_COUNT_HW_CACHE_LL;

        pe.disabled = 1;
        pe.exclude_kernel = 1;
        pe.exclude_hv = 1;

        // pid == 0 and cpu == -1:  This measures the calling process/thread on any CPU.
        // pid == 0 and cpu >= 0:   This measures the calling process/thread only when running on the specified CPU.
        // pid > 0 and cpu == -1:   This measures the specified process/thread on any CPU.
        // pid > 0 and cpu >= 0:    This measures the specified process/thread only when running on the specified CPU.
        // pid == -1 and cpu >= 0:  This measures all processes/threads on the specified CPU. This requires CAP_SYS_ADMIN capability or a /proc/sys/kernel/perf_event_paranoid value of less than 1.
        // pid == -1 and cpu == -1: This setting is invalid and will return an error.
        return perf::event_open(pe, 0, -1, -1, 0);
    }

    void    custom_args( benchmark::internal::Benchmark* b )
    {
        using namespace cache;
        for ( size_t i = 32; i <= 1024_KB; i *= 2 )
            b->Arg( i );
    }
}

// |oooooooooooooooooooo|-----------------------|
// | current cache line | prefetched cache line |
// Two aspects to watch out for
// - Locality
// - Prefetching
// hardware speculatively prefetches cache lines, thus can generally prefetch for you as long as the access is consistent (i.e. i++ or i += 4 or even i--), will prefetch (if applicable) line n - 1 or n + 1 depending of the access pattern
// std::vector / std::array excels at both
// std::list sequentially allocated nodes provide some sort of non-guaranteed locality
// shuffled nodes is the worst scenario
namespace
{
    template < typename T >
    void    start_traversal( const T& container, benchmark::State& state )
    {
        auto fd = init_perf_event();
        ioctl( fd, PERF_EVENT_IOC_RESET, 0 );
        ioctl( fd, PERF_EVENT_IOC_ENABLE, 0 );

        while ( state.KeepRunning() )
            benchmark::DoNotOptimize( std::accumulate( std::begin( container ), std::end( container ), 0 ) );

        ioctl( fd, PERF_EVENT_IOC_DISABLE, 0 );

        // Won't be accurate for node based container
        state.SetLabel( cache::to_string( cache::byteToAppropriateCacheSize< int >( state.range_x() ) ) );

        // TODO: add fixture and display number of cache miss in label
        close( fd );
    }

    void    bench_cache_vector_traversal( benchmark::State& state )
    {
        auto vector = create_container_random_values< std::vector< int > >( state.range_x() );
        start_traversal( vector, state );
    }

    void    bench_cache_list_traversal( benchmark::State& state )
    {
        auto list = create_container_random_values< std::list< int > >( state.range_x() );
        start_traversal( list, state );
    }

    void    bench_cache_shuffle_list_traversal( benchmark::State& state )
    {
        auto shuffleList = create_container_random_values< std::list< int > >( state.range_x() );
        shuffleList.sort();

        start_traversal( shuffleList, state );
    }
}

BENCHMARK( bench_cache_vector_traversal )->Apply( custom_args );
BENCHMARK( bench_cache_list_traversal )->Apply( custom_args );
BENCHMARK( bench_cache_shuffle_list_traversal )->Apply( custom_args );

// Unordered maps can be implemented in a variety of ways, with implications for the memory usage.The fundamental expectation is that there'll be a contiguous array of key/value "buckets",
// but in real-world implementations the basic design tradeoffs may involve:
//
// - two or more contiguous regions to reduce the performance cost when growing the container capacity, and separately
// - when there's a collision, an implementation may
//      (A) use an algorithm to select a sequence of alternative buckets or
//      (B) they may have each bucket be / point - to a resizable container of the key / value pairs.
//
//  Trying to make this latter choice more tangible : at its academic simplest, you can imagine :
//      (A) the hash table finding alternative "buckets" - as an array containing of key / value pairs, with empty / unused values scattered amongst the meaningful ones, akin to vector<optional<pair<key, value>>>.
//      (B) the hash table that instead uses containers is like a vector<list<pair<key, value>>> where every vector element is populated,
//          but getting from the vector elements to the lists involves extra pointers and discontiguous memory regions : this will be a little slower to deallocate as there are more distinct memory areas to delete.
//
// If the ratio of used to unused buckets is kept low, then there will be less collisions but more wasted memory.
// Another consideration : as the size of key / value pairs increase, the memory allocation overheads and pointers become less significant in comparison,
// so maps tend to use less memory than a hash map with low utilisation and values stored directly in the buckets.But, you can often create a hash map of key / pointers - to - value which mitigates that problem.
// So, there's the potential for a hash map to use less overall memory (particularly with small key/value types and a high used-to-unusued bucket ratio)
// and do less distinct allocations and deallocations as well as working better with caches, but it's far from guaranteed.
namespace
{
    void    bench_cache_unordered_map_traversal( benchmark::State& state )
    {
        std::unordered_set< int > unorderedSet;
        for ( auto i = 0; i < state.range_x(); ++i )
            unorderedSet.insert( i );

        start_traversal( unorderedSet, state );
    }

    void    bench_cache_map_traversal( benchmark::State& state )
    {
        std::set< int > set;
        for ( auto i = 0; i < state.range_x(); ++i )
            set.insert( i );

        start_traversal( set, state );
    }
}

BENCHMARK( bench_cache_unordered_map_traversal )->Apply( custom_args );
BENCHMARK( bench_cache_map_traversal )->Apply( custom_args );

// TODO: don't want to have a link to boost
/*
namespace
{

}

BENCHMARK( bench_cache_matrix_traversal_column_first )->Apply( custom_args );
BENCHMARK( bench_cache_matrix_traversal_row_first )->Apply( custom_args );
 */