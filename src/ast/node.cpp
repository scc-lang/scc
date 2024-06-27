module;

#include <utility>

export module scc.ast:ast_node;
import :source_range;

namespace scc::ast {

export struct Visitor;

export struct Node {
    SourceRange sourceRange;

    Node(SourceRange sourceRange)
        : sourceRange { std::move(sourceRange) }
    {
    }

    virtual ~Node() = default;

    virtual void Visit(Visitor& visitor) = 0;
};

}