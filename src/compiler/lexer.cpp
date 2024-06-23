module;

#include <cassert>
#include <istream>
#include <memory>
#include <queue>

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
            m_tokens.pop();
            return std::move(token);
        }
    }

    Token GetRequiredToken(int tokenType)
    {
        auto token = GetToken();
        if (token.type != tokenType) {
            throw Exception(token.startLine, token.startColumn, token.endLine, token.endColumn, "expected '{}'", (TokenType)tokenType);
        }
        return std::move(token);
    }

    const Token& PeekToken()
    {
        if (m_tokens.empty()) {
            m_tokens.emplace(ReadTokenFromInput());
        }
        return m_tokens.front();
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

                case '(':
                case ')':
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
        assert(PeekChar() == '#');
        for (auto ch = GetChar(); ch != TOKEN_EOF && ch != '\n'; ch = GetChar())
            ;
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
        return Token { TOKEN_IDENTIFIER, startLine, startColumn, m_line, m_column - 1, std::move(str) };
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
    std::queue<Token> m_tokens {};
};

}