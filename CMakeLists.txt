cmake_minimum_required(VERSION 3.2)

set(COMPILER_FLAGS_STANDARD "-std=c++2a -g -D_REENTRANT -fPIC -m64 -fmessage-length=0")
set(COMPILER_FLAGS_WARNING "-Werror -Wall -Wextra -Wno-deprecated-declarations -Wstrict-aliasing -Wno-unused-local-typedefs -Wno-unused-parameter \
                            -Wshadow -Wdouble-promotion -Wdeprecated-declarations")

# -fsanitize=undefined fail on appveyor (16.04)
set(CMAKE_CXX_FLAGS_DEBUG "-O1                                                          \
                           -fsanitize=address -fsanitize=leak                           \
                           -fno-sanitize=alignment")
# Disable '-fomit-frame-pointer' during profiling
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -fomit-frame-pointer")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(COMPILER_FLAGS_WARNING "${COMPILER_FLAGS_WARNING} -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wrestrict")
        #set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fdump-class-hierarchy")

        # TODO: -flto (gcc: ar / ranlib / nm)
        # might add -flto=8 -fno-fat-lto-objects -mfpmath=sse -finline-limit=???
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fdevirtualize-at-ltrans -floop-interchange -floop-strip-mine -floop-block -fgraphite-identity -funsafe-loop-optimizations \
                                                                -static-libstdc++")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Clang is only used for additionals warnings and usage of the sanitizers
        # (Not interested by llvm libc++ at the moment)
        set(COMPILER_FLAG_STANDARD "${COMPILER_FLAG_STANDARD} -stdlib=libstdc++")
endif()

set(CMAKE_CXX_FLAGS "${COMPILER_FLAGS_STANDARD} ${COMPILER_FLAGS_WARNING}")

set(PATH_FLAT_HASH_MAP "${CMAKE_CURRENT_SOURCE_DIR}/external/flat_hash_map")
set(PATH_GOOGLE_BENCHMARK "${CMAKE_CURRENT_SOURCE_DIR}/external/benchmark")
set(PATH_GOOGLE_TEST "${CMAKE_CURRENT_SOURCE_DIR}/external/googletest")
set(PATH_LIB_EXPERIMENTAL "${CMAKE_CURRENT_SOURCE_DIR}/experimental/experimental")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(experimental/experimental)
add_subdirectory(experimental/tests)
add_subdirectory(experimental/benchmarks)
