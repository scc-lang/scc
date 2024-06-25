module;

#include <memory>

export module scc.compiler:ast_conditional_statement;
import :ast_expression;
import :ast_scope;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::compiler {

export struct AstConditionalStatement final : AstStatement {
    std::unique_ptr<AstExpression> conditionalExpression {};
    AstScope trueScope {};
    AstScope falseScope {};

    AstConditionalStatement(SourceRange sourceRange, std::unique_ptr<AstExpression> conditionalExpression, AstScope trueScope, AstScope falseScope)
        : AstStatement { std::move(sourceRange) }
        , conditionalExpression { std::move(conditionalExpression) }
        , trueScope { std::move(trueScope) }
        , falseScope { std::move(falseScope) }
    {
    }

    void Visit(AstVisitor& visitor) override
    {
        visitor.VisitAstConditionalStatement(*this);
    }
};

}