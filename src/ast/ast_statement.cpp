module;

#include <utility>

export module scc.ast:ast_statement;
import :ast_node;
import :source_range;

namespace scc::ast {

export struct AstStatement : AstNode {
    AstStatement(SourceRange sourceRange)
        : AstNode { std::move(sourceRange) }
    {
    }
};

}