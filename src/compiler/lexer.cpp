module;

#include <cassert>
#include <deque>
#include <istream>
#include <memory>

import scc.ast;

export module scc.compiler:lexer;
import :exception;
import :token;

namespace scc::compiler {

export struct Lexer final {
    Lexer(std::shared_ptr<std::istream> in)
        : m_in { std::move(in) }
    {
        assert(m_in);
    }

    Token GetToken()
    {
        if (m_tokens.empty()) {
            return ReadTokenFromInput();
        } else {
            auto token = std::move(m_tokens.front());
            m_tokens.pop_front();
            return std::move(token);
        }
    }

    Token GetRequiredToken(int tokenType)
    {
        auto token = GetToken();
        if (token.type != tokenType) {
            if (tokenType == TOKEN_IDENTIFIER) {
                throw Exception(token.sourceRange, "expected unqualified-id");
            } else {
                throw Exception(token.sourceRange, "expected '{}'", (TokenType)tokenType);
            }
        }
        return std::move(token);
    }

    const Token& PeekToken()
    {
        if (m_tokens.empty()) {
            m_tokens.push_back(ReadTokenFromInput());
        }
        return m_tokens.front();
    }

    void PutbackToken(Token token)
    {
        m_tokens.push_front(std::move(token));
    }

private:
    static_assert(TOKEN_EOF == std::istream::traits_type::eof());

    Token ReadTokenFromInput()
    {
        for (auto ch = PeekChar(); ch != TOKEN_EOF; ch = PeekChar()) {
            if (isspace(ch)) {
                GetChar();
            } else if (isalpha(ch) || ch == '_') {
                return ReadIdentifier();
            } else if (isdigit(ch)) {
                return ReadInteger();
            } else {
                switch (ch) {
                case '#':
                    ReadSingleLineComment();
                    continue;

                case '"':
                    return ReadString();

                case ':':
                    GetChar();
                    if (PeekChar() == ':') {
                        GetChar();
                        return Token { TOKEN_SCOPE, m_line, m_column - 2, m_column - 1 };
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '<':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_LESS_EQUAL, m_line, m_column - 2, m_column - 1 };
                    } else if (PeekChar() == '<') {
                        GetChar();
                        if (PeekChar() == '=') {
                            GetChar();
                            return Token { TOKEN_SHIFT_LEFT_ASSIGNMENT, m_line, m_column - 3, m_column - 1 };
                        } else {
                            return Token { TOKEN_SHIFT_LEFT, m_line, m_column - 2, m_column - 1 };
                        }
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '>':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_GREATER_EQUAL, m_line, m_column - 2, m_column - 1 };
                    } else if (PeekChar() == '>') {
                        GetChar();
                        if (PeekChar() == '=') {
                            GetChar();
                            return Token { TOKEN_SHIFT_RIGHT_ASSIGNMENT, m_line, m_column - 3, m_column - 1 };
                        } else {
                            return Token { TOKEN_SHIFT_RIGHT, m_line, m_column - 2, m_column - 1 };
                        }
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '*':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_MUL_ASSIGNMENT, m_line, m_column - 2, m_column - 1 };
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '/':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_DIV_ASSIGNMENT, m_line, m_column - 2, m_column - 1 };
                    } else if (PeekChar() == '/') {
                        ReadSingleLineComment();
                        continue;
                    } else if (PeekChar() == '*') {
                        ReadMultipleLinesComment();
                        continue;
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '%':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_MOD_ASSIGNMENT, m_line, m_column - 2, m_column - 1 };
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '+':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_ADD_ASSIGNMENT, m_line, m_column - 2, m_column - 1 };
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '-':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_SUB_ASSIGNMENT, m_line, m_column - 2, m_column - 1 };
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '&':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_BIT_AND_ASSIGNMENT, m_line, m_column - 2, m_column - 1 };
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '^':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_BIT_XOR_ASSIGNMENT, m_line, m_column - 2, m_column - 1 };
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '|':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_BIT_OR_ASSIGNMENT, m_line, m_column - 2, m_column - 1 };
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '=':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_EQUAL, m_line, m_column - 2, m_column - 1 };
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '!':
                    GetChar();
                    if (PeekChar() == '=') {
                        GetChar();
                        return Token { TOKEN_NOT_EQUAL, m_line, m_column - 2, m_column - 1 };
                    } else {
                        return Token { ch, m_line, m_column - 1 };
                    }

                case '(':
                case ')':
                case '{':
                case '}':
                case ';':
                case ',':
                    GetChar();
                    return Token { ch, m_line, m_column - 1 };

                default:
                    throw Exception { m_line, m_column, "unexpected input" };
                }
            }
        }
        return Token { GetChar(), m_line, m_column };
    }

    void ReadSingleLineComment()
    {
        assert(PeekChar() == '#' || PeekChar() == '/');
        if (PeekChar() == '#') {
            GetChar();
            // If the next character is not white character or other supported characters, throw error.
            if (auto ch = PeekChar(); ch != TOKEN_EOF && !std::isspace((unsigned char)ch)) {
                if (ch != '!') {
                    throw Exception { m_line, m_column, "'#' comment must be followed by a whitespace character" };
                }
            }
        }

        for (auto ch = GetChar(); ch != TOKEN_EOF && ch != '\n'; ch = GetChar())
            ;
    }

    void ReadMultipleLinesComment()
    {
        assert(PeekChar() == '*');
        GetChar();

        auto startLine = m_line;
        auto startColumn = m_column - 2;
        auto flagCount = int { 1 };
        auto ch = GetChar();
        for (; ch != TOKEN_EOF; ch = GetChar()) {
            if (ch == '/' && PeekChar() == '*') {
                GetChar();
                ++flagCount;
            } else if (ch == '*' && PeekChar() == '/') {
                GetChar();
                if (!--flagCount) {
                    break;
                }
            }
        }
        if (ch == TOKEN_EOF) {
            throw Exception(startLine, startColumn, m_line, m_column, "unterminated /* comment");
        }
    }

    Token ReadIdentifier()
    {
        int startLine = m_line;
        int startColumn = m_column;

        std::string str { (char)GetChar() };
        assert(isalpha(str[0]) || str[0] == '_');
        for (auto ch = PeekChar(); isalnum(ch) || ch == '_'; ch = PeekChar()) {
            str += (char)GetChar();
        }

        if (str == "for") {
            return Token { TOKEN_FOR, startLine, startColumn, m_column - 1 };
        } else if (str == "if") {
            return Token { TOKEN_IF, startLine, startColumn, m_column - 1 };
        } else if (str == "else") {
            return Token { TOKEN_ELSE, startLine, startColumn, m_column - 1 };
        } else if (str == "return") {
            return Token { TOKEN_RETURN, startLine, startColumn, m_column - 1 };
        } else {
            return Token { TOKEN_IDENTIFIER, startLine, startColumn, m_line, m_column - 1, std::move(str) };
        }
    }

    Token ReadString()
    {
        int startLine = m_line;
        int startColumn = m_column;

        assert(PeekChar() == '"');
        GetChar();

        std::string str;
        auto ch = PeekChar();
        for (; ch != '"' && ch != '\n' && ch != TOKEN_EOF; ch = PeekChar()) {
            if (ch == '\\') {
                str += GetEscapeChar();
            } else {
                str += GetChar();
            }
        }
        if (ch == TOKEN_EOF || ch == '\n') {
            throw Exception { m_line, m_column, "missing terminating '\"' character" };
        } else {
            assert(PeekChar() == '"');
            GetChar();
            return Token { TOKEN_STRING, startLine, startColumn, m_line, m_column - 1, std::move(str) };
        }
    }

    Token ReadInteger()
    {
        assert(std::isdigit(PeekChar()));

        int startLine = m_line;
        int startColumn = m_column;

        uint64_t v {};
        for (auto ch = PeekChar(); std::isdigit(ch); ch = PeekChar()) {
            v *= 10;
            v += GetChar() - '0';
            // TODO: handle overflow
        }
        return Token { TOKEN_INTEGER, startLine, startColumn, m_line, m_column - 1, v };
    }

    int PeekChar()
    {
        return m_in->peek();
    }

    int GetChar()
    {
        auto ch = m_in->get();
        if (ch == '\n') {
            m_column = 1;
            ++m_line;
        } else if (ch != TOKEN_EOF) {
            ++m_column;
        }
        return ch;
    }

    // https://en.cppreference.com/w/cpp/language/escape
    int GetEscapeChar()
    {
        int startLine = m_line;
        int startColumn = m_column;

        assert(PeekChar() == '\\');
        GetChar();

        auto ch = GetChar();
        if (ch == TOKEN_EOF) {
            throw Exception { m_line, m_column, "missing terminating escape sequence" };
        } else if (ch >= '0' && ch <= '7') {
            return ReadOctalEscapeSequence(startLine, startColumn, ch);
        } else if (ch == 'x') {
            return ReadHexEscapeSequence(startLine, startColumn);
        } else {
            switch (ch) {
            case '\'':
                return 0x27;

            case '"':
                return 0x22;

            case '?':
                return 0x3f;

            case '\\':
                return 0x5c;

            case 'a':
                return 0x07;

            case 'b':
                return 0x08;

            case 'f':
                return 0x0c;

            case 'n':
                return 0x0a;

            case 'r':
                return 0x0d;

            case 't':
                return 0x09;

            case 'v':
                return 0x0b;

            default:
                throw Exception { startLine, startColumn, m_line, m_column - 1, "Unknown missing terminating escape sequence" };
            }
        }
    }

    int ReadOctalEscapeSequence(int startLine, int startColumn, int ch)
    {
        assert(ch >= '0' && ch <= '7');

        int v { ch - '0' };
        for (int i = 0, ch = PeekChar(); i < 2 && ch >= '0' && ch <= '7'; ++i, ch = PeekChar()) {
            v *= 8;
            v += GetChar() - '0';
            if (v > 255) {
                throw Exception { startLine, startColumn, m_column - 1, "octal escape sequence out of range" };
            }
        }
        assert(v <= 255);
        return v;
    }

    int ReadHexEscapeSequence(int startLine, int startColumn)
    {
        if (!std::isxdigit(PeekChar())) {
            throw Exception { startLine, startColumn, m_line, m_column - 1, "\\x used with no following hex digits" };
        }

        int v { 0 };
        for (auto ch = PeekChar(); std::isxdigit(ch); ch = PeekChar()) {
            ch = GetChar();
            v *= 16;
            v += std::isdigit(ch) ? ch - '0' : std::tolower(ch) - 'a' + 10;
            if (v > 255) {
                throw Exception { startLine, startColumn, m_column - 1, "hex escape sequence out of range" };
            }
        }
        assert(v <= 255);
        return v;
    }

    std::shared_ptr<std::istream> m_in {};
    int m_line { 1 };
    int m_column { 1 };
    std::deque<Token> m_tokens {};
};

}