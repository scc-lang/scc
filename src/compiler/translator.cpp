module;

#include <cassert>
#include <memory>
#include <ostream>

import scc.ast;

export module scc.compiler:translator;
import :printer;

namespace scc::compiler {

using namespace ast;

export struct Translator final : AstVisitor {
    Translator(std::shared_ptr<std::ostream> out)
        : m_printer { std::move(out) }
    {
    }

    void VisitAstBinaryExpression(const AstBinaryExpression& binaryExpression) override
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

    void VisitAstBreakStatement(const AstBreakStatement& breakStatement) override
    {
    }

    void VisitAstConditionalStatement(const AstConditionalStatement& conditionalStatement) override
    {
        m_printer.Print("if (");
        conditionalStatement.conditionalExpression->Visit(*this);
        m_printer.Println(")");
        VisitAstScope(conditionalStatement.trueScope);
        m_printer.Println("else");
        VisitAstScope(conditionalStatement.falseScope);
    }

    void VisitAstExpressionStatement(const AstExpressionStatement& expressionStatement) override
    {
        assert(expressionStatement.expression);
        expressionStatement.expression->Visit(*this);
        m_printer.Println(";");
    }

    void VisitAstFunctionCallExpression(const AstFunctionCallExpression& functionCallExpression) override
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

    void VisitAstIdentifierExpression(const AstIdentifierExpression& identifierExpression) override
    {
        if (identifierExpression.fullName.starts_with("std::")) {
            m_printer.Print("scc::{}", identifierExpression.fullName);
        } else {
            m_printer.Print(identifierExpression.fullName);
        }
    }

    void VisitAstIntegerLiteralExpression(const AstIntegerLiteralExpression& integerLiteralExpression) override
    {
        m_printer.Print("{}", integerLiteralExpression.value);
    }

    void VisitAstForLoopStatement(const AstForLoopStatement& forLoopStatement) override
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

    void VisitAstScope(const AstScope& scope) override
    {
        if (!scope.parentScope) {
            // Output declare for global scope.
            m_printer.Println("// scc autogenerated file.");
            m_printer.Println();
            m_printer.Println("import scc.std;");
            m_printer.Println();
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

    void VisitAstStringLiteralExpression(const AstStringLiteralExpression& stringLiteralExpression) override
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

    void VisitAstUnaryExpression(const AstUnaryExpression& unaryExpression) override
    {
        m_printer.Print("(");

        assert(unaryExpression.oprand);
        unaryExpression.oprand->Visit(*this);

        m_printer.Print(")");
    }

    void VisitAstVariableDeclaration(const AstVariableDeclaration& variableDeclaration) override
    {
        PrintTypeInfo(variableDeclaration.typeInfo);
        m_printer.Print(" ");
        m_printer.Print(variableDeclaration.name);
        m_printer.Print(" {{");
        if (variableDeclaration.initExpression) {
            m_printer.Print(" ");
            variableDeclaration.initExpression->Visit(*this);
            m_printer.Print(" ");
        }
        m_printer.Print("}}");
    }

    void VisitAstVariableDefinitionStatement(const AstVariableDefinitionStatement& variableDefinitionStatemet) override
    {
        VisitAstVariableDeclaration(variableDefinitionStatemet.variableDeclaration);
        m_printer.Println(";");
    }

private:
    void PrintTypeInfo(const AstTypeInfo& typeInfo)
    {
        m_printer.Print(typeInfo.fullName);
    }

    Printer m_printer;
};

}