module;

#include <string>

export module scc.ast:ast_identifier_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct IdentifierExpression : Expression {
    std::string fullName {};

    IdentifierExpression(SourceRange sourceRange, std::string fullName)
        : Expression(std::move(sourceRange))
        , fullName { std::move(fullName) }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstIdentifierExpression(*this);
    }
};

}