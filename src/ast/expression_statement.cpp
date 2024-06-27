module;

#include <memory>

export module scc.ast:ast_expression_statement;
import :ast_expression;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct ExpressionStatement final : Statement {
    std::unique_ptr<Expression> expression {};

    ExpressionStatement(SourceRange sourceRange, std::unique_ptr<Expression> expression)
        : Statement { std::move(sourceRange) }
        , expression { std::move(expression) }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstExpressionStatement(*this);
    }
};

}