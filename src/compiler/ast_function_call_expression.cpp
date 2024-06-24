module;

#include <memory>
#include <vector>

export module scc.compiler:ast_function_call_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::compiler {

export struct AstFunctionCallExpression final : AstExpression {
    std::unique_ptr<AstExpression> funcExpression;
    std::vector<std::unique_ptr<AstExpression>> argsExpression;

    AstFunctionCallExpression(SourceRange sourceRange, std::unique_ptr<AstExpression> funcExpression, std::vector<std::unique_ptr<AstExpression>> argsExpression)
        : AstExpression { std::move(sourceRange) }
        , funcExpression { std::move(funcExpression) }
        , argsExpression { std::move(argsExpression) }
    {
    }

    void Visit(AstVisitor& visitor) override {
        visitor.VisitAstFunctionCallExpression(*this);
    }
};

}