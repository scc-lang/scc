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

export struct AstBinaryExpression final : AstExpression {
    std::unique_ptr<AstExpression> leftOprand {};
    BinaryOp op {};
    std::unique_ptr<AstExpression> rightOprand {};

    AstBinaryExpression(SourceRange sourceRange, std::unique_ptr<AstExpression> leftOprand, BinaryOp op, std::unique_ptr<AstExpression> rightOperand)
        : AstExpression { std::move(sourceRange) }
        , leftOprand { std::move(leftOprand) }
        , op { op }
        , rightOprand { std::move(rightOperand) }
    {
        assert(this->leftOprand);
        assert(this->rightOprand);
    }

    void Visit(AstVisitor& visitor) override
    {
        visitor.VisitAstBinaryExpression(*this);
    }
};

}