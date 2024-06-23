add_library(scc.compiler)
target_sources(scc.compiler PUBLIC FILE_SET CXX_MODULES FILES
    ast_expression.cpp
    ast_function_call_expression.cpp
    ast_identifier_expression.cpp
    ast_node.cpp
    ast_scope.cpp
    ast_statement.cpp
    ast_string_literal_expression.cpp
    exception.cpp
    lexer.cpp
    module.cpp
    parser.cpp
    source_range.cpp
    token.cpp
)