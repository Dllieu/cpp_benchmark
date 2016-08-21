cmake_minimum_required(VERSION 3.2)
project(cpp_benchmark)

option(BENCHMARK_ENABLE_TESTING "Enable testing of the benchmark library." OFF)

set(BENCHMARK_ENABLE_TESTING OFF)
add_subdirectory(external/benchmark)

# FLAGS
## might add --gdb  -static-libgcc -static-libstdc++
set(COMPILER_FLAG_STANDARD "-O3 -std=c++1z -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -fPIC -m64")
set(COMPILER_FLAG_WARNING "-Wall -Wextra -Wno-deprecated-declarations -Wno-unused-local-typedefs -Wno-unused-parameter -fmessage-length=0")
if (CMAKE_COMPILER_IS_GNUCXX)
        # might add -fno-exceptions -flto=8 -fno-fat-lto-objects -mfpmath=sse -fomit-frame-pointer
        # -fomit-frame-pointer: get one more register available for the register allocator, and save 1-3 instructions on entry/exit to functions, not supported by all architecture, should not omit for measurement (e.g. vtune)
        set(COMPILER_FLAG_PERF "-fdevirtualize-at-ltrans -floop-interchange -floop-strip-mine -floop-block -fgraphite-identity -funsafe-loop-optimizations -march=native")
else()
        set(COMPILER_FLAG_PERF "-march=native")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAG_STANDARD} ${COMPILER_FLAG_WARNING} ${COMPILER_FLAG_PERF}")
# !FLAGS

aux_source_directory(src SRC_LIST)
aux_source_directory(src/utils UTILS_LIST)
add_executable(cpp_benchmark ${SRC_LIST} ${UTILS_LIST})
add_dependencies(cpp_benchmark benchmark)

find_package(Threads REQUIRED)
target_link_libraries( cpp_benchmark
                       LINK_PRIVATE Threads::Threads
                       LINK_PRIVATE benchmark )
