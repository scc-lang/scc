module;

#include <memory>
#include <vector>

export module scc.compiler:ast_conditional_statement;
import :ast_expression;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::compiler {

export struct AstConditionalStatement final : AstStatement {
    std::unique_ptr<AstExpression> conditionalExpression {};
    std::vector<std::unique_ptr<AstStatement>> trueStatements {};
    std::vector<std::unique_ptr<AstStatement>> falseStatements {};

    AstConditionalStatement(SourceRange sourceRange, std::unique_ptr<AstExpression> conditionalExpression,
        std::vector<std::unique_ptr<AstStatement>> trueStatements,
        std::vector<std::unique_ptr<AstStatement>> falseStatements)
        : AstStatement { std::move(sourceRange) }
        , conditionalExpression { std::move(conditionalExpression) }
        , trueStatements { std::move(trueStatements) }
        , falseStatements { std::move(falseStatements) }
    {
    }

    void Visit(AstVisitor& visitor) override
    {
        visitor.VisitAstConditionalStatement(*this);
    }
};

}