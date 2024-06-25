module;

#include <utility>

export module scc.compiler:ast_variable_definition_statement;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::compiler {

export struct AstVariableDeclaration;

export struct AstVariableDefinitionStatement final : AstStatement {
    AstVariableDeclaration& variableDeclaration;

    AstVariableDefinitionStatement(SourceRange sourceRange, AstVariableDeclaration& variableDeclaration)
        : AstStatement { std::move(sourceRange) }
        , variableDeclaration { variableDeclaration }
    {
    }

    void Visit(AstVisitor& visitor) override {
        visitor.VisitAstVariableDefinitionStatement(*this);
    }
};

}