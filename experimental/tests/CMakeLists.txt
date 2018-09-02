project(test_experimental)

if (NOT TARGET googletest)
    add_subdirectory(${PATH_GOOGLE_TEST} "${CMAKE_CURRENT_BINARY_DIR}/googletest")
endif()

if (NOT TARGET lib_experimental)
    add_subdirectory(${PATH_LIB_EXPERIMENTAL} "${CMAKE_CURRENT_BINARY_DIR}/experimental")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tests)
include_directories(${PATH_LIB_EXPERIMENTAL})
include_directories("${PATH_GOOGLE_TEST}/googletest/include")
include_directories("${PATH_GOOGLE_TEST}/googlemock/include")
include_directories(${PATH_FLAT_HASH_MAP})

aux_source_directory(tests SRC_TESTS)
aux_source_directory(tests/libstd SRC_LIBSTD_TESTS)
aux_source_directory(tests/utils SRC_UTILS_TESTS)

add_executable(test_experimental ${SRC_TESTS} ${SRC_LIBSTD_TESTS} ${SRC_UTILS_TESTS})

find_package(Threads REQUIRED)
target_link_libraries(test_experimental
                      LINK_PRIVATE Threads::Threads
                      LINK_PRIVATE gtest
                      LINK_PRIVATE gmock
                      LINK_PRIVATE lib_experimental)

add_test(NAME TEST_EXPERIMENTAL COMMAND $<TARGET_FILE:test_experimental>)
