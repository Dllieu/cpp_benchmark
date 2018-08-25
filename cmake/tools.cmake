file(GLOB_RECURSE ALL_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/*.cpp)
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX ".*CMakeCXXCompilerId.cpp$")

file(GLOB_RECURSE ALL_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/*.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/*.hxx)
list(APPEND ALL_CPP_FILES ${ALL_SOURCE_FILES})

function(ensure_program_exist PROGRAM_NAME PATH_PROGRAM)
    find_program(${PATH_PROGRAM} NAMES ${PROGRAM_NAME})

    if(NOT ${PATH_PROGRAM})
        message(FATAL_ERROR "${PROGRAM_NAME} not found!")
    else()
        message("${PROGRAM_NAME} found")
    endif()
endfunction(ensure_program_exist)

# cppcheck
ensure_program_exist("cppcheck" CPPCHECK_PATH)

add_custom_target(cppcheck
                  COMMAND ${CPPCHECK_PATH}
                  --std=c++14
                  --language=c++
                  --quiet
                  --inconclusive
                  --xml-version=2

                  --enable=all
                  --suppressions-list=${CMAKE_CURRENT_SOURCE_DIR}/cmake/cppcheck_listing.csv
                  ${ALL_CPP_FILES})

# clang-format
ensure_program_exist("clang-format" CLANG_FORMAT_PATH)

add_custom_target(clang-format-xml
                  COMMAND ${CLANG_FORMAT_PATH}
                  -style=file
                  -output-replacements-xml
                  ${ALL_CPP_FILES})

add_custom_target(clang-format-inplace
                  COMMAND ${CLANG_FORMAT_PATH}
                  -i
                  ${ALL_CPP_FILES})

# clang-tidy
ensure_program_exist("clang-tidy" CLANG_TIDY_PATH)

add_custom_target(clang-tidy
                  COMMAND ${CLANG_TIDY_PATH}
                  -p ${CMAKE_BINARY_DIR}/
                  -config ''
                  ${ALL_SOURCE_FILES})
