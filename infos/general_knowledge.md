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

##Instructions
http://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.pdf
p. 31

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
- 16-entry Return Stack Buffer (RSB). It enables the BPU to accurately predict RET instructions
- Front end queuing of BPU lookups. The BPU makes branch predictions for 32 bytes at a time, twice the width of the fetch engine
- The BPU makes the following types of predictions:
 - Direct Calls and Jumps. Targets are read as a target array, without regarding the taken or not-taken prediction.
 - Indirect Calls and Jumps. These may either be predicted as having a monotonic target or as having targets that vary in accordance with recent program behavior.
 - Conditional branches. Predicts the branch target and whether or not the branch will be taken.
- Performance Challenge
 - Enables speculative execution
 - Improves speculative execution efficiency by reducing the amount of code in the “non-architected path” to be fetched into the pipeline

####Instruction Fetch Unit
- An instruction fetch is a 16-byte aligned lookup through the ITLB into the instruction cache and instruction prefetch buffers. A hit in the instruction cache causes 16 bytes to be delivered to the instruction predecoder. Typical programs average slightly less than 4 bytes per instruction, depending on the code being executed
- Prefetches instructions that are likely to be executed
- Caches frequently-used instructions
- Predecodes and buffers instructions, maintaining a constant bandwidth despite irregularities in the instruction stream, The predecode unit accepts the sixteen bytes from the instruction cache or prefetch buffers and carries out the following tasks:
 - Determine the length of the instructions.
 - Decode all prefixes associated with instructions.
 - Mark various properties of instructions for the decoders (for example, "is branch")
- The predecode unit can write up to six instructions per cycle into the instruction queue. If a fetch contains more than six instructions, the predecoder continues to decode up to six instructions per cycle until all instructions in the fetch are written to
the instruction queue. Subsequent fetches can only enter predecoding after the current fetch completes. For a fetch of seven instructions, the predecoder decodes the first six in one cycle, and then only one in the next cycle
- The following instruction prefixes cause problems during length decoding. These prefixes can dynamically change the length of instructions and are known as length changing prefixes (LCPs):
 - Operand Size Override (66H) preceding an instruction with a word immediate data
 - Address Size Override (67H) preceding an instruction with a mod R/M in real, 16-bit protected or 32-bit protected modes
- When the predecoder encounters an LCP in the fetch line, it must use a slower length decoding algorithm. With the slower length decoding algorithm, the predecoder decodes the fetch in 6 cycles, instead of the usual 1 cycle
- Performance Challenges
 - Variable length instruction format causes unevenness (bubbles) in decode bandwidth
 - Taken branches and misaligned targets causes disruptions in the overall bandwidth delivered by the fetch unit
 
####Instruction Queue and Decode Unit
- The instruction queue is 18 instructions deep. It sits between the instruction predecode unit and the instruction decoders
- Decodes up to four instructions by cycle, or up to five with macro-fusion (only one by cycle) (allow CPU to join two related instructions into a single one, micro-fusion joins two relates micro-instruction into a single one)
- It also serves as a loop cache for loops smaller than 18 instructions
- A Loop Stream Detector (LSD) resides in the BPU. The LSD attempts to detect loops which are candidates for streaming from the instruction queue (IQ). When such a loop is detected, the instruction bytes are locked down and the loop is allowed to stream from the IQ until a misprediction ends it. When the loop plays back from the IQ, it provides higher bandwidth at reduced power
- The LSD provides the following benefits:
 - No loss of bandwidth due to taken branches
 - No loss of bandwidth due to misaligned instructions
 - No LCP penalties, as the pre-decode stage has already been passed
 - Reduced front end power consumption, because the instruction cache, BPU and predecode unit can be idle
 - Software should use the loop cache functionality opportunistically. Loop unrolling and other code optimizations may make the loop too big to fit into the LSD. For high performance code, loop unrolling is generally preferable for performance even when it overflows the loop cache capability
- The Intel Core microarchitecture contains four instruction decoders. The first, Decoder 0, can decode Intel 64 and IA-32 instructions up to 4 μops in size. Three other decoders handles single-μop instructions. The microsequencer (Each microinstruction has an 8-bit "next" field, which tells which microinstruction follows, if branch not to be taken, microcode sequence aborts before it finished) can provide up to 3 μops per cycle, and helps decode instructions larger than 4 μops.
- All decoders support the common cases of single μop flows, including: micro-fusion, stack pointer tracking and macro-fusion. Thus, the three simple decoders are not limited to decoding single-μop instructions. Packing instructions into a 4-1-1-1 template is not necessary and not recommended. Macro-fusion merges two instructions into a single μop. Intel Core microarchitecture is capable of one macro-fusion per cycle in 32-bit operation (including compatibility sub-mode of the Intel 64 architecture), but not in 64-bit mode because code that uses longer instructions (length in bytes) more often is less likely to take advantage of hardware support for macro-fusion. 
- Stack pointer tracker algorithm for efficient procedure entry and exit
- Implements the Macro-Fusion feature, providing higher performance and efficiency
- The Instruction Queue is also used as a loop cache, enabling some loops to be executed with both higher bandwidth and lower power
- Performance Challenges
 - Varying amounts of work per instruction requires expansion into variable numbers of μops
 - Prefix adds a dimension of decoding complexity
 - Length Changing Prefix (LCP) can cause front end bubbles
