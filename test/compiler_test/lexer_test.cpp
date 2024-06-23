#include "test/test.h"

import scc.compiler;

using namespace scc::compiler;

class LexerTest : public testing::Test {
protected:
    Lexer CreateLexer(std::string str)
    {
        return Lexer { std::make_shared<std::istringstream>(std::move(str)) };
    }
};

TEST_F(LexerTest, ParseEmptyContent)
{
    auto lexer = CreateLexer("");
    auto token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_EOF);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 1);
}

TEST_F(LexerTest, ParseSingleComment)
{
    auto lexer = CreateLexer("#");
    auto token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_EOF);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 2);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 2);

    lexer = CreateLexer("# 34567");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_EOF);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 8);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 8);

    lexer = CreateLexer("    # 789");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_EOF);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 10);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 10);

    lexer = CreateLexer(R"(    # 789
  # 567 9abc)");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_EOF);
    ASSERT_EQ(token.startLine, 2);
    ASSERT_EQ(token.startColumn, 13);
    ASSERT_EQ(token.endLine, 2);
    ASSERT_EQ(token.endColumn, 13);
}

TEST_F(LexerTest, ParseIdentifier)
{
    auto lexer = CreateLexer("a");
    auto token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 1);
    ASSERT_EQ(token.string(), "a");

    lexer = CreateLexer("abcd");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 4);
    ASSERT_EQ(token.string(), "abcd");

    lexer = CreateLexer("abcd efgh");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 4);
    ASSERT_EQ(token.string(), "abcd");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 6);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 9);
    ASSERT_EQ(token.string(), "efgh");

    lexer = CreateLexer(R"( _ _abc   # 789
  # 567 9abc
  int_12_456 abc_
  )");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 2);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 2);
    ASSERT_EQ(token.string(), "_");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 4);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 7);
    ASSERT_EQ(token.string(), "_abc");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 3);
    ASSERT_EQ(token.startColumn, 3);
    ASSERT_EQ(token.endLine, 3);
    ASSERT_EQ(token.endColumn, 12);
    ASSERT_EQ(token.string(), "int_12_456");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 3);
    ASSERT_EQ(token.startColumn, 14);
    ASSERT_EQ(token.endLine, 3);
    ASSERT_EQ(token.endColumn, 17);
    ASSERT_EQ(token.string(), "abc_");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_EOF);
    ASSERT_EQ(token.startLine, 4);
    ASSERT_EQ(token.startColumn, 3);
    ASSERT_EQ(token.endLine, 4);
    ASSERT_EQ(token.endColumn, 3);
}

TEST_F(LexerTest, ParseScope)
{
    auto lexer = CreateLexer("ab::cd::ef");
    auto token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 2);
    ASSERT_EQ(token.string(), "ab");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_SCOPE);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 3);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 4);

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 5);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 6);
    ASSERT_EQ(token.string(), "cd");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_SCOPE);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 7);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 8);

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 9);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 10);
    ASSERT_EQ(token.string(), "ef");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_EOF);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 11);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 11);
}

TEST_F(LexerTest, ParsePunctuationChar)
{
    auto lexer = CreateLexer("a:b (d);");
    auto token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 1);
    ASSERT_EQ(token.string(), "a");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, ':');
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 2);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 2);

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 3);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 3);
    ASSERT_EQ(token.string(), "b");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, '(');
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 5);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 5);

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 6);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 6);
    ASSERT_EQ(token.string(), "d");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, ')');
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 7);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 7);

    token = lexer.GetToken();
    ASSERT_EQ(token.type, ';');
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 8);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 8);

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_EOF);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 9);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 9);
}

TEST_F(LexerTest, UnexpectedInput)
{
    ASSERT_THROW_COMPILER_EXCEPTION(CreateLexer("@").GetToken(), (Exception { 1, 1, "unexpected input" }));

    auto lexer = CreateLexer(R"( abc
  @)");
    auto token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 2);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 4);
    ASSERT_EQ(token.string(), "abc");
    ASSERT_THROW_COMPILER_EXCEPTION(lexer.GetToken(), (Exception { 2, 3, "unexpected input" }));
}

TEST_F(LexerTest, ParseString)
{
    auto lexer = CreateLexer("\"abc def\"");
    auto token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 9);
    ASSERT_EQ(token.string(), "abc def");

    lexer = CreateLexer("ab \"abc 1223 !@#213 $~*&< > ()[;,:] def\"");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 2);
    ASSERT_EQ(token.string(), "ab");

    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 4);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 40);
    ASSERT_EQ(token.string(), "abc 1223 !@#213 $~*&< > ()[;,:] def");

    lexer = CreateLexer(R"("\'\"\?\\\a\b\f\n\r\t\v")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 24);
    ASSERT_EQ(token.string(), "\'\"\?\\\a\b\f\n\r\t\v");

    lexer = CreateLexer("\"abc");
    ASSERT_THROW_COMPILER_EXCEPTION(lexer.GetToken(), (Exception { 1, 5, "missing terminating '\"' character" }));

    lexer = CreateLexer("\"ab\nc\"");
    ASSERT_THROW_COMPILER_EXCEPTION(lexer.GetToken(), (Exception { 1, 4, "missing terminating '\"' character" }));

    lexer = CreateLexer(R"("abc\)");
    ASSERT_THROW_COMPILER_EXCEPTION(lexer.GetToken(), (Exception { 1, 6, "missing terminating escape sequence" }));

    lexer = CreateLexer("std::println(\"Hello world!\");");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_SCOPE);
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_IDENTIFIER);
    token = lexer.GetToken();
    ASSERT_EQ(token.type, '(');
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.string(), "Hello world!");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, ')');
    token = lexer.GetToken();
    ASSERT_EQ(token.type, ';');
}

TEST_F(LexerTest, ParseOctalEscapeSequence)
{
    auto lexer = CreateLexer(R"("\1")");
    auto token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 4);
    ASSERT_EQ(token.string(), "\1");

    lexer = CreateLexer(R"("\12")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 5);
    ASSERT_EQ(token.string(), "\12");

    lexer = CreateLexer(R"("\123")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 6);
    ASSERT_EQ(token.string(), "\123");

    lexer = CreateLexer(R"("\1234")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 7);
    ASSERT_EQ(token.string().length(), 2);
    ASSERT_EQ(token.string(), "\1234");

    lexer = CreateLexer(R"("a\377")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 7);
    ASSERT_EQ(token.string().length(), 2);
    ASSERT_EQ(token.string(), "a\377");

    lexer = CreateLexer(R"("\18")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 5);
    ASSERT_EQ(token.string().length(), 2);
    ASSERT_EQ(token.string(), "\18");

    lexer = CreateLexer(R"("\128")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 6);
    ASSERT_EQ(token.string().length(), 2);
    ASSERT_EQ(token.string(), "\128");

    lexer = CreateLexer(R"("\1238")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 7);
    ASSERT_EQ(token.string().length(), 2);
    ASSERT_EQ(token.string(), "\1238");

    lexer = CreateLexer(R"("\0")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 4);
    ASSERT_EQ(token.string().length(), 1);
    ASSERT_EQ(token.string()[0], '\0');

    lexer = CreateLexer(R"("a\400")");
    ASSERT_THROW_COMPILER_EXCEPTION(lexer.GetToken(), (Exception { 1, 3, 6, "octal escape sequence out of range" }));
}

TEST_F(LexerTest, ParseHexEscapeSequence)
{
    auto lexer = CreateLexer(R"("\x)");
    ASSERT_THROW_COMPILER_EXCEPTION(lexer.GetToken(), (Exception { 1, 2, 3, "\\x used with no following hex digits" }));

    lexer = CreateLexer(R"("\x")");
    ASSERT_THROW_COMPILER_EXCEPTION(lexer.GetToken(), (Exception { 1, 2, 3, "\\x used with no following hex digits" }));

    lexer = CreateLexer(R"("\xG")");
    ASSERT_THROW_COMPILER_EXCEPTION(lexer.GetToken(), (Exception { 1, 2, 3, "\\x used with no following hex digits" }));

    lexer = CreateLexer(R"("\x1")");
    auto token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 5);
    ASSERT_EQ(token.string(), "\x1");

    lexer = CreateLexer(R"("\x12")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 6);
    ASSERT_EQ(token.string(), "\x12");

    lexer = CreateLexer(R"("\x120")");
    ASSERT_THROW_COMPILER_EXCEPTION(lexer.GetToken(), (Exception { 1, 2, 6, "hex escape sequence out of range" }));

    lexer = CreateLexer(R"("\xa")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 5);
    ASSERT_EQ(token.string(), "\xa");

    lexer = CreateLexer(R"("\xaB")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 6);
    ASSERT_EQ(token.string(), "\xab");

    lexer = CreateLexer(R"("a\xB")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 6);
    ASSERT_EQ(token.string(), "a\xb");

    lexer = CreateLexer(R"("a\xB1")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 7);
    ASSERT_EQ(token.string(), "a\xb1");

    lexer = CreateLexer(R"("a\xBR")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 7);
    ASSERT_EQ(token.string(), "a\xbR");

    lexer = CreateLexer(R"("a\xB2R")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 8);
    ASSERT_EQ(token.string(), "a\xb2R");

    lexer = CreateLexer(R"("\xB2R3")");
    token = lexer.GetToken();
    ASSERT_EQ(token.type, TOKEN_STRING);
    ASSERT_EQ(token.startLine, 1);
    ASSERT_EQ(token.startColumn, 1);
    ASSERT_EQ(token.endLine, 1);
    ASSERT_EQ(token.endColumn, 8);
    ASSERT_EQ(token.string(), "\xb2R3");
}