add_library(scc.std)
target_sources(scc.std PUBLIC FILE_SET CXX_MODULES FILES
    print/println.cpp
    module.cpp
)
add_custom_command(TARGET scc.std POST_BUILD
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/../cli/std
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/scc.std.dir/*.pcm ${CMAKE_CURRENT_BINARY_DIR}/../cli/std
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/*.a ${CMAKE_CURRENT_BINARY_DIR}/../cli/std
)