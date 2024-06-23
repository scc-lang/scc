add_library(scc.compiler)
target_sources(scc.compiler PUBLIC FILE_SET CXX_MODULES FILES
    exception.cpp
    lexer.cpp
    module.cpp
    token.cpp
)