module;

#include <cassert>
#include <memory>
#include <optional>
#include <vector>

export module scc.compiler:parser;
import :ast_binary_expression;
import :ast_break_statement;
import :ast_conditional_statement;
import :ast_expression;
import :ast_expression_statement;
import :ast_for_loop_statement;
import :ast_function_call_expression;
import :ast_identifier_expression;
import :ast_integer_literal_expression;
import :ast_string_literal_expression;
import :ast_scope;
import :ast_statement;
import :ast_type_info;
import :ast_unary_expression;
import :ast_variable_declaration;
import :ast_variable_definition_statement;
import :exception;
import :lexer;
import :source_range;

namespace scc::compiler {

export struct Parser {
    // compile_unit
    //  : /* empty */
    //  : compile_unit statement
    void ParseCompileUnit(AstScope& scope, Lexer& lexer)
    {
        while (lexer.PeekToken().type != TOKEN_EOF) {
            ParseStatement(scope, lexer);
        }
    }

    // statement
    //  : /* empty statement */ ';'
    //  : declaration_or_expression_statement
    //  | for_loop_statement
    void ParseStatement(AstScope& scope, Lexer& lexer)
    {
        const auto& token = lexer.PeekToken();

        if (token.type == ';') {
            // Empty statement.
            lexer.GetToken();
            return;
        }

        switch (token.type) {
        case TOKEN_IDENTIFIER:
            // For a identifier token, this statement maybe an expression statement or
            // a variable declaration statement, depends on the value of the identifier.
            // If the identifier is a type identifier, then it is a variable declaration
            // statement, otherwise it is an expression statement.
            ParseVariableDeclarationOrExpressionStatement(scope, lexer);
            break;

        case TOKEN_FOR:
            ParseForStatement(scope, lexer);
            break;

        default:
            ParseExpressionStatement(scope, lexer);
            break;
        }
    }

    // variable_declaration_or_expression_statement
    //  : variable_declaration_statement
    //  | expression_statement
    void ParseVariableDeclarationOrExpressionStatement(AstScope& scope, Lexer& lexer)
    {
        // Parse the identifier expression.
        auto identifier = std::unique_ptr<AstIdentifierExpression>(static_cast<AstIdentifierExpression*>(ParseIdentifierExpression(scope, lexer).release()));
        assert(identifier);

        // Query the identifer in the scope.
        if (auto typeInfo = scope.QueryTypeInfo(identifier->fullName)) {
            ParseVariableDeclarationStatement(scope, lexer, std::move(identifier));
        } else {
            ParseExpressionStatement(scope, lexer, std::move(identifier));
        }
    }

    // variable_declaration_statement
    //  : (variable_declaration ',')* variable_declaration ';'
    //  : variable_declaration (',' IDENTIFIER ('=' expression)?)*
    void ParseVariableDeclarationStatement(AstScope& scope, Lexer& lexer, std::unique_ptr<AstIdentifierExpression> typeIdentifierExpression)
    {
        assert(typeIdentifierExpression);

        ParseVariableDeclaration(scope, lexer, std::move(typeIdentifierExpression));

        while (lexer.PeekToken().type == ',') {
            lexer.GetToken();

            if (lexer.PeekToken().type == TOKEN_IDENTIFIER && !scope.QueryTypeInfo(lexer.PeekToken().string())) {
                // The next token is identifier but not a type name, treat it as a variable with the same type.
                ParseVariableDeclarationWithType(scope, lexer, scope.variableDeclarations.back()->typeInfo);
            } else {
                // For all other cases, treat as a new variable declaration.
                ParseVariableDeclaration(scope, lexer);
            }
        }

        lexer.GetRequiredToken(';');
    }

    // variable_declaration
    //  : type_idenitifer IDENTIFIER ('=' expression)?
    void ParseVariableDeclaration(AstScope& scope, Lexer& lexer, std::unique_ptr<AstIdentifierExpression> typeIdentifierExpression = nullptr)
    {
        if (!typeIdentifierExpression) {
            typeIdentifierExpression.reset(static_cast<AstIdentifierExpression*>(ParseIdentifierExpression(scope, lexer).release()));
        }

        assert(typeIdentifierExpression);

        auto type = scope.QueryTypeInfo(typeIdentifierExpression->fullName);
        if (!type) {
            throw Exception { typeIdentifierExpression->sourceRange, "Undefined type '{}'", typeIdentifierExpression->fullName };
        }

        ParseVariableDeclarationWithType(scope, lexer, *type);
    }

    void ParseVariableDeclarationWithType(AstScope& scope, Lexer& lexer, AstTypeInfo& type)
    {
        auto identifier = lexer.GetRequiredToken(TOKEN_IDENTIFIER);
        auto sourceRange = identifier.sourceRange;

        auto initExpression = std::unique_ptr<AstExpression> {};
        if (lexer.PeekToken().type == '=') {
            lexer.GetToken();
            initExpression = ParseExpression(scope, lexer);
            sourceRange.endLine = initExpression->sourceRange.endLine;
            sourceRange.endColumn = initExpression->sourceRange.endColumn;
        } else {
            sourceRange.endLine = identifier.sourceRange.endLine;
            sourceRange.endColumn = identifier.sourceRange.endColumn;
        }

        scope.variableDeclarations.push_back(std::make_unique<AstVariableDeclaration>(sourceRange, type, std::move(identifier.string()), std::move(initExpression)));
        scope.statements.push_back(std::make_unique<AstVariableDefinitionStatement>(std::move(sourceRange), *scope.variableDeclarations.back()));
    }

    // expression_statement
    //  : expression ';'
    void ParseExpressionStatement(AstScope& scope, Lexer& lexer, std::unique_ptr<AstIdentifierExpression> preExpression = nullptr)
    {
        auto expression = ParseExpression(scope, lexer, std::move(preExpression));
        assert(expression);

        auto sourceRange = expression->sourceRange;

        auto lastToken = lexer.GetRequiredToken(';');

        sourceRange.endLine = lastToken.sourceRange.endLine;
        sourceRange.endColumn = lastToken.sourceRange.endColumn;

        scope.statements.push_back(std::make_unique<AstExpressionStatement>(std::move(sourceRange), std::move(expression)));
    }

    // expression
    //  : assignment_expression
    std::unique_ptr<AstExpression> ParseExpression(AstScope& scope, Lexer& lexer, std::unique_ptr<AstIdentifierExpression> preExpression = nullptr)
    {
        return ParseAssignmentExpression(scope, lexer, std::move(preExpression));
    }

    // assignment_expression
    //  : relational_expression
    //  | primary_expression ['='|'*='|'/='|'%='|'+='|'-='|'<<='|'>>='|'&='|'^='|'|='] assignment_expression
    std::unique_ptr<AstExpression> ParseAssignmentExpression(AstScope& scope, Lexer& lexer, std::unique_ptr<AstIdentifierExpression> preExpression = nullptr)
    {
        auto expression = ParseRelationalExpression(scope, lexer, std::move(preExpression));
        assert(expression);

        std::optional<scc::compiler::BinaryOp> op;
        switch (lexer.PeekToken().type) {
        case '=':
            op = BinaryOp::Assignment;
            break;
        case TOKEN_MUL_ASSIGNMENT:
            op = BinaryOp::MulAssignment;
            break;
        case TOKEN_DIV_ASSIGNMENT:
            op = BinaryOp::DivAssignment;
            break;
        case TOKEN_MOD_ASSIGNMENT:
            op = BinaryOp::ModAssignment;
            break;
        case TOKEN_ADD_ASSIGNMENT:
            op = BinaryOp::AddAssignment;
            break;
        case TOKEN_SUB_ASSIGNMENT:
            op = BinaryOp::SubAssignment;
            break;
        case TOKEN_SHIFT_LEFT_ASSIGNMENT:
            op = BinaryOp::ShiftLeftAssignment;
            break;
        case TOKEN_SHIFT_RIGHT_ASSIGNMENT:
            op = BinaryOp::ShiftRightAssignment;
            break;
        case TOKEN_BIT_AND_ASSIGNMENT:
            op = BinaryOp::BitAndAssignment;
            break;
        case TOKEN_BIT_XOR_ASSIGNMENT:
            op = BinaryOp::BitXorAssignment;
            break;
        case TOKEN_BIT_OR_ASSIGNMENT:
            op = BinaryOp::BitOrAssignment;
            break;
        }
        if (op) {
            lexer.GetToken();
            auto leftOprand = std::move(expression);
            auto rightOprand = ParseAssignmentExpression(scope, lexer);
            auto sourceRange = SourceRange { leftOprand->sourceRange, rightOprand->sourceRange };
            expression = std::make_unique<AstBinaryExpression>(std::move(sourceRange), std::move(leftOprand), *op, std::move(rightOprand));
        }
        return std::move(expression);
    }

    // relational_expression
    //  : additive_expression
    //  | relational_expression ['<'|'>'|'<='|'>='] additive_expression
    std::unique_ptr<AstExpression> ParseRelationalExpression(AstScope& scope, Lexer& lexer, std::unique_ptr<AstIdentifierExpression> preExpression = nullptr)
    {
        auto expression = ParseAdditiveExpression(scope, lexer, std::move(preExpression));
        assert(expression);

        while (true) {
            std::optional<scc::compiler::BinaryOp> op;
            switch (lexer.PeekToken().type) {
            case '<':
                op = BinaryOp::Less;
                break;
            case '>':
                op = BinaryOp::Greater;
                break;
            case TOKEN_LESS_EQUAL:
                op = BinaryOp::LessEqual;
                break;
            case TOKEN_GREATER_EQUAL:
                op = BinaryOp::GreaterEqual;
                break;
            }
            if (op) {
                lexer.GetToken();
                auto leftOprand = std::move(expression);
                auto rightOperand = ParseAdditiveExpression(scope, lexer);
                auto sourceRange = SourceRange { leftOprand->sourceRange, rightOperand->sourceRange };
                expression = std::make_unique<AstBinaryExpression>(std::move(sourceRange), std::move(leftOprand), *op, std::move(rightOperand));
            } else {
                break;
            }
        }
        return std::move(expression);
    }

    // additive_expression
    //  : multiplicative_expression
    //  | additive_expression '+' multiplicative_expression
    //  | additive_expression '-' multiplicative_expression
    std::unique_ptr<AstExpression> ParseAdditiveExpression(AstScope& scope, Lexer& lexer, std::unique_ptr<AstIdentifierExpression> preExpression = nullptr)
    {
        auto expression = ParseMultiplicativeExpression(scope, lexer, std::move(preExpression));
        assert(expression);

        while (true) {
            std::optional<scc::compiler::BinaryOp> op;
            switch (lexer.PeekToken().type) {
            case '+':
                op = BinaryOp::Add;
                break;
            case '-':
                op = BinaryOp::Sub;
                break;
            }
            if (op) {
                lexer.GetToken();
                auto leftOprand = std::move(expression);
                auto rightOperand = ParseMultiplicativeExpression(scope, lexer);
                auto sourceRange = SourceRange { leftOprand->sourceRange, rightOperand->sourceRange };
                expression = std::make_unique<AstBinaryExpression>(std::move(sourceRange), std::move(leftOprand), *op, std::move(rightOperand));
            } else {
                break;
            }
        }
        return std::move(expression);
    }

    // multiplicative_expression
    //  : primary_expression
    //  | multiplicative_expression '*' primary_expression
    //  | multiplicative_expression '/' primary_expression
    //  | multiplicative_expression '%' primary_expression
    std::unique_ptr<AstExpression> ParseMultiplicativeExpression(AstScope& scope, Lexer& lexer, std::unique_ptr<AstIdentifierExpression> preExpression = nullptr)
    {
        auto expression = ParsePrimaryExpression(scope, lexer, std::move(preExpression));
        assert(expression);

        while (true) {
            std::optional<scc::compiler::BinaryOp> op;
            switch (lexer.PeekToken().type) {
            case '*':
                op = BinaryOp::Mul;
                break;
            case '/':
                op = BinaryOp::Div;
                break;
            case '%':
                op = BinaryOp::Mod;
                break;
            }
            if (op) {
                lexer.GetToken();
                auto leftOprand = std::move(expression);
                auto rightOperand = ParsePrimaryExpression(scope, lexer);
                auto sourceRange = SourceRange { leftOprand->sourceRange, rightOperand->sourceRange };
                expression = std::make_unique<AstBinaryExpression>(std::move(sourceRange), std::move(leftOprand), *op, std::move(rightOperand));
            } else {
                break;
            }
        }
        return std::move(expression);
    }

    // primary_expression
    //  : identifier_expression
    //  | function_call_expression
    //  | integer_literal_expression
    //  | string_literal_expression
    //  | '(' expression ')'
    std::unique_ptr<AstExpression> ParsePrimaryExpression(AstScope& scope, Lexer& lexer, std::unique_ptr<AstIdentifierExpression> preExpression = nullptr)
    {
        if (preExpression) {
            return ParseFunctionCallExpression(scope, lexer, std::move(preExpression));
        } else if (lexer.PeekToken().type == TOKEN_INTEGER) {
            return ParseIntegerLiteralExpression(scope, lexer);
        } else if (lexer.PeekToken().type == TOKEN_STRING) {
            return ParseStringLiteralExpression(scope, lexer);
        } else if (lexer.PeekToken().type == '(') {
            lexer.GetRequiredToken('(');
            auto expression = ParseExpression(scope, lexer);
            lexer.GetRequiredToken(')');
            return std::make_unique<AstUnaryExpression>(expression->sourceRange, UnaryOp::Bracket, std::move(expression));
        } else {
            return ParseFunctionCallExpression(scope, lexer);
        }
    }

    // function_call_expression
    //  : identifier_expression
    //  | identifier_expression '(' ')'
    //  | identifier_expression '(' (expression ',')* expression ')'
    std::unique_ptr<AstExpression> ParseFunctionCallExpression(AstScope& scope, Lexer& lexer, std::unique_ptr<AstIdentifierExpression> preExpression = nullptr)
    {
        auto funcExpression = preExpression ? std::unique_ptr<AstExpression> { preExpression.release() } : ParseIdentifierExpression(scope, lexer);
        if (lexer.PeekToken().type != '(') {
            return std::move(funcExpression);
        } else {
            auto sourceRange = funcExpression->sourceRange;

            lexer.GetRequiredToken('(');

            std::vector<std::unique_ptr<AstExpression>> args {};
            while (lexer.PeekToken().type != ')') {
                if (!args.empty()) {
                    lexer.GetRequiredToken(',');
                }
                args.push_back(ParseExpression(scope, lexer));
            }

            auto endToken = lexer.GetRequiredToken(')');
            sourceRange.endLine = endToken.sourceRange.endLine;
            sourceRange.endColumn = endToken.sourceRange.endColumn;

            return std::make_unique<AstFunctionCallExpression>(std::move(sourceRange), std::move(funcExpression), std::move(args));
        }
    }

    // identifier_expression
    //  : (IDENTIFIER '::')* IDENTIFIER
    std::unique_ptr<AstExpression> ParseIdentifierExpression(AstScope& scope, Lexer& lexer)
    {
        auto token = lexer.GetRequiredToken(TOKEN_IDENTIFIER);
        auto sourceRange = std::move(token.sourceRange);
        auto fullName = std::string { std::move(token.string()) };

        while (lexer.PeekToken().type == TOKEN_SCOPE) {
            lexer.GetToken();
            token = lexer.GetRequiredToken(TOKEN_IDENTIFIER);
            sourceRange.endLine = token.sourceRange.endLine;
            sourceRange.endColumn = token.sourceRange.endColumn;
            fullName += "::";
            fullName += std::move(token.string());
        }

        return std::make_unique<AstIdentifierExpression>(std::move(sourceRange), std::move(fullName));
    }

    // for_statement
    //  : FOR '(' declaration_or_expression_statement expression? ';' expression ')' '{' statements* '}'
    //  : FOR '(' expression? ';' expression? ';' expression ')' '{' statements* '}'
    void ParseForStatement(AstScope& scope, Lexer& lexer)
    {
        const auto& startToken = lexer.GetRequiredToken(TOKEN_FOR);

        // Parse header.
        auto forInitScope = AstScope { &scope };
        lexer.GetRequiredToken('(');
        if (lexer.PeekToken().type != ';') {
            ParseVariableDeclarationOrExpressionStatement(forInitScope, lexer);
        } else {
            lexer.GetRequiredToken(';');
        }

        auto conditionalExpression = std::unique_ptr<AstExpression> {};
        if (lexer.PeekToken().type != ';') {
            conditionalExpression = ParseExpression(forInitScope, lexer);
        }
        lexer.GetRequiredToken(';');

        auto iterationExpression = std::unique_ptr<AstExpression> {};
        if (lexer.PeekToken().type != ')') {
            iterationExpression = ParseExpression(forInitScope, lexer);
        }
        lexer.GetRequiredToken(')');

        // Parse body.
        auto forBodyScope = AstScope { &forInitScope };
        lexer.GetRequiredToken('{');
        while (lexer.PeekToken().type != '}') {
            ParseStatement(forBodyScope, lexer);
        }

        const auto& lastToken = lexer.GetRequiredToken('}');

        // Finish for statement parsing, add to parent scope.
        scope.statements.push_back(std::make_unique<AstForLoopStatement>(
            SourceRange { startToken.sourceRange, lastToken.sourceRange }, std::move(forInitScope), std::move(conditionalExpression), std::move(iterationExpression), std::move(forBodyScope)));
    }

    // integer_literal_expression
    //  : TOKEN_INTEGER
    std::unique_ptr<AstExpression> ParseIntegerLiteralExpression(AstScope& scope, Lexer& lexer)
    {
        auto token = lexer.GetRequiredToken(TOKEN_INTEGER);
        return std::make_unique<AstIntegerLiteralExpression>(std::move(token.sourceRange), token.integer());
    }

    // string_literal_expression
    //  : TOKEN_STRING
    std::unique_ptr<AstExpression> ParseStringLiteralExpression(AstScope& scope, Lexer& lexer)
    {
        auto token = lexer.GetRequiredToken(TOKEN_STRING);
        return std::make_unique<AstStringLiteralExpression>(std::move(token.sourceRange), std::move(token.string()));
    }
};

}