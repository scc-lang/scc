module;

#include <format>
#include <stdexcept>

export module scc.compiler:exception;

namespace scc::compiler {

export struct Exception : std::runtime_error {
    int startLine {};
    int startColumn {};
    int endLine {};
    int endColumn {};

    template <typename... Args>
    Exception(int startLine, int startColumn, const std::string_view& message, Args&&... args)
        : Exception(startLine, startColumn, startLine, startColumn, message, std::forward<Args>(args)...)
    {
    }

    template <typename... Args>
    Exception(int startLine, int startColumn, int endColumn, const std::string_view& message, Args&&... args)
        : Exception(startLine, startColumn, startLine, endColumn, message, std::forward<Args>(args)...)
    {
    }

    template <typename... Args>
    Exception(int startLine, int startColumn, int endLine, int endColumn, const std::string_view& message, Args&&... args)
        : std::runtime_error { std::vformat(message, std::make_format_args(args...)) }
        , startLine { startLine }
        , startColumn { startColumn }
        , endLine { endLine }
        , endColumn { endColumn }
    {
    }
};

}