project(benchmark_experimental)

if (NOT TARGET benchmark)
    option(BENCHMARK_ENABLE_TESTING "Enable testing of the benchmark library." OFF)
    set(BENCHMARK_ENABLE_TESTING OFF)
    add_subdirectory(${PATH_GOOGLE_BENCHMARK} ${CMAKE_CURRENT_BINARY_DIR}/benchmark)
endif()

if (NOT TARGET lib_experimental)
    add_subdirectory(${PATH_LIB_EXPERIMENTAL} ${CMAKE_CURRENT_BINARY_DIR}/experimental)
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${PATH_LIB_EXPERIMENTAL})
include_directories(${PATH_FLAT_HASH_MAP})
include_directories(${PATH_GOOGLE_BENCHMARK}/include)
include_directories(${PATH_GOOGLE_SPARSEHASH})

aux_source_directory(benchmarks SRC_BENCHMARKS)
aux_source_directory(benchmarks/cache SRC_CACHE)
aux_source_directory(benchmarks/hashtable SRC_HASHTABLE)
aux_source_directory(benchmarks/threading SRC_THREADING)

add_executable(benchmark_experimental ${SRC_BENCHMARKS} ${SRC_CACHE} ${SRC_HASHTABLE} ${SRC_THREADING})
find_package(Threads REQUIRED)
target_link_libraries(benchmark_experimental
                      LINK_PRIVATE Threads::Threads
                      LINK_PRIVATE benchmark
                      LINK_PRIVATE lib_experimental)
