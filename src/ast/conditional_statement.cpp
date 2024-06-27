module;

#include <memory>

export module scc.ast:ast_conditional_statement;
import :ast_expression;
import :ast_scope;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct ConditionalStatement final : Statement {
    std::unique_ptr<Expression> conditionalExpression {};
    Scope trueScope {};
    Scope falseScope {};

    ConditionalStatement(SourceRange sourceRange, std::unique_ptr<Expression> conditionalExpression, Scope trueScope, Scope falseScope)
        : Statement { std::move(sourceRange) }
        , conditionalExpression { std::move(conditionalExpression) }
        , trueScope { std::move(trueScope) }
        , falseScope { std::move(falseScope) }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstConditionalStatement(*this);
    }
};

}