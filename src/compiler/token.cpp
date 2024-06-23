module;

#include <any>
#include <cassert>
#include <format>
#include <string>

export module scc.compiler:token;

namespace scc::compiler {

export enum TokenType {
    TOKEN_EOF = -1,
    TOKEN_EMPTY = 256,
    TOKEN_IDENTIFIER,
    TOKEN_SCOPE,
    TOKEN_STRING,
};

export struct Token final {
    int type {};
    int startLine {};
    int startColumn {};
    int endLine {};
    int endColumn {};
    std::any value {};

    Token(int type, int startLine, int startColumn)
        : Token { type, startLine, startColumn, startLine, startColumn, {} }
    {
    }

    Token(int type, int startLine, int startColumn, int endColumn)
        : Token { type, startLine, startColumn, startLine, endColumn, {} }
    {
    }

    Token(int type, int startLine, int startColumn, int endLine, int endColumn, std::any value)
        : type { type }
        , startLine { startLine }
        , startColumn { startColumn }
        , endLine { endLine }
        , endColumn { endColumn }
        , value { std::move(value) }
    {
    }

    std::string& string()
    {
        return *std::any_cast<std::string>(&value);
    }
};

}

export template <>
struct std::formatter<scc::compiler::TokenType> {
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    auto format(scc::compiler::TokenType type, std::format_context& ctx) const
    {
        if (type < 256) {
            return std::format_to(ctx.out(), "{}", (char)type);
        }

        switch (type) {
        case scc::compiler::TOKEN_IDENTIFIER:
            return std::format_to(ctx.out(), "IDENTIFIER");

        case scc::compiler::TOKEN_SCOPE:
            return std::format_to(ctx.out(), "::");

        case scc::compiler::TOKEN_STRING:
            return std::format_to(ctx.out(), "STRING");

        default:
            assert(false);
            return std::format_to(ctx.out(), "(TokenType: {})", type);
        }
    }
};