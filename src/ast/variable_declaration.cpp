module;

#include <memory>

export module scc.ast:ast_variable_declaration;
import :ast_expression;
import :ast_node;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct TypeInfo;

export struct VariableDeclaration : Node {
    TypeInfo& typeInfo;
    std::string name {};
    std::unique_ptr<Expression> initExpression {};

    VariableDeclaration(SourceRange sourceRange, TypeInfo& typeinfo, std::string name, std::unique_ptr<Expression> initExpression = nullptr)
        : Node { std::move(sourceRange) }
        , typeInfo { typeinfo }
        , name { std::move(name) }
        , initExpression { std::move(initExpression) }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstVariableDeclaration(*this);
    }
};

}