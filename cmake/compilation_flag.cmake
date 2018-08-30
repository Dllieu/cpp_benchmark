set(COMPILER_FLAGS_STANDARD "-std=c++2a -g -D_REENTRANT -fPIC -m64 -fmessage-length=0")
set(COMPILER_FLAGS_WARNING "-Werror -Wall -Wextra -Wno-deprecated-declarations -Wstrict-aliasing -Wshadow -Wpedantic -Wdouble-promotion \
                            -Wdeprecated-declarations -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wabi -Wcast-qual \
                            -Wno-unused-local-typedefs -Wno-unused-parameter")

# -fsanitize=undefined fail on appveyor (16.04)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_DEBUG -fsanitize=address -fsanitize=leak -fno-sanitize=alignment -fno-inline")

# Disable '-fomit-frame-pointer' during profiling
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -fomit-frame-pointer")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(COMPILER_FLAGS_WARNING "${COMPILER_FLAGS_WARNING} -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wrestrict")

    # gcov
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fdevirtualize-at-ltrans -floop-interchange -floop-strip-mine -floop-block -fgraphite-identity -funsafe-loop-optimizations -static-libstdc++")

    # # Bin utils issue with gcc 8 on 16.04
    # # -flto -fno-fat-lto-objects -Wno-odr
    # if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    #         SET(BENCHMARK_ENABLE_LTO ON CACHE BOOL "Build google benchmark with lto")
    # endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang is only used for additionals warnings and usage of the sanitizers
    # (Not interested by llvm libc++ at the moment)
    set(COMPILER_FLAG_STANDARD "${COMPILER_FLAG_STANDARD} -stdlib=libstdc++")

    # llvm-cov gcov need -fprofile-arcs -ftest-coverage (or --coverage)
    # llvm-cov show need -fprofile-instr-generate -fcoverage-mapping
    # Cannot activate the gcov flag otherwise it won't be compatible with the flag required by show
    # llvm-cov show / PGO
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
endif()

set(CMAKE_CXX_FLAGS "${COMPILER_FLAGS_STANDARD} ${COMPILER_FLAGS_WARNING}")
