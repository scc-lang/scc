module;

#include <string>

export module scc.ast:ast_type_info;

namespace scc::ast {

export struct AstTypeInfo final {
    std::string fullName {};

    explicit AstTypeInfo(std::string fullName)
        : fullName { std::move(fullName) }
    {
    }
};

}