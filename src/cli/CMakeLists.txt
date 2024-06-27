add_library(scc.cli)
target_sources(scc.cli PUBLIC FILE_SET CXX_MODULES FILES
    commandline_processor.cpp
    module.cpp
)
target_link_libraries(scc.cli PUBLIC
    scc.compiler
)

add_executable(scc
    main.cpp
)
target_link_libraries(scc PUBLIC
    scc.cli
)