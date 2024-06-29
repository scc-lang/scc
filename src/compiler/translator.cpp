module;

#include <cassert>
#include <memory>
#include <ostream>

import scc.ast;

export module scc.compiler:translator;
import :printer;

namespace scc::compiler {

using namespace ast;

export struct Translator final : Visitor {
    Translator(std::shared_ptr<std::ostream> out)
        : m_printer { std::move(out) }
    {
    }

    void VisitAstBinaryExpression(const BinaryExpression& binaryExpression) override
    {
        binaryExpression.leftOprand->Visit(*this);
        switch (binaryExpression.op) {
        case BinaryOp::Assignment:
            m_printer.Print(" = ");
            break;
        case BinaryOp::MulAssignment:
            m_printer.Print(" *= ");
            break;
        case BinaryOp::DivAssignment:
            m_printer.Print(" /= ");
            break;
        case BinaryOp::ModAssignment:
            m_printer.Print(" %= ");
            break;
        case BinaryOp::AddAssignment:
            m_printer.Print(" += ");
            break;
        case BinaryOp::SubAssignment:
            m_printer.Print(" -= ");
            break;
        case BinaryOp::ShiftLeftAssignment:
            m_printer.Print(" <<= ");
            break;
        case BinaryOp::ShiftRightAssignment:
            m_printer.Print(" >>= ");
            break;
        case BinaryOp::BitAndAssignment:
            m_printer.Print(" &= ");
            break;
        case BinaryOp::BitXorAssignment:
            m_printer.Print(" ^= ");
            break;
        case BinaryOp::BitOrAssignment:
            m_printer.Print(" |= ");
            break;

        case BinaryOp::Mul:
            m_printer.Print(" * ");
            break;
        case BinaryOp::Div:
            m_printer.Print(" / ");
            break;
        case BinaryOp::Mod:
            m_printer.Print(" % ");
            break;
        case BinaryOp::Add:
            m_printer.Print(" + ");
            break;
        case BinaryOp::Sub:
            m_printer.Print(" - ");
            break;

        case BinaryOp::Equal:
            m_printer.Print(" == ");
            break;
        case BinaryOp::NotEqual:
            m_printer.Print(" != ");
            break;
        case BinaryOp::Less:
            m_printer.Print(" < ");
            break;
        case BinaryOp::LessEqual:
            m_printer.Print(" <= ");
            break;
        case BinaryOp::Greater:
            m_printer.Print(" > ");
            break;
        case BinaryOp::GreaterEqual:
            m_printer.Print(" >= ");
            break;

        default:
            assert(false);
        }
        binaryExpression.rightOprand->Visit(*this);
    }

    void VisitAstBreakStatement(const BreakStatement& breakStatement) override
    {
    }

    void VisitAstConditionalStatement(const ConditionalStatement& conditionalStatement) override
    {
        m_printer.Print("if (");
        conditionalStatement.conditionalExpression->Visit(*this);
        m_printer.Println(")");
        VisitAstScope(conditionalStatement.trueScope);
        m_printer.Println("else");
        VisitAstScope(conditionalStatement.falseScope);
    }

    void VisitAstExpressionStatement(const ExpressionStatement& expressionStatement) override
    {
        assert(expressionStatement.expression);
        expressionStatement.expression->Visit(*this);
        m_printer.Println(";");
    }

    void VisitAstFunctionCallExpression(const FunctionCallExpression& functionCallExpression) override
    {
        assert(functionCallExpression.funcExpression);
        functionCallExpression.funcExpression->Visit(*this);

        m_printer.Print("(");
        if (!functionCallExpression.argsExpression.empty()) {
            auto it = functionCallExpression.argsExpression.begin();
            assert(*it);
            (*it)->Visit(*this);
            while (++it != functionCallExpression.argsExpression.end()) {
                m_printer.Print(", ");
                (*it)->Visit(*this);
            }
        }
        m_printer.Print(")");
    }

    void VisitFunctionDefinitionStatement(const FunctionDefinitionStatement& functionDefinitionStatement) override
    {
        PrintFunctionHeader(functionDefinitionStatement);
        m_printer.Println();
        VisitAstScope(functionDefinitionStatement.bodyScope);
    }

    void VisitAstIdentifierExpression(const IdentifierExpression& identifierExpression) override
    {
        if (identifierExpression.fullName.starts_with("std::")) {
            m_printer.Print("scc::{}", identifierExpression.fullName);
        } else {
            m_printer.Print(identifierExpression.fullName);
        }
    }

    void VisitAstIntegerLiteralExpression(const IntegerLiteralExpression& integerLiteralExpression) override
    {
        m_printer.Print("{}", integerLiteralExpression.value);
    }

    void VisitAstForLoopStatement(const ForLoopStatement& forLoopStatement) override
    {
        m_printer.Println("{{");
        m_printer.PushIndent();

        for (const auto& statement : forLoopStatement.initScope.statements) {
            statement->Visit(*this);
        }
        m_printer.Println();
        m_printer.Print("for (;");
        if (forLoopStatement.conditionalExpression) {
            m_printer.Print(" ");
            forLoopStatement.conditionalExpression->Visit(*this);
        }
        m_printer.Print(";");

        if (forLoopStatement.iterationExpression) {
            m_printer.Print(" ");
            forLoopStatement.iterationExpression->Visit(*this);
        }
        m_printer.Println(")");
        VisitAstScope(forLoopStatement.bodyScope);

        m_printer.PopIndent();
        m_printer.Println("}}");
    }

    void VisitReturnStatement(const ReturnStatement& returnStatement) override
    {
        if (returnStatement.expression) {
            m_printer.Print("return ");
            returnStatement.expression->Visit(*this);
        } else {
            m_printer.Print("return");
        }
        m_printer.Println(";");
    }

    void VisitAstScope(const Scope& scope) override
    {
        if (!scope.parentScope) {
            // Output declare for global scope.
            m_printer.Println("// scc autogenerated file.");
            m_printer.Println();
            m_printer.Println("import scc.std;");
            m_printer.Println();

            // Output function forward declaration.
            auto functions = scope.GetFunctions();
            if (!functions.empty()) {
                m_printer.Println("// function declarations");
                for (const auto& func : functions) {
                    PrintFunctionHeader(*static_cast<FunctionDefinitionStatement*>(func));
                    m_printer.Println(";");
                }
                m_printer.Println("int main();");
                m_printer.Println();

                // Output function.
                m_printer.Println("// function definitions");
                for (const auto& func : functions) {
                    VisitFunctionDefinitionStatement(*static_cast<FunctionDefinitionStatement*>(func));
                    m_printer.Println();
                }
            }

            // Output main.
            m_printer.Println("int main()");
        }

        m_printer.Println("{{");
        m_printer.PushIndent();
        for (const auto& statement : scope.statements) {
            statement->Visit(*this);
        }

        if (!scope.parentScope) {
            m_printer.Println("return 0;");
        }
        m_printer.PopIndent();
        m_printer.Println("}}");
    }

    void VisitAstStringLiteralExpression(const StringLiteralExpression& stringLiteralExpression) override
    {
        m_printer.Print("\"");
        for (const auto ch : stringLiteralExpression.value) {
            if (std::isprint(ch)) {
                m_printer.Print("{}", ch);
            } else {
                m_printer.Print("\\{:#03o}", ch);
            }
        }
        m_printer.Print("\"");
    }

    void VisitAstUnaryExpression(const UnaryExpression& unaryExpression) override
    {
        m_printer.Print("(");

        assert(unaryExpression.oprand);
        unaryExpression.oprand->Visit(*this);

        m_printer.Print(")");
    }

    void VisitAstVariableDeclaration(const VariableDeclaration& variableDeclaration) override
    {
        PrintTypeInfo(variableDeclaration.typeInfo);
        m_printer.Print(" ");
        m_printer.Print(variableDeclaration.name);
    }

    void VisitAstVariableDefinitionStatement(const VariableDefinitionStatement& variableDefinitionStatemet) override
    {
        const auto& variableDeclaration = variableDefinitionStatemet.variableDeclaration;
        VisitAstVariableDeclaration(variableDeclaration);
        m_printer.Print(" {{");
        if (variableDeclaration.initExpression) {
            m_printer.Print(" ");
            variableDeclaration.initExpression->Visit(*this);
            m_printer.Print(" ");
        }
        m_printer.Println("}};");
    }

private:
    void PrintTypeInfo(const TypeInfo& typeInfo)
    {
        m_printer.Print(typeInfo.fullName);
    }

    void PrintFunctionHeader(const FunctionDefinitionStatement& functionDefinitionStatement)
    {
        PrintTypeInfo(functionDefinitionStatement.typeInfo);

        m_printer.Print(" ");

        m_printer.Print(functionDefinitionStatement.name);

        m_printer.Print("(");
        if (auto it = functionDefinitionStatement.headerScope.variableDeclarations.begin(); it != functionDefinitionStatement.headerScope.variableDeclarations.end()) {
            VisitAstVariableDeclaration(**it++);
            while (it != functionDefinitionStatement.headerScope.variableDeclarations.end()) {
                m_printer.Print(", ");
                VisitAstVariableDeclaration(**it++);
            }
        }
        m_printer.Print(")");
    }

    Printer m_printer;
};

}