#include "test/test.h"
#include <memory>

import scc.ast;
import scc.compiler;

using namespace scc::ast;
using namespace scc::compiler;

class ParserTest : public testing::Test {
protected:
    Scope Parse(std::string content)
    {
        Scope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        Parser {}.ParseCompileUnit(scope, lexer);
        return std::move(scope);
    }

    std::unique_ptr<FunctionCallExpression> ParseFunctionCallExpression(std::string content)
    {
        Scope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        return std::unique_ptr<FunctionCallExpression> { dynamic_cast<FunctionCallExpression*>(Parser {}.ParseFunctionCallExpression(scope, lexer).release()) };
    }

    std::unique_ptr<BinaryExpression> ParseRelationalExpression(std::string content)
    {
        Scope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        return std::unique_ptr<BinaryExpression> { dynamic_cast<BinaryExpression*>(Parser {}.ParseRelationalExpression(scope, lexer).release()) };
    }

    std::unique_ptr<BinaryExpression> ParseAssignmentExpression(std::string content)
    {
        Scope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        return std::unique_ptr<BinaryExpression> { dynamic_cast<BinaryExpression*>(Parser {}.ParseAssignmentExpression(scope, lexer).release()) };
    }

    std::unique_ptr<Expression> ParseExpression(std::string content)
    {
        Scope scope {};
        Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
        return Parser {}.ParseExpression(scope, lexer);
    }

    Scope ParseStatement(std::string content)
    {
        Scope scope {};
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

    auto identifier = dynamic_cast<IdentifierExpression*>(func->funcExpression.get());
    ASSERT_TRUE(identifier);
    ASSERT_EQ(identifier->fullName, "a");

    func = ParseFunctionCallExpression("a::b::c(a(), d(), \"123\")");
    ASSERT_TRUE(func->funcExpression);
    ASSERT_EQ(func->argsExpression.size(), 3);

    identifier = dynamic_cast<IdentifierExpression*>(func->funcExpression.get());
    ASSERT_TRUE(identifier);
    ASSERT_EQ(identifier->fullName, "a::b::c");

    auto arg1 = dynamic_cast<FunctionCallExpression*>(func->argsExpression[0].get());
    ASSERT_TRUE(arg1->funcExpression);
    ASSERT_EQ(dynamic_cast<IdentifierExpression*>(arg1->funcExpression.get())->fullName, "a");
    ASSERT_TRUE(arg1->argsExpression.empty());

    auto arg2 = dynamic_cast<FunctionCallExpression*>(func->argsExpression[1].get());
    ASSERT_TRUE(arg2->funcExpression);
    ASSERT_EQ(dynamic_cast<IdentifierExpression*>(arg2->funcExpression.get())->fullName, "d");
    ASSERT_TRUE(arg2->argsExpression.empty());

    auto arg3 = dynamic_cast<StringLiteralExpression*>(func->argsExpression[2].get());
    ASSERT_EQ(arg3->value, "123");
}

TEST_F(ParserTest, ParseFunctionDefinitionStatement)
{
    auto scope = ParseStatement("int foo() {}");
    auto func = dynamic_cast<FunctionDefinitionStatement*>(scope.QueryFunction("foo"));
    ASSERT_NE(func, nullptr);
    ASSERT_EQ(func->typeInfo.fullName, "int");
    ASSERT_EQ(func->name, "foo");
    ASSERT_EQ(func->headerScope.statements.size(), 0);
    ASSERT_EQ(func->headerScope.variableDeclarations.size(), 0);
    ASSERT_EQ(func->bodyScope.statements.size(), 0);
    ASSERT_EQ(func->bodyScope.variableDeclarations.size(), 0);

    scope = ParseStatement(R"(int foo(int a, int b) {
    int c = a * b;
    return a + c;
})");
    func = dynamic_cast<FunctionDefinitionStatement*>(scope.QueryFunction("foo"));
    ASSERT_NE(func, nullptr);
    ASSERT_EQ(func->typeInfo.fullName, "int");
    ASSERT_EQ(func->name, "foo");
    ASSERT_EQ(func->headerScope.statements.size(), 2);
    ASSERT_EQ(func->headerScope.variableDeclarations.size(), 2);
    ASSERT_EQ(func->headerScope.variableDeclarations[0]->typeInfo.fullName, "int");
    ASSERT_EQ(func->headerScope.variableDeclarations[0]->name, "a");
    ASSERT_EQ(func->headerScope.variableDeclarations[0]->initExpression, nullptr);
    ASSERT_EQ(func->headerScope.variableDeclarations[1]->typeInfo.fullName, "int");
    ASSERT_EQ(func->headerScope.variableDeclarations[1]->name, "b");
    ASSERT_EQ(func->headerScope.variableDeclarations[1]->initExpression, nullptr);

    ASSERT_EQ(func->bodyScope.statements.size(), 2);
    ASSERT_NE(dynamic_cast<VariableDefinitionStatement*>(func->bodyScope.statements[0].get()), nullptr);
    ASSERT_EQ(dynamic_cast<BinaryExpression*>(dynamic_cast<ReturnStatement*>(func->bodyScope.statements[1].get())->expression.get())->op, BinaryOp::Add);
    ASSERT_EQ(func->bodyScope.variableDeclarations.size(), 1);
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

        const auto* initExpression = dynamic_cast<FunctionCallExpression*>(variableDeclaration->initExpression.get());
        ASSERT_EQ(initExpression->argsExpression.size(), 0);

        auto identifier = dynamic_cast<IdentifierExpression*>(initExpression->funcExpression.get());
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

        const auto* initExpression = dynamic_cast<FunctionCallExpression*>(variableDeclaration->initExpression.get());
        ASSERT_EQ(initExpression->argsExpression.size(), 0);

        auto identifier = dynamic_cast<IdentifierExpression*>(initExpression->funcExpression.get());
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

    auto rightOprand = dynamic_cast<IdentifierExpression*>(binaryExpression->rightOprand.get());
    ASSERT_EQ(rightOprand->fullName, "d");

    auto leftOprand = dynamic_cast<BinaryExpression*>(binaryExpression->leftOprand.get());
    ASSERT_EQ(leftOprand->op, BinaryOp::LessEqual);

    rightOprand = dynamic_cast<IdentifierExpression*>(leftOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->fullName, "c");

    leftOprand = dynamic_cast<BinaryExpression*>(leftOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->op, BinaryOp::Less);

    auto func = dynamic_cast<FunctionCallExpression*>(leftOprand->rightOprand.get());
    ASSERT_EQ(dynamic_cast<IdentifierExpression*>(func->funcExpression.get())->fullName, "b");
    ASSERT_EQ(func->argsExpression.size(), 1);
    ASSERT_EQ(dynamic_cast<StringLiteralExpression*>(func->argsExpression[0].get())->value, "abc");

    auto mostLeftOprand = dynamic_cast<IdentifierExpression*>(leftOprand->leftOprand.get());
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

    auto leftOprand = dynamic_cast<IdentifierExpression*>(binaryExpression->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "a");

    auto rightOprand = dynamic_cast<BinaryExpression*>(binaryExpression->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::MulAssignment);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "b");

    rightOprand = dynamic_cast<BinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::DivAssignment);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "c");

    rightOprand = dynamic_cast<BinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::ModAssignment);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "d");

    rightOprand = dynamic_cast<BinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::AddAssignment);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "e");

    rightOprand = dynamic_cast<BinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::SubAssignment);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "f");

    rightOprand = dynamic_cast<BinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::ShiftLeftAssignment);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "g");

    rightOprand = dynamic_cast<BinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::ShiftRightAssignment);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "h");

    rightOprand = dynamic_cast<BinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::BitAndAssignment);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "i");

    rightOprand = dynamic_cast<BinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::BitXorAssignment);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "j");

    rightOprand = dynamic_cast<BinaryExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::BitOrAssignment);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "k");

    auto mostRightOprand = dynamic_cast<IdentifierExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(mostRightOprand->fullName, "l");
}

TEST_F(ParserTest, ParseArithmeticExpression1)
{
    auto parse = [this](std::string content) {
        return std::unique_ptr<BinaryExpression> { dynamic_cast<BinaryExpression*>(ParseExpression(std::move(content)).release()) };
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

    auto rightOprand = dynamic_cast<IdentifierExpression*>(binaryExpression->rightOprand.get());
    ASSERT_EQ(rightOprand->fullName, "c");

    auto leftOprand = dynamic_cast<BinaryExpression*>(binaryExpression->leftOprand.get());
    ASSERT_EQ(leftOprand->op, BinaryOp::Add);

    rightOprand = dynamic_cast<IdentifierExpression*>(leftOprand->rightOprand.get());
    ASSERT_EQ(rightOprand->fullName, "b");

    auto mostLeftOprand = dynamic_cast<IdentifierExpression*>(leftOprand->leftOprand.get());
    ASSERT_EQ(mostLeftOprand->fullName, "a");
}

TEST_F(ParserTest, ParseArithmeticExpression2)
{
    auto parse = [this](std::string content) {
        return std::unique_ptr<BinaryExpression> { dynamic_cast<BinaryExpression*>(ParseExpression(std::move(content)).release()) };
    };

    auto binaryExpression = parse("a+b*c");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Add);

    auto leftOprand = dynamic_cast<IdentifierExpression*>(binaryExpression->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "a");

    auto rightOprand = dynamic_cast<BinaryExpression*>(binaryExpression->rightOprand.get());
    ASSERT_EQ(rightOprand->op, BinaryOp::Mul);

    leftOprand = dynamic_cast<IdentifierExpression*>(rightOprand->leftOprand.get());
    ASSERT_EQ(leftOprand->fullName, "b");

    auto mostRightOprand = dynamic_cast<IdentifierExpression*>(rightOprand->rightOprand.get());
    ASSERT_EQ(mostRightOprand->fullName, "c");
}

TEST_F(ParserTest, ParseArithmeticExpression3)
{
    auto parse = [this](std::string content) {
        return std::unique_ptr<BinaryExpression> { dynamic_cast<BinaryExpression*>(ParseExpression(std::move(content)).release()) };
    };

    auto binaryExpression = parse("a+b+c*d-e-f/g/h+i-j");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Sub);

    auto rightLeafExpression = std::unique_ptr<IdentifierExpression> { dynamic_cast<IdentifierExpression*>(binaryExpression->rightOprand.release()) };
    ASSERT_EQ(rightLeafExpression->fullName, "j");

    auto leftBinaryExpression = std::unique_ptr<BinaryExpression> { dynamic_cast<BinaryExpression*>(binaryExpression->leftOprand.release()) };
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Add);

    rightLeafExpression.reset(dynamic_cast<IdentifierExpression*>(leftBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "i");

    binaryExpression.reset(dynamic_cast<BinaryExpression*>(leftBinaryExpression->leftOprand.release()));
    ASSERT_EQ(binaryExpression->op, BinaryOp::Sub);

    auto rightBinaryExpression = std::unique_ptr<BinaryExpression> { dynamic_cast<BinaryExpression*>(binaryExpression->rightOprand.release()) };
    ASSERT_EQ(rightBinaryExpression->op, BinaryOp::Div);

    rightLeafExpression.reset(dynamic_cast<IdentifierExpression*>(rightBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "h");

    leftBinaryExpression.reset(dynamic_cast<BinaryExpression*>(rightBinaryExpression->leftOprand.release()));
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Div);

    auto leftLeafExpression = std::unique_ptr<IdentifierExpression> { dynamic_cast<IdentifierExpression*>(leftBinaryExpression->leftOprand.release()) };
    ASSERT_EQ(leftLeafExpression->fullName, "f");

    rightLeafExpression.reset(dynamic_cast<IdentifierExpression*>(leftBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "g");

    leftBinaryExpression.reset(dynamic_cast<BinaryExpression*>(binaryExpression->leftOprand.release()));
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Sub);

    rightLeafExpression.reset(dynamic_cast<IdentifierExpression*>(leftBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "e");

    leftBinaryExpression.reset(dynamic_cast<BinaryExpression*>(leftBinaryExpression->leftOprand.release()));
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Add);

    rightBinaryExpression.reset(dynamic_cast<BinaryExpression*>(leftBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightBinaryExpression->op, BinaryOp::Mul);

    rightLeafExpression.reset(dynamic_cast<IdentifierExpression*>(rightBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "d");

    leftLeafExpression.reset(dynamic_cast<IdentifierExpression*>(rightBinaryExpression->leftOprand.release()));
    ASSERT_EQ(leftLeafExpression->fullName, "c");

    leftBinaryExpression.reset(dynamic_cast<BinaryExpression*>(leftBinaryExpression->leftOprand.release()));
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Add);

    rightLeafExpression.reset(dynamic_cast<IdentifierExpression*>(leftBinaryExpression->rightOprand.release()));
    ASSERT_EQ(rightLeafExpression->fullName, "b");

    leftLeafExpression.reset(dynamic_cast<IdentifierExpression*>(leftBinaryExpression->leftOprand.release()));
    ASSERT_EQ(leftLeafExpression->fullName, "a");
}

TEST_F(ParserTest, ParseArithmeticExpression4)
{
    auto parse = [this](std::string content) {
        return std::unique_ptr<BinaryExpression> { dynamic_cast<BinaryExpression*>(ParseExpression(std::move(content)).release()) };
    };

    auto binaryExpression = parse("(a+b)*c");
    ASSERT_EQ(binaryExpression->op, BinaryOp::Mul);

    auto leftUnaryExpression = dynamic_cast<UnaryExpression*>(binaryExpression->leftOprand.get());
    ASSERT_EQ(leftUnaryExpression->op, UnaryOp::Bracket);

    auto leftBinaryExpression = dynamic_cast<BinaryExpression*>(leftUnaryExpression->oprand.get());
    ASSERT_EQ(leftBinaryExpression->op, BinaryOp::Add);

    auto rightLeafExpression = dynamic_cast<IdentifierExpression*>(leftBinaryExpression->rightOprand.get());
    ASSERT_EQ(rightLeafExpression->fullName, "b");

    auto leftLeafExpression = dynamic_cast<IdentifierExpression*>(leftBinaryExpression->leftOprand.get());
    ASSERT_EQ(leftLeafExpression->fullName, "a");

    rightLeafExpression = dynamic_cast<IdentifierExpression*>(binaryExpression->rightOprand.get());
    ASSERT_EQ(rightLeafExpression->fullName, "c");
}

TEST_F(ParserTest, ParseForLoopStatement)
{
    auto scope = ParseStatement("for (;;) {}");
    ASSERT_EQ(scope.statements.size(), 1);

    auto forStatement = dynamic_cast<ForLoopStatement*>(scope.statements[0].get());
    ASSERT_EQ(forStatement->initScope.statements.size(), 0);
    ASSERT_EQ(forStatement->initScope.variableDeclarations.size(), 0);
    ASSERT_EQ(forStatement->conditionalExpression, nullptr);
    ASSERT_EQ(forStatement->iterationExpression, nullptr);
    ASSERT_EQ(forStatement->bodyScope.statements.size(), 0);
    ASSERT_EQ(forStatement->bodyScope.variableDeclarations.size(), 0);

    scope = ParseStatement("for (int a, int b = 10; a < b; a += 2) {}");
    ASSERT_EQ(scope.statements.size(), 1);

    forStatement = dynamic_cast<ForLoopStatement*>(scope.statements[0].get());
    ASSERT_EQ(forStatement->initScope.statements.size(), 2);
    ASSERT_EQ(forStatement->initScope.variableDeclarations.size(), 2);
    ASSERT_NE(forStatement->conditionalExpression, nullptr);
    ASSERT_NE(forStatement->iterationExpression, nullptr);
    ASSERT_EQ(forStatement->bodyScope.statements.size(), 0);
    ASSERT_EQ(forStatement->bodyScope.variableDeclarations.size(), 0);

    scope = ParseStatement(R"(
for (int a, int b = 10; a < b; a += 2) {
    foo(a + b);
})");
    ASSERT_EQ(scope.statements.size(), 1);

    forStatement = dynamic_cast<ForLoopStatement*>(scope.statements[0].get());
    ASSERT_EQ(forStatement->initScope.statements.size(), 2);
    ASSERT_EQ(forStatement->initScope.variableDeclarations.size(), 2);
    ASSERT_NE(forStatement->conditionalExpression, nullptr);
    ASSERT_NE(forStatement->iterationExpression, nullptr);
    ASSERT_EQ(forStatement->bodyScope.statements.size(), 1);
    ASSERT_EQ(forStatement->bodyScope.variableDeclarations.size(), 0);

    auto funcStatement = dynamic_cast<ExpressionStatement*>(forStatement->bodyScope.statements[0].get());
    ASSERT_NE(funcStatement, nullptr);

    auto functionCallExpression = dynamic_cast<FunctionCallExpression*>(funcStatement->expression.get());
    ASSERT_NE(functionCallExpression, nullptr);
    ASSERT_EQ(dynamic_cast<IdentifierExpression*>(functionCallExpression->funcExpression.get())->fullName, "foo");
}

TEST_F(ParserTest, ParseIfStatement)
{
    auto scope = ParseStatement("if (a) {}");
    ASSERT_EQ(scope.statements.size(), 1);

    auto ifStatement = dynamic_cast<ConditionalStatement*>(scope.statements[0].get());
    ASSERT_EQ(dynamic_cast<IdentifierExpression*>(ifStatement->conditionalExpression.get())->fullName, "a");
    ASSERT_EQ(ifStatement->trueScope.statements.size(), 0);
    ASSERT_EQ(ifStatement->trueScope.variableDeclarations.size(), 0);
    ASSERT_EQ(ifStatement->falseScope.statements.size(), 0);
    ASSERT_EQ(ifStatement->falseScope.variableDeclarations.size(), 0);

    scope = ParseStatement("if (a) {a + b; a * b;}");
    ASSERT_EQ(scope.statements.size(), 1);

    ifStatement = dynamic_cast<ConditionalStatement*>(scope.statements[0].get());
    ASSERT_EQ(ifStatement->trueScope.statements.size(), 2);
    ASSERT_EQ(ifStatement->trueScope.variableDeclarations.size(), 0);
    ASSERT_EQ(ifStatement->falseScope.statements.size(), 0);
    ASSERT_EQ(ifStatement->falseScope.variableDeclarations.size(), 0);

    auto binaryExpression = dynamic_cast<BinaryExpression*>(dynamic_cast<ExpressionStatement*>(ifStatement->trueScope.statements[0].get())->expression.get());
    ASSERT_EQ(binaryExpression->op, BinaryOp::Add);
    binaryExpression = dynamic_cast<BinaryExpression*>(dynamic_cast<ExpressionStatement*>(ifStatement->trueScope.statements[1].get())->expression.get());
    ASSERT_EQ(binaryExpression->op, BinaryOp::Mul);

    scope = ParseStatement("if (a) {a + b; a * b;} else {c - d; c / d;}");
    ASSERT_EQ(scope.statements.size(), 1);

    ifStatement = dynamic_cast<ConditionalStatement*>(scope.statements[0].get());
    ASSERT_EQ(ifStatement->trueScope.statements.size(), 2);
    ASSERT_EQ(ifStatement->trueScope.variableDeclarations.size(), 0);
    ASSERT_EQ(ifStatement->falseScope.statements.size(), 2);
    ASSERT_EQ(ifStatement->falseScope.variableDeclarations.size(), 0);

    binaryExpression = dynamic_cast<BinaryExpression*>(dynamic_cast<ExpressionStatement*>(ifStatement->trueScope.statements[0].get())->expression.get());
    ASSERT_EQ(binaryExpression->op, BinaryOp::Add);
    binaryExpression = dynamic_cast<BinaryExpression*>(dynamic_cast<ExpressionStatement*>(ifStatement->trueScope.statements[1].get())->expression.get());
    ASSERT_EQ(binaryExpression->op, BinaryOp::Mul);

    binaryExpression = dynamic_cast<BinaryExpression*>(dynamic_cast<ExpressionStatement*>(ifStatement->falseScope.statements[0].get())->expression.get());
    ASSERT_EQ(binaryExpression->op, BinaryOp::Sub);
    binaryExpression = dynamic_cast<BinaryExpression*>(dynamic_cast<ExpressionStatement*>(ifStatement->falseScope.statements[1].get())->expression.get());
    ASSERT_EQ(binaryExpression->op, BinaryOp::Div);

    scope = ParseStatement("if (a) {a + b; a * b;} else if (a) {c - d; c / d;}");
    ASSERT_EQ(scope.statements.size(), 1);

    ifStatement = dynamic_cast<ConditionalStatement*>(scope.statements[0].get());
    ASSERT_EQ(ifStatement->trueScope.statements.size(), 2);
    ASSERT_EQ(ifStatement->trueScope.variableDeclarations.size(), 0);
    ASSERT_EQ(ifStatement->falseScope.statements.size(), 1);
    ASSERT_EQ(ifStatement->falseScope.variableDeclarations.size(), 0);

    binaryExpression = dynamic_cast<BinaryExpression*>(dynamic_cast<ExpressionStatement*>(ifStatement->trueScope.statements[0].get())->expression.get());
    ASSERT_EQ(binaryExpression->op, BinaryOp::Add);
    binaryExpression = dynamic_cast<BinaryExpression*>(dynamic_cast<ExpressionStatement*>(ifStatement->trueScope.statements[1].get())->expression.get());
    ASSERT_EQ(binaryExpression->op, BinaryOp::Mul);

    ifStatement = dynamic_cast<ConditionalStatement*>(ifStatement->falseScope.statements[0].get());
    ASSERT_EQ(ifStatement->trueScope.statements.size(), 2);
    ASSERT_EQ(ifStatement->trueScope.variableDeclarations.size(), 0);
    ASSERT_EQ(ifStatement->falseScope.statements.size(), 0);
    ASSERT_EQ(ifStatement->falseScope.variableDeclarations.size(), 0);
}

TEST_F(ParserTest, CannotHaveOtherGlobalStatementsIfMainFunctionIsDefined)
{
    auto content = R"(int a = 10;
int b = 10;

void main() {
    std::println("a + b = {}", a + b);
})";

    Scope scope {};
    Lexer lexer { std::make_shared<std::istringstream>(std::move(content)) };
    ASSERT_THROW_COMPILER_EXCEPTION(Parser {}.ParseCompileUnit(scope, lexer), (Exception { 1, 1, 11, "unexpected global statement when 'main' function is defined (4:1)" }));
}
