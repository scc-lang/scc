module;

#include <utility>

export module scc.ast:ast_node;
import :source_range;

namespace scc::ast {

export struct AstVisitor;

export struct AstNode {
    SourceRange sourceRange;

    AstNode(SourceRange sourceRange)
        : sourceRange { std::move(sourceRange) }
    {
    }

    virtual ~AstNode() = default;

    virtual void Visit(AstVisitor& visitor) = 0;
};

}