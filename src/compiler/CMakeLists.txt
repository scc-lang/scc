add_library(scc.compiler)
target_sources(scc.compiler PUBLIC FILE_SET CXX_MODULES FILES
    exception.cpp
    lexer.cpp
    module.cpp
    parser.cpp
    printer.cpp
    token.cpp
    translator.cpp
)
target_link_libraries(scc.compiler PUBLIC
    scc.ast
)