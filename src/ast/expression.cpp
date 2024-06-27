module;

#include <utility>

export module scc.ast:ast_expression;
import :ast_node;
import :source_range;

namespace scc::ast {

export struct Expression : Node {
    Expression(SourceRange sourceRange)
        : Node { std::move(sourceRange) }
    {
    }
};

}