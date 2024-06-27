module;

export module scc.ast:ast_visitor;

namespace scc::ast {

export struct AstBinaryExpression;
export struct AstBreakStatement;
export struct AstConditionalStatement;
export struct AstExpressionStatement;
export struct AstForLoopStatement;
export struct AstFunctionCallExpression;
export struct AstIdentifierExpression;
export struct AstIntegerLiteralExpression;
export struct AstScope;
export struct AstStringLiteralExpression;
export struct AstUnaryExpression;
export struct AstVariableDeclaration;
export struct AstVariableDefinitionStatement;

export struct AstVisitor {
    virtual ~AstVisitor() = default;

    virtual void VisitAstBinaryExpression(const AstBinaryExpression& binaryExpression) = 0;
    virtual void VisitAstBreakStatement(const AstBreakStatement& breakStatement) = 0;
    virtual void VisitAstConditionalStatement(const AstConditionalStatement& conditionalStatement) = 0;
    virtual void VisitAstExpressionStatement(const AstExpressionStatement& expressionStatement) = 0;
    virtual void VisitAstForLoopStatement(const AstForLoopStatement& forLoopStatement) = 0;
    virtual void VisitAstFunctionCallExpression(const AstFunctionCallExpression& functionCallExpression) = 0;
    virtual void VisitAstIdentifierExpression(const AstIdentifierExpression& identifierExpression) = 0;
    virtual void VisitAstIntegerLiteralExpression(const AstIntegerLiteralExpression& integerLiteralExpression) = 0;
    virtual void VisitAstScope(const AstScope& scope) = 0;
    virtual void VisitAstStringLiteralExpression(const AstStringLiteralExpression& stringLiteralExpression) = 0;
    virtual void VisitAstUnaryExpression(const AstUnaryExpression& unaryExpression) = 0;
    virtual void VisitAstVariableDeclaration(const AstVariableDeclaration& variableDeclaration) = 0;
    virtual void VisitAstVariableDefinitionStatement(const AstVariableDefinitionStatement& variableDefinitionStatemet) = 0;
};

}