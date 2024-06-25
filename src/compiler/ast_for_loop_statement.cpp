module;

#include <memory>

export module scc.compiler:ast_for_loop_statement;
import :ast_expression;
import :ast_scope;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::compiler {

export struct AstForLoopStatement final : AstStatement {
    AstScope initScope {};
    std::unique_ptr<AstExpression> conditionalExpression {};
    std::unique_ptr<AstExpression> iterationExpression {};
    AstScope bodyScope {};

    AstForLoopStatement(SourceRange sourceRange, AstScope initScope, std::unique_ptr<AstExpression> conditionalExpression, std::unique_ptr<AstExpression> iterationExpression, AstScope bodyScope)
        : AstStatement { std::move(sourceRange) }
        , initScope { std::move(initScope) }
        , conditionalExpression { std::move(conditionalExpression) }
        , iterationExpression { std::move(iterationExpression) }
        , bodyScope { std::move(bodyScope) }
    {
    }

    void Visit(AstVisitor& visitor) override
    {
        visitor.VisitAstForLoopStatement(*this);
    }
};

}