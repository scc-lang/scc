module;

#include <string>

export module scc.compiler:ast_identifier_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::compiler {

export struct AstIdentifierExpression : AstExpression {
    std::string fullName {};

    AstIdentifierExpression(SourceRange sourceRange, std::string fullName)
        : AstExpression(std::move(sourceRange))
        , fullName { std::move(fullName) }
    {
    }

    void Visit(AstVisitor& visitor) override {
        visitor.VisitAstIdentifierExpression(*this);
    }
};

}