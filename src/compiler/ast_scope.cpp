module;

#include <memory>
#include <vector>

export module scc.compiler:ast_scope;

namespace scc::compiler {

export struct AstStatement;

export struct AstScope final {
    std::vector<std::unique_ptr<AstStatement>> statements {};
    std::weak_ptr<AstScope> parentScope {};
};

}