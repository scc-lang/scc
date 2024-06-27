module;

#include <memory>

export module scc.ast:ast_expression_statement;
import :ast_expression;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct AstExpressionStatement final : AstStatement {
    std::unique_ptr<AstExpression> expression {};

    AstExpressionStatement(SourceRange sourceRange, std::unique_ptr<AstExpression> expression)
        : AstStatement { std::move(sourceRange) }
        , expression { std::move(expression) }
    {
    }

    void Visit(AstVisitor& visitor) override
    {
        visitor.VisitAstExpressionStatement(*this);
    }
};

}