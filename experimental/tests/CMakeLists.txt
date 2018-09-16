project(test_experimental)

if (NOT TARGET googletest)
    add_subdirectory(${PATH_GOOGLE_TEST} ${CMAKE_CURRENT_BINARY_DIR}/googletest)
endif()

if (NOT TARGET lib_experimental)
    add_subdirectory(${PATH_LIB_EXPERIMENTAL} ${CMAKE_CURRENT_BINARY_DIR}/experimental)
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${PATH_FLAT_HASH_MAP})
include_directories(${PATH_GOOGLE_TEST}/googletest/include)
include_directories(${PATH_GOOGLE_TEST}/googlemock/include)
include_directories(${PATH_LIB_EXPERIMENTAL})

aux_source_directory(tests SRC_TESTS)
aux_source_directory(tests/libstd SRC_LIBSTD_TESTS)
aux_source_directory(tests/network SRC_NETWORK_TESTS)
aux_source_directory(tests/utils SRC_UTILS_TESTS)

add_executable(test_experimental ${SRC_TESTS} ${SRC_LIBSTD_TESTS} ${SRC_NETWORK_TESTS} ${SRC_UTILS_TESTS})
find_package(Threads REQUIRED)
target_link_libraries(test_experimental
                      LINK_PRIVATE Threads::Threads
                      LINK_PRIVATE gtest
                      LINK_PRIVATE gmock
                      LINK_PRIVATE lib_experimental)

add_custom_command(TARGET test_experimental PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/tests/data $<TARGET_FILE_DIR:test_experimental>/tests/data)
add_test(NAME TEST_EXPERIMENTAL COMMAND $<TARGET_FILE:test_experimental>)
