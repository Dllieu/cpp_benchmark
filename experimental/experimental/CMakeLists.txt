project(lib_experimental)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
aux_source_directory(allocators SRC_ALLOCATORS)
aux_source_directory(messages SRC_MESSAGES)
aux_source_directory(messages/default_messages SRC_MESSAGES_DEFAULT_MESSAGES)
aux_source_directory(network SRC_NETWORK)
aux_source_directory(properties SRC_PROPERTIES)
aux_source_directory(utils SRC_UTILS)

add_library(lib_experimental ${SRC_ALLOCATORS}
                             ${SRC_MESSAGES}
                             ${SRC_MESSAGES_DEFAULT_MESSAGES}
                             ${SRC_NETWORK}
                             ${SRC_PROPERTIES}
                             ${SRC_UTILS})

set_target_properties(lib_experimental PROPERTIES PREFIX "")
