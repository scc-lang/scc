#include "test/test.h"
#include <memory>

import scc.compiler;

using namespace scc::compiler;

class ParserTest : public testing::Test {
protected:
    AstScope Parse(std::string content)
    {
        AstScope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        Parser {}.ParseCompileUnit(scope, lexer);
        return std::move(scope);
    }

    std::unique_ptr<AstFunctionCallExpression> ParseFunctionCallExpression(std::string content)
    {
        AstScope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        return std::unique_ptr<AstFunctionCallExpression> { dynamic_cast<AstFunctionCallExpression*>(Parser {}.ParseFunctionCallExpression(scope, lexer).release()) };
    }

    std::unique_ptr<AstBinaryExpression> ParseRelationalExpression(std::string content)
    {
        AstScope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        return std::unique_ptr<AstBinaryExpression> { dynamic_cast<AstBinaryExpression*>(Parser {}.ParseRelationalExpression(scope, lexer).release()) };
    }

    AstScope ParseStatement(std::string content)
    {
        AstScope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        Parser {}.ParseStatement(scope, lexer);
        return std::move(scope);
    }
};

TEST_F(ParserTest, ParseEmptyContent)
{
    auto scope = Parse("");
    ASSERT_TRUE(scope.statements.empty());
}

TEST_F(ParserTest, ParseFunctionCallExpression)
{
    auto func = ParseFunctionCallExpression("a()");
    ASSERT_TRUE(func->funcExpression);
    ASSERT_TRUE(func->argsExpression.empty());

    auto identifier = dynamic_cast<AstIdentifierExpression*>(func->funcExpression.get());
    ASSERT_TRUE(identifier);
    ASSERT_EQ(identifier->fullName, "a");

    func = ParseFunctionCallExpression("a::b::c(a(), d(), \"123\")");
    ASSERT_TRUE(func->funcExpression);
    ASSERT_EQ(func->argsExpression.size(), 3);

    identifier = dynamic_cast<AstIdentifierExpression*>(func->funcExpression.get());
    ASSERT_TRUE(identifier);
    ASSERT_EQ(identifier->fullName, "a::b::c");

    auto arg1 = dynamic_cast<AstFunctionCallExpression*>(func->argsExpression[0].get());
    ASSERT_TRUE(arg1->funcExpression);
    ASSERT_EQ(dynamic_cast<AstIdentifierExpression*>(arg1->funcExpression.get())->fullName, "a");
    ASSERT_TRUE(arg1->argsExpression.empty());

    auto arg2 = dynamic_cast<AstFunctionCallExpression*>(func->argsExpression[1].get());
    ASSERT_TRUE(arg2->funcExpression);
    ASSERT_EQ(dynamic_cast<AstIdentifierExpression*>(arg2->funcExpression.get())->fullName, "d");
    ASSERT_TRUE(arg2->argsExpression.empty());

    auto arg3 = dynamic_cast<AstStringLiteralExpression*>(func->argsExpression[2].get());
    ASSERT_EQ(arg3->value, "123");
}

TEST_F(ParserTest, ParseVariableDeclarationStatement)
{
    auto scope = ParseStatement("int a;");
    ASSERT_EQ(scope.variableDeclarations.size(), 1);
    {
        const auto& variableDeclaration = scope.variableDeclarations[0];
        ASSERT_EQ(variableDeclaration->typeInfo.fullName, "int");
        ASSERT_EQ(variableDeclaration->name, "a");
        ASSERT_EQ(variableDeclaration->initExpression, nullptr);
    }

    scope = ParseStatement("int a = foo();");
    ASSERT_EQ(scope.variableDeclarations.size(), 1);
    {
        const auto& variableDeclaration = scope.variableDeclarations[0];
        ASSERT_EQ(variableDeclaration->typeInfo.fullName, "int");
        ASSERT_EQ(variableDeclaration->name, "a");

        const auto* initExpression = dynamic_cast<AstFunctionCallExpression*>(variableDeclaration->initExpression.get());
        ASSERT_EQ(initExpression->argsExpression.size(), 0);

        auto identifier = dynamic_cast<AstIdentifierExpression*>(initExpression->funcExpression.get());
        ASSERT_EQ(identifier->fullName, "foo");
    }

    scope = ParseStatement("int a, int b = foo(), int c;");
    ASSERT_EQ(scope.variableDeclarations.size(), 3);
    {
        const auto& variableDeclaration = scope.variableDeclarations[0];
        ASSERT_EQ(variableDeclaration->typeInfo.fullName, "int");
        ASSERT_EQ(variableDeclaration->name, "a");
        ASSERT_EQ(variableDeclaration->initExpression, nullptr);
    }
    {
        const auto& variableDeclaration = scope.variableDeclarations[1];
        ASSERT_EQ(variableDeclaration->typeInfo.fullName, "int");
        ASSERT_EQ(variableDeclaration->name, "b");

        const auto* initExpression = dynamic_cast<AstFunctionCallExpression*>(variableDeclaration->initExpression.get());
        ASSERT_EQ(initExpression->argsExpression.size(), 0);

        auto identifier = dynamic_cast<AstIdentifierExpression*>(initExpression->funcExpression.get());
        ASSERT_EQ(identifier->fullName, "foo");
    }
    {
        const auto& variableDeclaration = scope.variableDeclarations[2];
        ASSERT_EQ(variableDeclaration->typeInfo.fullName, "int");
        ASSERT_EQ(variableDeclaration->name, "c");
        ASSERT_EQ(variableDeclaration->initExpression, nullptr);
    }
}

TEST_F(ParserTest, ParseRelationalExpression)
{
    auto binaryExpression = ParseRelationalExpression("a < b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Less);

    binaryExpression = ParseRelationalExpression("a > b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Greater);

    binaryExpression = ParseRelationalExpression("a <= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::LessEqual);

    binaryExpression = ParseRelationalExpression("a >= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::GreaterEqual);

    binaryExpression = ParseRelationalExpression("a < b(\"abc\") <= c >= d");
    ASSERT_EQ(binaryExpression->op, BinaryOp::GreaterEqual);

    auto rightOprand = dynamic_cast<AstIdentifierExpression*>(binaryExpression->rightOprand.get());
    ASSERT_NE(rightOprand, nullptr);
    ASSERT_EQ(rightOprand->fullName, "d");

    auto leftOprand = dynamic_cast<AstBinaryExpression*>(binaryExpression->leftOprand.get());
    ASSERT_NE(leftOprand, nullptr);
    ASSERT_EQ(leftOprand->op, BinaryOp::LessEqual);

    rightOprand = dynamic_cast<AstIdentifierExpression*>(leftOprand->rightOprand.get());
    ASSERT_NE(rightOprand, nullptr);
    ASSERT_EQ(rightOprand->fullName, "c");

    leftOprand = dynamic_cast<AstBinaryExpression*>(leftOprand->leftOprand.get());
    ASSERT_NE(leftOprand, nullptr);
    ASSERT_EQ(leftOprand->op, BinaryOp::Less);

    auto func = dynamic_cast<AstFunctionCallExpression*>(leftOprand->rightOprand.get());
    ASSERT_NE(func, nullptr);
    ASSERT_EQ(dynamic_cast<AstIdentifierExpression*>(func->funcExpression.get())->fullName, "b");
    ASSERT_EQ(func->argsExpression.size(), 1);
    ASSERT_EQ(dynamic_cast<AstStringLiteralExpression*>(func->argsExpression[0].get())->value, "abc");

    auto mostLeftOprand = dynamic_cast<AstIdentifierExpression*>(leftOprand->leftOprand.get());
    ASSERT_NE(mostLeftOprand, nullptr);
    ASSERT_EQ(mostLeftOprand->fullName, "a");
}