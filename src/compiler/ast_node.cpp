module;

#include <utility>

export module scc.compiler:ast_node;
import :source_range;

namespace scc::compiler {

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