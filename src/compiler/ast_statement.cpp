module;

#include <utility>

export module scc.compiler:ast_statement;
import :ast_node;
import :source_range;

namespace scc::compiler {

export struct AstStatement : AstNode {
    AstStatement(SourceRange sourceRange)
        : AstNode { std::move(sourceRange) }
    {
    }
};

}