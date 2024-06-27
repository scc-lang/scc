module;

#include <memory>

export module scc.ast:ast_variable_declaration;
import :ast_expression;
import :ast_node;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct AstTypeInfo;

export struct AstVariableDeclaration : AstNode {
    AstTypeInfo& typeInfo;
    std::string name {};
    std::unique_ptr<AstExpression> initExpression {};

    AstVariableDeclaration(SourceRange sourceRange, AstTypeInfo& typeinfo, std::string name, std::unique_ptr<AstExpression> initExpression = nullptr)
        : AstNode { std::move(sourceRange) }
        , typeInfo { typeinfo }
        , name { std::move(name) }
        , initExpression { std::move(initExpression) }
    {
    }

    void Visit(AstVisitor& visitor) override
    {
        visitor.VisitAstVariableDeclaration(*this);
    }
};

}