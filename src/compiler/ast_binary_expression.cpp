module;

#include <cassert>
#include <memory>

export module scc.compiler:ast_binary_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::compiler {

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