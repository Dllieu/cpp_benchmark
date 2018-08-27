#pragma once

#include <bits/wordsize.h>
#include <cstddef>
#include <utils/user_defined_literals.h>

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
//             * 1 per core (hyper threading delivers two processing threads per physical core, shared by two HW thread)
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
// - Memory is transferred from the main memory into the caches in blocks which are smaller than the cache line size.Today 64 bits are transferred at once and the cache line size is (usually) 64 bytes (This means 8 or 16 transfers per cache line are
// needed)
// - The DRAM chips can transfer those 64-byte blocks in burst mode.This can fill the cache line without any further commands from the memory controller and the possibly associated delays. If the processor prefetches cache lines this is probably the
// best way to operate
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
//  - could possibly have cache miss on unlinked data (two static data that are put on the same cache line, N global on different translation unit could also be put in same cache line, same thing for different dynamic allocation, or stack for that
//  matter)
//  - As data cache is 8-way associative, we can know in which block a cache line is depending of the address of the data (i.e. beginning_adress_cache_line(address) % 8)
//    theoretically we could only work with data that are contains in the same cache block which could result in horrible performance as we would only use 1/8 of the cache available

// About Instruction Cache friendly
// Any code that changes the flow of execution affects the Instruction Cache. This includes function calls and loops as well as dereferencing function pointers.
// - When a branch or jump instruction is executed, the processor has to spend extra time deciding if the code is already in the instruction cache or whether it needs to reload the instruction cache( from the destination of the branch ).
// - For example, some processors may have a large enough instruction cache to hold the execution code for small loops.Some processors don't have a large instruction cache and simple reload it. Reloading of the instruction cache takes time that could
// be spent executing instructions. What can help
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
namespace experimental
{
    constexpr const auto PAGE_SIZE = 4'096;
    // In bytes
    constexpr const auto WORD_SIZE = __WORDSIZE / 8;

    // Max number of segment in L1 = 32KB / 64 = 512
    // Specific Intel Core i5-4460
    enum class CacheSize
    {
        L1 = 32_KB,
        L2 = 256_KB,
        L3 = 6_MB,
        DRAM
    };

    const char* to_string(CacheSize cacheSize);

    template <typename T>
    CacheSize byteToAppropriateCacheSize(std::size_t numberElements);
}

#include <utils/cache_information.hxx>
