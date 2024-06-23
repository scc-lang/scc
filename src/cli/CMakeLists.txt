add_library(scc.cli)
target_sources(scc.cli PUBLIC FILE_SET CXX_MODULES FILES
    commandline_processor.cpp
    module.cpp
)

add_executable(scc
    main.cpp
)
target_link_libraries(scc
    scc.cli
)