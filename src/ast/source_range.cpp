module;

export module scc.ast:source_range;

namespace scc::ast {

export struct SourceRange {
    int startLine {};
    int startColumn {};
    int endLine {};
    int endColumn {};

    SourceRange(int startLine, int startColumn)
        : SourceRange { startLine, startColumn, startLine, startColumn }
    {
    }

    SourceRange(int startLine, int startColumn, int endLine, int endColumn)
        : startLine { startLine }
        , startColumn { startColumn }
        , endLine { endLine }
        , endColumn { endColumn }
    {
    }

    SourceRange(const SourceRange& start, const SourceRange& end)
        : startLine { start.startLine }
        , startColumn { start.startColumn }
        , endLine { end.endLine }
        , endColumn { end.endColumn }
    {
    }
};

}