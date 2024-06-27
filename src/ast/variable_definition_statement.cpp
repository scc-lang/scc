module;

#include <utility>

export module scc.ast:ast_variable_definition_statement;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct VariableDeclaration;

export struct VariableDefinitionStatement final : Statement {
    VariableDeclaration& variableDeclaration;

    VariableDefinitionStatement(SourceRange sourceRange, VariableDeclaration& variableDeclaration)
        : Statement { std::move(sourceRange) }
        , variableDeclaration { variableDeclaration }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstVariableDefinitionStatement(*this);
    }
};

}