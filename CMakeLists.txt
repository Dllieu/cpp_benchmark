cmake_minimum_required(VERSION 3.10)

project(experimental CXX)

include(cmake/compilation_flag.cmake)
include(cmake/external.cmake)
include(cmake/tools.cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

enable_testing()
add_subdirectory(experimental/experimental)
add_subdirectory(experimental/tests)
add_subdirectory(experimental/benchmarks)
