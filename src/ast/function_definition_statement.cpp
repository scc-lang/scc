module;

#include <string>

export module scc.ast:function_definition_statement;
import :ast_statement;
import :ast_scope;
import :ast_type_info;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct FunctionDefinitionStatement final : Statement {
    TypeInfo& typeInfo;
    std::string name {};
    Scope headerScope {};
    Scope bodyScope {};

    FunctionDefinitionStatement(SourceRange sourceRange, TypeInfo& typeInfo, std::string name, Scope headerScope, Scope bodyScope)
        : Statement { std::move(sourceRange) }
        , typeInfo { typeInfo }
        , name { std::move(name) }
        , headerScope { std::move(headerScope) }
        , bodyScope { std::move(bodyScope) }
    {
    }

    void Visit(Visitor& visitor) override {
        visitor.VisitFunctionDefinitionStatement(*this);
    }
};

}