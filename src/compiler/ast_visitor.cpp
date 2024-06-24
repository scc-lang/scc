module;

export module scc.compiler:ast_visitor;

namespace scc::compiler {

export struct AstExpressionStatement;
export struct AstFunctionCallExpression;
export struct AstIdentifierExpression;
export struct AstScope;
export struct AstStringLiteralExpression;
export struct AstVariableDeclaration;

export struct AstVisitor {
    virtual ~AstVisitor() = default;

    virtual void VisitAstScope(const AstScope& scope) = 0;
    virtual void VisitAstExpressionStatement(const AstExpressionStatement& expressionStatement) = 0;
    virtual void VisitAstFunctionCallExpression(const AstFunctionCallExpression& functionCallExpression) = 0;
    virtual void VisitAstIdentifierExpression(const AstIdentifierExpression& identifierExpression) = 0;
    virtual void VisitAstStringLiteralExpression(const AstStringLiteralExpression& stringLiteralExpression) = 0;
    virtual void VisitAstVariableDeclaration(const AstVariableDeclaration&) = 0;
};

}