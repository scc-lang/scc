module;

#include <memory>

export module scc.ast:return_statement;
import :ast_expression;
import :source_range;
import :ast_statement;
import :ast_visitor;

namespace scc::ast {

export struct ReturnStatement final : Statement {
    std::unique_ptr<Expression> expression {};

    ReturnStatement(SourceRange sourceRange, std::unique_ptr<Expression> expression)
        : Statement { std::move(sourceRange) }
        , expression { std::move(expression) }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitReturnStatement(*this);
    }
};

}