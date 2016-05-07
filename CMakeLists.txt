cmake_minimum_required(VERSION 3.2)
project(cpp_benchmark)

include(ExternalProject)
ExternalProject_Add( google_benchmark
                     GIT_REPOSITORY https://github.com/google/benchmark.git
                     GIT_TAG v1.0.0
                     PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/tmp/benchmark
                     CMAKE_ARGS
                     -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/external
                     -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} )

link_directories(${CMAKE_CURRENT_BINARY_DIR}/external/lib)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/external/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

#find_package(Boost 1.60.0 COMPONENTS program_options REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})

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

add_dependencies(cpp_benchmark google_benchmark)
target_include_directories(cpp_benchmark PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/external/include)

find_package(Threads REQUIRED)
target_link_libraries( cpp_benchmark
                       LINK_PRIVATE Threads::Threads
                       LINK_PRIVATE benchmark )
