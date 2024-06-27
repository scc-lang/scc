module;

#include <string>

export module scc.ast:ast_type_info;

namespace scc::ast {

export struct TypeInfo final {
    std::string fullName {};

    explicit TypeInfo(std::string fullName)
        : fullName { std::move(fullName) }
    {
    }
};

}