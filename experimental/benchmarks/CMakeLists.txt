project(benchmark_experimental)

if (NOT TARGET benchmark)
    option(BENCHMARK_ENABLE_TESTING "Enable testing of the benchmark library." OFF)
    set(BENCHMARK_ENABLE_TESTING OFF)
    add_subdirectory(${PATH_GOOGLE_BENCHMARK} "${CMAKE_CURRENT_BINARY_DIR}/benchmark")
endif()

if (NOT TARGET lib_experimental)
    add_subdirectory(${PATH_LIB_EXPERIMENTAL} "${CMAKE_CURRENT_BINARY_DIR}/experimental")
endif()

include_directories(${PATH_LIB_EXPERIMENTAL})
include_directories("${PATH_GOOGLE_BENCHMARK}/include")
aux_source_directory(benchmarks SRC_BENCHMARKS)

add_executable(benchmark_experimental ${SRC_BENCHMARKS})
find_package(Threads REQUIRED)
target_link_libraries(benchmark_experimental
                      LINK_PRIVATE Threads::Threads
                      LINK_PRIVATE benchmark
                      LINK_PRIVATE lib_experimental)
