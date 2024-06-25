module;

#include <utility>

export module scc.compiler:ast_loop_statement;
import :ast_scope;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::compiler {

export struct AstLoopStatement final : AstStatement {
    AstScope scope;

    AstLoopStatement(SourceRange sourceRange, AstScope scope)
        : AstStatement { std::move(sourceRange) }
        , scope { std::move(scope) }
    {
    }

    void Visit(AstVisitor& visitor) override
    {
        visitor.VisitAstLoopStatement(*this);
    }
};

}