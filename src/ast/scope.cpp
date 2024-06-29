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
            m_types.emplace("void", TypeInfo { "void" });
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

    void AddFunction(std::string name, std::unique_ptr<Statement> func)
    {
        m_functions.emplace(std::move(name), std::move(func));
    }

    Statement* QueryFunction(const std::string& funcName) const
    {
        auto it = m_functions.find(funcName);
        if (it == m_functions.end()) {
            return parentScope ? parentScope->QueryFunction(funcName) : nullptr;
        } else {
            return it->second.get();
        }
    }

    std::vector<Statement*> GetFunctions() const
    {
        std::vector<Statement*> functions {};
        for (auto it = m_functions.begin(); it != m_functions.end(); ++it) {
            functions.push_back(it->second.get());
        }
        return std::move(functions);
    }

private:
    std::unordered_map<std::string, TypeInfo> m_types {};
    std::unordered_map<std::string, std::unique_ptr<Statement>> m_functions {};
};

}