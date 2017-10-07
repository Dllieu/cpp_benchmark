project(lib_common)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
aux_source_directory(allocators SRC_ALLOCATORS)
aux_source_directory(utils SRC_UTILS)

add_library(lib_common ${SRC_ALLOCATORS}
                       ${SRC_UTILS})

set_target_properties(lib_common PROPERTIES PREFIX "")
