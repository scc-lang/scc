module;

#include <cassert>
#include <memory>

export module scc.ast:ast_unary_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export enum class UnaryOp {
    Bracket,
};

export struct UnaryExpression final : Expression {
    UnaryOp op {};
    std::unique_ptr<Expression> oprand {};

    UnaryExpression(SourceRange sourceRange, UnaryOp op, std::unique_ptr<Expression> oprand)
        : Expression { std::move(sourceRange) }
        , op { op }
        , oprand { std::move(oprand) }
    {
        assert(this->oprand);
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstUnaryExpression(*this);
    }
};

}