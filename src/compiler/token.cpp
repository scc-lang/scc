module;

#include <any>
#include <cassert>
#include <cstdint>
#include <format>
#include <string>

export module scc.compiler:token;
import :source_range;

namespace scc::compiler {

export enum TokenType {
    TOKEN_EOF = -1,
    TOKEN_EMPTY = 256,
    TOKEN_IDENTIFIER,
    TOKEN_SCOPE,
    TOKEN_STRING,
    TOKEN_INTEGER,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,
    TOKEN_SHIFT_LEFT,
    TOKEN_SHIFT_RIGHT,
    TOKEN_MUL_ASSIGNMENT,
    TOKEN_DIV_ASSIGNMENT,
    TOKEN_MOD_ASSIGNMENT,
    TOKEN_ADD_ASSIGNMENT,
    TOKEN_SUB_ASSIGNMENT,
    TOKEN_SHIFT_LEFT_ASSIGNMENT,
    TOKEN_SHIFT_RIGHT_ASSIGNMENT,
    TOKEN_BIT_AND_ASSIGNMENT,
    TOKEN_BIT_XOR_ASSIGNMENT,
    TOKEN_BIT_OR_ASSIGNMENT,
    TOKEN_FOR,
};

export struct Token final {
    int type {};
    SourceRange sourceRange;
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
        : Token { type, SourceRange { startLine, startColumn, endLine, endColumn }, std::move(value) }
    {
    }

    Token(int type, SourceRange sourceRange, std::any value)
        : type { type }
        , sourceRange { std::move(sourceRange) }
        , value { std::move(value) }
    {
    }

    std::string& string()
    {
        return *std::any_cast<std::string>(&value);
    }

    const std::string& string() const
    {
        return *std::any_cast<std::string>(&value);
    }

    uint64_t integer() const
    {
        return *std::any_cast<uint64_t>(&value);
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