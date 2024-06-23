module;

export module scc.compiler:source_range;

namespace scc::compiler {

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
};

}