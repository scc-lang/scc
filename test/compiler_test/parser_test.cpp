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

    std::unique_ptr<AstBinaryExpression> ParseAssignmentExpression(std::string content)
    {
        AstScope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        return std::unique_ptr<AstBinaryExpression> { dynamic_cast<AstBinaryExpression*>(Parser {}.ParseAssignmentExpression(scope, lexer).release()) };
    }

    std::unique_ptr<AstExpression> ParseExpression(std::string content)
    {
        AstScope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        return Parser {}.ParseExpression(scope, lexer);
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
    ASSERT_EQ(rightOprand->fullName, "d");

    auto leftOprand = dynamic_cast<AstBinaryExpression*>(binaryExpression->leftOprand.get());
    ASSERT_EQ(leftOprand->op, BinaryOp::LessEqual);

    rightOprand = dynamic_cast<AstIdentifierExpression*>(leftOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->fullName, "c");

    leftOprand = dynamic_cast<AstBinaryExpression*>(leftOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->op, BinaryOp::Less);

    auto func = dynamic_cast<AstFunctionCallExpression*>(leftOprand->rightOprand.get());
    ASSERT_EQ(dynamic_cast<AstIdentifierExpression*>(func->funcExpression.get())->fullName, "b");
    ASSERT_EQ(func->argsExpression.size(), 1);
    ASSERT_EQ(dynamic_cast<AstStringLiteralExpression*>(func->argsExpression[0].get())->value, "abc");

    auto mostLeftOprand = dynamic_cast<AstIdentifierExpression*>(leftOprand->leftOprand.get());
    ASSERT_EQ(mostLeftOprand->fullName, "a");
}

TEST_F(ParserTest, ParseAssignmentExpression)
{
    auto binaryExpression = ParseAssignmentExpression("a = b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Assignment);

    binaryExpression = ParseAssignmentExpression("a *= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::MulAssignment);

    binaryExpression = ParseAssignmentExpression("a /= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::DivAssignment);

    binaryExpression = ParseAssignmentExpression("a %= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::ModAssignment);

    binaryExpression = ParseAssignmentExpression("a += b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::AddAssignment);

    binaryExpression = ParseAssignmentExpression("a -= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::SubAssignment);

    binaryExpression = ParseAssignmentExpression("a <<= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::ShiftLeftAssignment);

    binaryExpression = ParseAssignmentExpression("a >>= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::ShiftRightAssignment);

    binaryExpression = ParseAssignmentExpression("a &= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::BitAndAssignment);

    binaryExpression = ParseAssignmentExpression("a ^= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::BitXorAssignment);

    binaryExpression = ParseAssignmentExpression("a |= b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::BitOrAssignment);

    binaryExpression = ParseAssignmentExpression("a = b *= c /= d %= e += f -= g <<= h >>= i &= j ^= k |= l");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Assignment);

    auto leftOprand = dynamic_cast<AstIdentifierExpression*>(binaryExpression->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "a");

    auto rightOprand = dynamic_cast<AstBinaryExpression*>(binaryExpression->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::MulAssignment);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "b");

    rightOprand = dynamic_cast<AstBinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::DivAssignment);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "c");

    rightOprand = dynamic_cast<AstBinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::ModAssignment);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "d");

    rightOprand = dynamic_cast<AstBinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::AddAssignment);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "e");

    rightOprand = dynamic_cast<AstBinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::SubAssignment);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "f");

    rightOprand = dynamic_cast<AstBinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::ShiftLeftAssignment);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "g");

    rightOprand = dynamic_cast<AstBinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::ShiftRightAssignment);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "h");

    rightOprand = dynamic_cast<AstBinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::BitAndAssignment);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "i");

    rightOprand = dynamic_cast<AstBinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::BitXorAssignment);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "j");

    rightOprand = dynamic_cast<AstBinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::BitOrAssignment);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "k");

    auto mostRightOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(mostRightOprand->fullName, "l");
}

TEST_F(ParserTest, ParseArithmeticExpression1)
{
    auto parse = [this](std::string content) {
        return std::unique_ptr<AstBinaryExpression> { dynamic_cast<AstBinaryExpression*>(ParseExpression(std::move(content)).release()) };
    };

    auto binaryExpression = parse("a+b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Add);

    binaryExpression = parse("a-b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Sub);

    binaryExpression = parse("a*b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Mul);

    binaryExpression = parse("a/b");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Div);

    binaryExpression = parse("a+b-c");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Sub);

    auto rightOprand = dynamic_cast<AstIdentifierExpression*>(binaryExpression->rightOprand.get());
    ASSERT_EQ(rightOprand->fullName, "c");

    auto leftOprand = dynamic_cast<AstBinaryExpression*>(binaryExpression->leftOprand.get());
    ASSERT_EQ(leftOprand->op, BinaryOp::Add);

    rightOprand = dynamic_cast<AstIdentifierExpression*>(leftOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->fullName, "b");

    auto mostLeftOprand = dynamic_cast<AstIdentifierExpression*>(leftOprand->leftOprand.get());
    ASSERT_EQ(mostLeftOprand->fullName, "a");
}

TEST_F(ParserTest, ParseArithmeticExpression2)
{
    auto parse = [this](std::string content) {
        return std::unique_ptr<AstBinaryExpression> { dynamic_cast<AstBinaryExpression*>(ParseExpression(std::move(content)).release()) };
    };

    auto binaryExpression = parse("a+b*c");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Add);

    auto leftOprand = dynamic_cast<AstIdentifierExpression*>(binaryExpression->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "a");

    auto rightOprand = dynamic_cast<AstBinaryExpression*>(binaryExpression->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::Mul);

    leftOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "b");

    auto mostRightOprand = dynamic_cast<AstIdentifierExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(mostRightOprand->fullName, "c");
}

TEST_F(ParserTest, ParseArithmeticExpression3)
{
    auto parse = [this](std::string content) {
        return std::unique_ptr<AstBinaryExpression> { dynamic_cast<AstBinaryExpression*>(ParseExpression(std::move(content)).release()) };
    };

    auto binaryExpression = parse("a+b+c*d-e-f/g/h+i-j");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Sub);

    auto rightLeafExpression = std::unique_ptr<AstIdentifierExpression> { dynamic_cast<AstIdentifierExpression*>(binaryExpression->rightOprand.release()) };
    ASSERT_EQ(rightLeafExpression->fullName, "j");

    auto leftBinaryExpression = std::unique_ptr<AstBinaryExpression> { dynamic_cast<AstBinaryExpression*>(binaryExpression->leftOprand.release()) };
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Add);

    rightLeafExpression.reset(dynamic_cast<AstIdentifierExpression*>(leftBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "i");

    binaryExpression.reset(dynamic_cast<AstBinaryExpression*>(leftBinaryExpression->leftOprand.release()));
    ASSERT_EQ(binaryExpression->op, BinaryOp::Sub);

    auto rightBinaryExpression = std::unique_ptr<AstBinaryExpression> { dynamic_cast<AstBinaryExpression*>(binaryExpression->rightOprand.release()) };
    ASSERT_EQ(rightBinaryExpression->op, BinaryOp::Div);

    rightLeafExpression.reset(dynamic_cast<AstIdentifierExpression*>(rightBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "h");

    leftBinaryExpression.reset(dynamic_cast<AstBinaryExpression*>(rightBinaryExpression->leftOprand.release()));
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Div);

    auto leftLeafExpression = std::unique_ptr<AstIdentifierExpression> { dynamic_cast<AstIdentifierExpression*>(leftBinaryExpression->leftOprand.release()) };
    ASSERT_EQ(leftLeafExpression->fullName, "f");

    rightLeafExpression.reset(dynamic_cast<AstIdentifierExpression*>(leftBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "g");

    leftBinaryExpression.reset(dynamic_cast<AstBinaryExpression*>(binaryExpression->leftOprand.release()));
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Sub);

    rightLeafExpression.reset(dynamic_cast<AstIdentifierExpression*>(leftBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "e");

    leftBinaryExpression.reset(dynamic_cast<AstBinaryExpression*>(leftBinaryExpression->leftOprand.release()));
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Add);

    rightBinaryExpression.reset(dynamic_cast<AstBinaryExpression*>(leftBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightBinaryExpression->op, BinaryOp::Mul);

    rightLeafExpression.reset(dynamic_cast<AstIdentifierExpression*>(rightBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "d");

    leftLeafExpression.reset(dynamic_cast<AstIdentifierExpression*>(rightBinaryExpression->leftOprand.release()));
    ASSERT_EQ(leftLeafExpression->fullName, "c");

    leftBinaryExpression.reset(dynamic_cast<AstBinaryExpression*>(leftBinaryExpression->leftOprand.release()));
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Add);

    rightLeafExpression.reset(dynamic_cast<AstIdentifierExpression*>(leftBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "b");

    leftLeafExpression.reset(dynamic_cast<AstIdentifierExpression*>(leftBinaryExpression->leftOprand.release()));
    ASSERT_EQ(leftLeafExpression->fullName, "a");
}

TEST_F(ParserTest, ParseArithmeticExpression4)
{
    auto parse = [this](std::string content) {
        return std::unique_ptr<AstBinaryExpression> { dynamic_cast<AstBinaryExpression*>(ParseExpression(std::move(content)).release()) };
    };

    auto binaryExpression = parse("(a+b)*c");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Mul);

    auto LeftBinaryExpression = dynamic_cast<AstBinaryExpression*>(binaryExpression->leftOprand.get());
    ASSERT_EQ(LeftBinaryExpression->op, BinaryOp::Add);

    auto rightLeafExpression = dynamic_cast<AstIdentifierExpression*>(LeftBinaryExpression->rightOprand.get());
    ASSERT_EQ(rightLeafExpression->fullName, "b");

    auto leftLeafExpression = dynamic_cast<AstIdentifierExpression*>(LeftBinaryExpression->leftOprand.get());
    ASSERT_EQ(leftLeafExpression->fullName, "a");

    rightLeafExpression = dynamic_cast<AstIdentifierExpression*>(binaryExpression->rightOprand.get());
    ASSERT_EQ(rightLeafExpression->fullName, "c");
}

TEST_F(ParserTest, ParseForLoopStatement)
{
    auto scope = ParseStatement("for (;;) {}");
    ASSERT_EQ(scope.statements.size(), 1);

    auto forStatement = dynamic_cast<AstLoopStatement*>(scope.statements[0].get());
    ASSERT_EQ(forStatement->scope.statements.size(), 0);
    ASSERT_EQ(forStatement->scope.variableDeclarations.size(), 0);

    scope = ParseStatement("for (int a, int b = 10; a < b; a += 2) {}");
    ASSERT_EQ(scope.statements.size(), 1);

    forStatement = dynamic_cast<AstLoopStatement*>(scope.statements[0].get());
    ASSERT_EQ(forStatement->scope.statements.size(), 4);
    ASSERT_EQ(forStatement->scope.variableDeclarations.size(), 2);

    scope = ParseStatement(R"(
for (int a, int b = 10; a < b; a += 2) {
    foo(a + b);
})");
    ASSERT_EQ(scope.statements.size(), 1);

    forStatement = dynamic_cast<AstLoopStatement*>(scope.statements[0].get());
    ASSERT_EQ(forStatement->scope.statements.size(), 5);
    ASSERT_EQ(forStatement->scope.variableDeclarations.size(), 2);

    auto funcStatement = dynamic_cast<AstExpressionStatement*>(forStatement->scope.statements[3].get());
    ASSERT_NE(funcStatement, nullptr);

    auto functionCallExpression = dynamic_cast<AstFunctionCallExpression*>(funcStatement->expression.get());
    ASSERT_NE(functionCallExpression, nullptr);
    ASSERT_EQ(dynamic_cast<AstIdentifierExpression*>(functionCallExpression->funcExpression.get())->fullName, "foo");
}