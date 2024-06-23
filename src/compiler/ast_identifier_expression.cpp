module;

#include <string>

export module scc.compiler:ast_identifier_expression;
import :ast_expression;
import :source_range;

namespace scc::compiler {

export struct AstIdentifierExpression : AstExpression {
    std::string fullName {};

    AstIdentifierExpression(SourceRange sourceRange, std::string fullName)
        : AstExpression(std::move(sourceRange))
        , fullName { std::move(fullName) }
    {
    }
};

}