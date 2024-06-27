module;

#include <utility>

export module scc.ast:ast_statement;
import :ast_node;
import :source_range;

namespace scc::ast {

export struct Statement : Node {
    Statement(SourceRange sourceRange)
        : Node { std::move(sourceRange) }
    {
    }
};

}