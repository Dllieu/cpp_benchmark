project(cpp_benchmark)

if (NOT TARGET benchmark)
    option(BENCHMARK_ENABLE_TESTING "Enable testing of the benchmark library." OFF)
    set(BENCHMARK_ENABLE_TESTING OFF)
    add_subdirectory(${PATH_GOOGLE_BENCHMARK} "${CMAKE_CURRENT_BINARY_DIR}/benchmark")
endif()

if (NOT TARGET lib_common)
    add_subdirectory(${PATH_LIB_COMMON} "${CMAKE_CURRENT_BINARY_DIR}/lib_common")
endif() 

include_directories(${PATH_LIB_COMMON})
aux_source_directory(benchmarks SRC_BENCHMARKS)

add_executable(cpp_benchmark ${SRC_BENCHMARKS})

find_package(Threads REQUIRED)
target_link_libraries(cpp_benchmark
                      LINK_PRIVATE Threads::Threads
                      LINK_PRIVATE benchmark
                      LINK_PRIVATE lib_common)
