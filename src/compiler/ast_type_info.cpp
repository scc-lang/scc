module;

#include <string>

export module scc.compiler:ast_type_info;

namespace scc::compiler {

export struct AstTypeInfo final {
    std::string fullName {};

    explicit AstTypeInfo(std::string fullName)
        : fullName { std::move(fullName) }
    {
    }
};

}