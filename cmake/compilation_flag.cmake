set(COMPILER_FLAGS_STANDARD "-std=c++2a -g -D_REENTRANT -fPIC -m64 -fmessage-length=0")
set(COMPILER_FLAGS_WARNING "-Werror -Wall -Wextra -Wno-deprecated-declarations -Wstrict-aliasing -Wshadow -Wpedantic -Wdouble-promotion \
                            -Wdeprecated-declarations -Wnon-virtual-dtor -Wcast-align -Woverloaded-virtual -Wcast-qual \
                            -Wno-unused-local-typedefs -Wno-unused-parameter")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_DEBUG -fno-inline \
                           -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-sanitize=alignment")

# Disable '-fomit-frame-pointer' during profiling
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -fomit-frame-pointer")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(COMPILER_FLAGS_WARNING "${COMPILER_FLAGS_WARNING} -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wrestrict")

    # gcov
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")

    # TODO: -flto -fno-fat-lto-objects -flto-odr-type-merging -Wno-odr (benchmark generate ODR violations)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fdevirtualize-at-ltrans -floop-interchange -floop-strip-mine -floop-block \
                                                            -fgraphite-identity -funsafe-loop-optimizations \
                                                            -static-libgcc -static-libstdc++")
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        SET(BENCHMARK_ENABLE_LTO ON CACHE BOOL "Build google benchmark with lto")
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang is only used for additionals warnings and usage of the sanitizers
    # (Not interested by llvm libc++ at the moment)
    set(COMPILER_FLAG_STANDARD "${COMPILER_FLAG_STANDARD} -stdlib=libstdc++")

    # Do not mix with --coverage flags
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
endif()

set(CMAKE_CXX_FLAGS "${COMPILER_FLAGS_STANDARD} ${COMPILER_FLAGS_WARNING}")
