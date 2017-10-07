cmake_minimum_required(VERSION 3.2)
# GOOD TO KNOW
# - to check which flag gcc are used, do make with VERBOSE=1

# FLAGS
## might add debug flag -fdump-class-hierarchy for investigating vtables entries
## might add --ggdb4  -static-libgcc -static-libstdc++
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

set(PATH_GOOGLE_BENCHMARK "${CMAKE_CURRENT_SOURCE_DIR}/external/benchmark")
set(PATH_GOOGLE_TEST "${CMAKE_CURRENT_SOURCE_DIR}/external/googletest")
set(PATH_LIB_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/src/lib_common")

add_subdirectory(src/lib_common)
add_subdirectory(src/benchmarks)
