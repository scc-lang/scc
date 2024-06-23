module;

#include <utility>

export module scc.compiler:ast_expression;
import :ast_node;
import :source_range;

namespace scc::compiler {

export struct AstExpression : AstNode {
    AstExpression(SourceRange sourceRange)
        : AstNode { std::move(sourceRange) }
    {
    }
};

}