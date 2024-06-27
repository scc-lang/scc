module;

#include <utility>

export module scc.ast:ast_break_statement;
import :ast_statement;
import :ast_visitor;
import :source_range;

namespace scc::ast {

export struct BreakStatement : Statement {
    BreakStatement(SourceRange sourceRange)
        : Statement { std::move(sourceRange) }
    {
    }

    void Visit(Visitor& visitor) override
    {
        visitor.VisitAstBreakStatement(*this);
    }
};

}