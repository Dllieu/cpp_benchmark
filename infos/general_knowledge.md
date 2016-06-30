# Processor Architecture
##MESIF

TODO: Images
*To remember: clean / dirty / request for ownership*

##Store forwarding
If a load follows a store and reloads the data that the store writes to memory, the data can forward directly from the store operation to the load. This process, called store to load forwarding, saves cycles by enabling the load to obtain the data directly from the store operation instead of through memory.

##Last level cache
The LLC consists of multiple cache slices. The number of slices is equal to the number of IA cores. Each slice has logic portion and data array portion. The logic portion handles data coherency, memory ordering, access to the data array portion, LLC misses and writeback to memory, and more. The data array portion stores cache lines. Each slice contains a full cache port that can supply 32 bytes/cycle.

The physical addresses of data kept in the LLC data arrays are distributed among the cache slices by a hash function, such that addresses are uniformly distributed. The data array in a cache block may have 4/8/12/16 ways corresponding to 0.5M/1M/1.5M/2M block size. However, due to the address distribution among the cache blocks from the software point of view, this does not appear as a normal N-way cache.

##Writing policies

TODO: Images (https://en.wikipedia.org/wiki/Cache_(computing))
- Write-through: write is done synchronously both to the cache and to the backing store.
- Write-back (also called write-behind): initially, writing is done only to the cache. The write to the backing store is postponed until the cache blocks containing the data are about to be modified/replaced by new content.

##Load prefetch
###L1
Triggered by load operations when the following conditions are met:
- Load is from writeback memory type.
- The prefetched data is within the same 4K byte page as the load instruction that triggered it
- No fence is in progress in the pipeline.
- Not many other load misses are in progress.
- There is not a continuous stream of stores.
Two hardware prefetchers load data to the L1 DCache:
- Data cache unit (DCU) prefetcher. This prefetcher, also known as the streaming prefetcher, is triggered by an ascending access to very recently loaded data. The processor assumes that this access is part of a streaming algorithm and automatically fetches the next line.
- Instruction pointer (IP)-based stride prefetcher. This prefetcher keeps track of individual load instructions. If a load instruction is detected to have a regular stride, then a prefetch is sent to the next address which is the sum of the current address and the stride. This prefetcher can prefetch forward or backward and can detect strides of up to 2K bytes

###L2
The following two hardware prefetchers fetched data from memory to the L2 cache and last level cache:
- Spatial Prefetcher: This prefetcher strives to complete every cache line fetched to the L2 cache with the pair line that completes it to a 128-byte aligned chunk.
- Streamer: This prefetcher monitors read requests from the L1 cache for ascending and descending sequences of addresses. Monitored read requests include L1 DCache requests initiated by load and store operations and by the hardware prefetchers, and L1 ICache requests for code fetch. When a forward or backward stream of requests is detected, the anticipated cache lines are prefetched. Prefetched cache lines must be in the same 4K page. The streamer and spatial prefetcher prefetch the data to the last level cache. Typically data is brought also to the L2 unless the L2 cache is heavily loaded with missing demand requests.

##Intel Core Ivy bridge Microarchitecture
###Intel® Wide Dynamic Execution
- enables each processor core to fetch, dispatch, execute with high bandwidths and retire up to four instructions per cycle.
- Fourteen-stage efficient pipeline
- Three arithmetic logical units
- Four decoders to decode up to five instruction per cycle
- Macro-fusion and micro-fusion to improve front-end throughput
- Peak issue rate of dispatching up to six μops per cycle
- Peak retirement bandwidth of up to four μops per cycle
- Advanced branch prediction
- Stack pointer tracker to improve efficiency of executing function/procedure entries and exits

###Intel® Advanced Smart Cache
- Delivers higher bandwidth from the second level cache to the core, optimal performance and flexibility for single-threaded and multi-threaded applications.
- Optimized for multicore and single-threaded execution environments
- 256 bit internal data path to improve bandwidth from L2 to first-level data cache
- Unified, shared second-level cache of 4 Mbyte, 16 way (or 2 MByte, 8 way)

###Intel® Smart Memory Access
- prefetches data from memory in response to data access patterns and reduces cache-miss exposure of out-of-order execution.
- Hardware prefetchers to reduce effective latency of second-level cache misses
- Hardware prefetchers to reduce effective latency of first-level data cache misses
- Memory disambiguation to improve efficiency of speculative execution execution engine

###Intel® Advanced Digital Media Boost
- improves most 128-bit SIMD instructions with single-cycle throughput and floating-point operations.
- Single-cycle throughput of most 128-bit SIMD instructions (except 128-bit shuffle, pack, unpack operations)
- Up to eight floating-point operations per cycle
- Three issue ports available to dispatching SIMD instructions for execution.

*The Enhanced Intel Core microarchitecture supports all of the features of Intel Core microarchitecture and provides a comprehensive set of enhancements.*
###Intel® Wide Dynamic Execution includes several enhancements:
- A radix-16 divider replacing previous radix-4 based divider to speedup longlatency operations such as divisions and square roots.
- Improved system primitives to speedup long-latency operations such as RDTSC, STI, CLI, and VM exit transitions.

###Intel® Advanced Smart Cache
- provides up to 6 MBytes of second-level cache shared between two processor cores (quad-core processors have up to 12 MBytes of L2); up to 24 way/set associativity.

###Intel® Smart Memory Access
- supports high-speed system bus up 1600 MHz and provides more efficient handling of memory operations such as split cache line load and store-to-load forwarding situations.

###Intel® Advanced Digital Media Boost
- Provides 128-bit shuffler unit to speedup shuffle, pack, unpack operations; adds support for 47 SSE4.1 instructions

##Pipeline
TODO: image p.68

###Front end components
- Part of the CPU is the piece of hardware responsible to fetch and decode instructions (convert them to UOPs)
- The front ends needs to supply decoded instructions (μops) and sustain the stream to a six-issue wide out-of-order engine

####Branch Prediction Unit (BPU)
- Helps the instruction fetch unit fetch the most likely instruction to be executed by predicting the various branch types:
 - conditional
 - indirect
 - direct
 - call
 - return
- Uses dedicated hardware for each type
- Performance Challenge
 - Enables speculative execution
 - Improves speculative execution efficiency by reducing the amount of code in the “non-architected path” to be fetched into the pipeline

####Instruction Fetch Unit
- Prefetches instructions that are likely to be executed
- Caches frequently-used instructions
- Predecodes and buffers instructions, maintaining a constant bandwidth despite irregularities in the instruction stream 
- Performance Challenges
 - Variable length instruction format causes unevenness (bubbles) in decode bandwidth
 - Taken branches and misaligned targets causes disruptions in the overall bandwidth delivered by the fetch unit
 
####Instruction Queue and Decode Unit
- Decodes up to four instructions, or up to five with macro-fusion
- Stack pointer tracker algorithm for efficient procedure entry and exit
- Implements the Macro-Fusion feature, providing higher performance and efficiency
- The Instruction Queue is also used as a loop cache, enabling some loops to be executed with both higher bandwidth and lower power
- Performance Challenges
 - Varying amounts of work per instruction requires expansion into variable numbers of μops
 - Prefix adds a dimension of decoding complexity
 - Length Changing Prefix (LCP) can cause front end bubbles
