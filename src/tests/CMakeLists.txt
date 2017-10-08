project(cpp_test)

if (NOT TARGET googletest)
    add_subdirectory(${PATH_GOOGLE_TEST} "${CMAKE_CURRENT_BINARY_DIR}/googletest")
endif()

if (NOT TARGET lib_common)
    add_subdirectory(${PATH_LIB_COMMON} "${CMAKE_CURRENT_BINARY_DIR}/lib_common")
endif() 

include_directories(${PATH_LIB_COMMON})
include_directories("${PATH_GOOGLE_TEST}/googletest/include")
include_directories("${PATH_GOOGLE_TEST}/googlemock/include")

aux_source_directory(tests SRC_TESTS)

add_executable(cpp_test ${SRC_TESTS})
find_package(Threads REQUIRED)
target_link_libraries(cpp_test
                      LINK_PRIVATE Threads::Threads
                      LINK_PRIVATE gtest
                      LINK_PRIVATE gmock
                      LINK_PRIVATE lib_common)
