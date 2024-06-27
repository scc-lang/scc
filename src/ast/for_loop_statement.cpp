module;

#include <memory>

export module scc.ast:ast_for_loop_statement;
import :ast_expression;
import :ast_scope;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct ForLoopStatement final : Statement {
    Scope initScope {};
    std::unique_ptr<Expression> conditionalExpression {};
    std::unique_ptr<Expression> iterationExpression {};
    Scope bodyScope {};

    ForLoopStatement(SourceRange sourceRange, Scope initScope, std::unique_ptr<Expression> conditionalExpression, std::unique_ptr<Expression> iterationExpression, Scope bodyScope)
        : Statement { std::move(sourceRange) }
        , initScope { std::move(initScope) }
        , conditionalExpression { std::move(conditionalExpression) }
        , iterationExpression { std::move(iterationExpression) }
        , bodyScope { std::move(bodyScope) }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstForLoopStatement(*this);
    }
};

}