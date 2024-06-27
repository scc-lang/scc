module;

#include <utility>

export module scc.ast:ast_expression;
import :ast_node;
import :source_range;

namespace scc::ast {

export struct AstExpression : AstNode {
    AstExpression(SourceRange sourceRange)
        : AstNode { std::move(sourceRange) }
    {
    }
};

}