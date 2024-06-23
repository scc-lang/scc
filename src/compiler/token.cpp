module;

#include <any>
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