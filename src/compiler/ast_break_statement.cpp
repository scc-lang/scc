module;

#include <utility>

export module scc.compiler:ast_break_statement;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::compiler {

export struct AstBreakStatement : AstStatement {
    AstBreakStatement(SourceRange sourceRange)
        : AstStatement { std::move(sourceRange)}
        {
        }

        void Visit(AstVisitor& visitor) override
    {
        visitor.VisitAstBreakStatement(*this);
    }
};

}