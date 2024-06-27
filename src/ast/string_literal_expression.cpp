module;

#include <string>

export module scc.ast:ast_string_literal_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct StringLiteralExpression final : Expression {
    std::string value {};

    StringLiteralExpression(SourceRange sourceRanage, std::string value)
        : Expression { std::move(sourceRanage) }
        , value { std::move(value) }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstStringLiteralExpression(*this);
    }
};

}