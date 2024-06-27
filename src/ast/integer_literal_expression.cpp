module;

#include <cstdint>
#include <utility>

export module scc.ast:ast_integer_literal_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct IntegerLiteralExpression final : Expression {
    uint64_t value {};

    IntegerLiteralExpression(SourceRange sourceRanage, uint64_t value)
        : Expression { std::move(sourceRanage) }
        , value { value }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstIntegerLiteralExpression(*this);
    }
};

}