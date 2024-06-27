module;

#include <memory>
#include <unordered_map>
#include <vector>

export module scc.ast:ast_scope;
import :ast_type_info;
import :ast_statement;
import :ast_variable_declaration;

namespace scc::ast {

export struct Scope final {
    std::vector<std::unique_ptr<Statement>> statements {};
    std::vector<std::unique_ptr<VariableDeclaration>> variableDeclarations {};

    Scope* parentScope {};

    Scope(Scope* parentScope = nullptr)
        : parentScope { parentScope }
    {
        if (!parentScope) {
            // For global scope, add builtin type.
            // TODO
            m_types.emplace("int", TypeInfo { "int" });
        }
    }

    TypeInfo* QueryTypeInfo(const std::string& symbol)
    {
        auto it = m_types.find(symbol);
        if (it == m_types.end()) {
            return parentScope ? parentScope->QueryTypeInfo(symbol) : nullptr;
        } else {
            return &it->second;
        }
    }

private:
    std::unordered_map<std::string, TypeInfo> m_types {};
};

}