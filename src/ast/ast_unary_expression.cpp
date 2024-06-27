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

export struct AstUnaryExpression final : AstExpression {
    UnaryOp op {};
    std::unique_ptr<AstExpression> oprand {};

    AstUnaryExpression(SourceRange sourceRange, UnaryOp op, std::unique_ptr<AstExpression> oprand)
        : AstExpression { std::move(sourceRange) }
        , op { op }
        , oprand { std::move(oprand) }
    {
        assert(this->oprand);
    }

    void Visit(AstVisitor& visitor) override
    {
        visitor.VisitAstUnaryExpression(*this);
    }
};

}