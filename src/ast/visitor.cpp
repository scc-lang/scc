module;

export module scc.ast:ast_visitor;

namespace scc::ast {

export struct BinaryExpression;
export struct BreakStatement;
export struct ConditionalStatement;
export struct ExpressionStatement;
export struct ForLoopStatement;
export struct FunctionCallExpression;
export struct IdentifierExpression;
export struct IntegerLiteralExpression;
export struct Scope;
export struct StringLiteralExpression;
export struct UnaryExpression;
export struct VariableDeclaration;
export struct VariableDefinitionStatement;

export struct Visitor {
    virtual ~Visitor() = default;

    virtual void VisitAstBinaryExpression(const BinaryExpression& binaryExpression) = 0;
    virtual void VisitAstBreakStatement(const BreakStatement& breakStatement) = 0;
    virtual void VisitAstConditionalStatement(const ConditionalStatement& conditionalStatement) = 0;
    virtual void VisitAstExpressionStatement(const ExpressionStatement& expressionStatement) = 0;
    virtual void VisitAstForLoopStatement(const ForLoopStatement& forLoopStatement) = 0;
    virtual void VisitAstFunctionCallExpression(const FunctionCallExpression& functionCallExpression) = 0;
    virtual void VisitAstIdentifierExpression(const IdentifierExpression& identifierExpression) = 0;
    virtual void VisitAstIntegerLiteralExpression(const IntegerLiteralExpression& integerLiteralExpression) = 0;
    virtual void VisitAstScope(const Scope& scope) = 0;
    virtual void VisitAstStringLiteralExpression(const StringLiteralExpression& stringLiteralExpression) = 0;
    virtual void VisitAstUnaryExpression(const UnaryExpression& unaryExpression) = 0;
    virtual void VisitAstVariableDeclaration(const VariableDeclaration& variableDeclaration) = 0;
    virtual void VisitAstVariableDefinitionStatement(const VariableDefinitionStatement& variableDefinitionStatemet) = 0;
};

}