module;

#include <cassert>
#include <memory>

export module scc.ast:ast_binary_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export enum class BinaryOp {
    Assignment,
    MulAssignment,
    DivAssignment,
    ModAssignment,
    AddAssignment,
    SubAssignment,
    ShiftLeftAssignment,
    ShiftRightAssignment,
    BitAndAssignment,
    BitXorAssignment,
    BitOrAssignment,

    Mul,
    Div,
    Mod,
    Add,
    Sub,

    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
};

export struct BinaryExpression final : Expression {
    std::unique_ptr<Expression> leftOprand {};
    BinaryOp op {};
    std::unique_ptr<Expression> rightOprand {};

    BinaryExpression(SourceRange sourceRange, std::unique_ptr<Expression> leftOprand, BinaryOp op, std::unique_ptr<Expression> rightOperand)
        : Expression { std::move(sourceRange) }
        , leftOprand { std::move(leftOprand) }
        , op { op }
        , rightOprand { std::move(rightOperand) }
    {
        assert(this->leftOprand);
        assert(this->rightOprand);
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstBinaryExpression(*this);
    }
};

}