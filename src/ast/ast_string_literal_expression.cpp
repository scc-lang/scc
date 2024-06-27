module;

#include <string>

export module scc.ast:ast_string_literal_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct AstStringLiteralExpression final : AstExpression {
    std::string value {};

    AstStringLiteralExpression(SourceRange sourceRanage, std::string value)
        : AstExpression { std::move(sourceRanage) }
        , value { std::move(value) }
    {
    }

    void Visit(AstVisitor& visitor) override
    {
        visitor.VisitAstStringLiteralExpression(*this);
    }
};

}