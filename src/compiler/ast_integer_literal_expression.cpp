module;

#include <cstdint>
#include <utility>

export module scc.compiler:ast_integer_literal_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::compiler {

export struct AstIntegerLiteralExpression final : AstExpression {
    uint64_t value {};

    AstIntegerLiteralExpression(SourceRange sourceRanage, uint64_t value)
        : AstExpression { std::move(sourceRanage) }
        , value { value }
    {
    }

    void Visit(AstVisitor& visitor) override {
        visitor.VisitAstIntegerLiteralExpression(*this);
    }
};

}