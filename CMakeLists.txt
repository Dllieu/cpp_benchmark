cmake_minimum_required(VERSION 3.2)
project(cpp_benchmark)

option(BENCHMARK_ENABLE_TESTING "Enable testing of the benchmark library." OFF)

set(BENCHMARK_ENABLE_TESTING OFF)
add_subdirectory(external/benchmark)

## FLAGS
# Standard Flags release (might add --gdb  -static-libgcc -static-libstdc++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++1z -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -fPIC -m64")
# Warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-deprecated-declarations -Wno-unused-local-typedefs -Wno-unused-parameter -fmessage-length=0")
# Perf flags -fno-exceptions -flto=8 -fno-fat-lto-objects -mfpmath=sse -fno-omit-frame-pointer
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
if (CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdevirtualize-at-ltrans -floop-interchange -floop-strip-mine -floop-block -fgraphite-identity -funsafe-loop-optimizations -march=native")
endif()

aux_source_directory(src SRC_LIST)
aux_source_directory(src/utils UTILS_LIST)
add_executable(cpp_benchmark ${SRC_LIST} ${UTILS_LIST})
add_dependencies(cpp_benchmark benchmark)

find_package(Threads REQUIRED)
target_link_libraries( cpp_benchmark
                       LINK_PRIVATE Threads::Threads
                       LINK_PRIVATE benchmark )
