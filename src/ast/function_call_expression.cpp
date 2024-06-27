module;

#include <memory>
#include <vector>

export module scc.ast:ast_function_call_expression;
import :ast_expression;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct FunctionCallExpression final : Expression {
    std::unique_ptr<Expression> funcExpression;
    std::vector<std::unique_ptr<Expression>> argsExpression;

    FunctionCallExpression(SourceRange sourceRange, std::unique_ptr<Expression> funcExpression, std::vector<std::unique_ptr<Expression>> argsExpression)
        : Expression { std::move(sourceRange) }
        , funcExpression { std::move(funcExpression) }
        , argsExpression { std::move(argsExpression) }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstFunctionCallExpression(*this);
    }
};

}