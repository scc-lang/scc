module;

#include <cassert>
#include <memory>
#include <optional>
#include <vector>

import scc.ast;

export module scc.compiler:parser;
import :exception;
import :lexer;

namespace scc::compiler {

using namespace ast;

export struct Parser {
    // compile_unit
    //  : /* empty */
    //  : compile_unit statement
    void ParseCompileUnit(Scope& scope, Lexer& lexer)
    {
        while (lexer.PeekToken().type != TOKEN_EOF) {
            ParseStatement(scope, lexer);
        }
    }

    // statement
    //  : /* empty statement */ ';'
    //  : declaration_or_expression_statement
    //  | for_loop_statement
    //  | if_statement
    void ParseStatement(Scope& scope, Lexer& lexer)
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

        case TOKEN_IF:
            ParseIfStatement(scope, lexer);
            break;

        default:
            ParseExpressionStatement(scope, lexer);
            break;
        }
    }

    // variable_declaration_or_expression_statement
    //  : variable_declaration_statement
    //  | expression_statement
    void ParseVariableDeclarationOrExpressionStatement(Scope& scope, Lexer& lexer)
    {
        // Parse the identifier expression.
        auto identifier = std::unique_ptr<IdentifierExpression>(static_cast<IdentifierExpression*>(ParseIdentifierExpression(scope, lexer).release()));
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
    void ParseVariableDeclarationStatement(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> typeIdentifierExpression)
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
    void ParseVariableDeclaration(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> typeIdentifierExpression = nullptr)
    {
        if (!typeIdentifierExpression) {
            typeIdentifierExpression.reset(static_cast<IdentifierExpression*>(ParseIdentifierExpression(scope, lexer).release()));
        }

        assert(typeIdentifierExpression);

        auto type = scope.QueryTypeInfo(typeIdentifierExpression->fullName);
        if (!type) {
            throw Exception { typeIdentifierExpression->sourceRange, "Undefined type '{}'", typeIdentifierExpression->fullName };
        }

        ParseVariableDeclarationWithType(scope, lexer, *type);
    }

    void ParseVariableDeclarationWithType(Scope& scope, Lexer& lexer, TypeInfo& type)
    {
        auto identifier = lexer.GetRequiredToken(TOKEN_IDENTIFIER);
        auto sourceRange = identifier.sourceRange;

        auto initExpression = std::unique_ptr<Expression> {};
        if (lexer.PeekToken().type == '=') {
            lexer.GetToken();
            initExpression = ParseExpression(scope, lexer);
            sourceRange.endLine = initExpression->sourceRange.endLine;
            sourceRange.endColumn = initExpression->sourceRange.endColumn;
        } else {
            sourceRange.endLine = identifier.sourceRange.endLine;
            sourceRange.endColumn = identifier.sourceRange.endColumn;
        }

        scope.variableDeclarations.push_back(std::make_unique<VariableDeclaration>(sourceRange, type, std::move(identifier.string()), std::move(initExpression)));
        scope.statements.push_back(std::make_unique<VariableDefinitionStatement>(std::move(sourceRange), *scope.variableDeclarations.back()));
    }

    // expression_statement
    //  : expression ';'
    void ParseExpressionStatement(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> preExpression = nullptr)
    {
        auto expression = ParseExpression(scope, lexer, std::move(preExpression));
        assert(expression);

        auto sourceRange = expression->sourceRange;

        auto lastToken = lexer.GetRequiredToken(';');

        sourceRange.endLine = lastToken.sourceRange.endLine;
        sourceRange.endColumn = lastToken.sourceRange.endColumn;

        scope.statements.push_back(std::make_unique<ExpressionStatement>(std::move(sourceRange), std::move(expression)));
    }

    // expression
    //  : assignment_expression
    std::unique_ptr<Expression> ParseExpression(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> preExpression = nullptr)
    {
        return ParseAssignmentExpression(scope, lexer, std::move(preExpression));
    }

    // assignment_expression
    //  : equality_expression
    //  | primary_expression ['='|'*='|'/='|'%='|'+='|'-='|'<<='|'>>='|'&='|'^='|'|='] assignment_expression
    std::unique_ptr<Expression> ParseAssignmentExpression(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> preExpression = nullptr)
    {
        auto expression = ParseEqualityExpression(scope, lexer, std::move(preExpression));
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
            expression = std::make_unique<BinaryExpression>(std::move(sourceRange), std::move(leftOprand), *op, std::move(rightOprand));
        }
        return std::move(expression);
    }

    // equality_expression
    //  : relational_expression
    //  | equality_expression '==' relational_expression
    //  | equality_expression '!=' relational_expression
    std::unique_ptr<Expression> ParseEqualityExpression(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> preExpression = nullptr)
    {
        auto expression = ParseRelationalExpression(scope, lexer, std::move(preExpression));
        assert(expression);

        while (true) {
            std::optional<scc::compiler::BinaryOp> op;
            switch (lexer.PeekToken().type) {
            case TOKEN_EQUAL:
                op = BinaryOp::Equal;
                break;
            case TOKEN_NOT_EQUAL:
                op = BinaryOp::NotEqual;
                break;
            }
            if (op) {
                lexer.GetToken();
                auto leftOprand = std::move(expression);
                auto rightOperand = ParseRelationalExpression(scope, lexer);
                auto sourceRange = SourceRange { leftOprand->sourceRange, rightOperand->sourceRange };
                expression = std::make_unique<BinaryExpression>(std::move(sourceRange), std::move(leftOprand), *op, std::move(rightOperand));
            } else {
                break;
            }
        }
        return std::move(expression);
    }

    // relational_expression
    //  : additive_expression
    //  | relational_expression ['<'|'>'|'<='|'>='] additive_expression
    std::unique_ptr<Expression> ParseRelationalExpression(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> preExpression = nullptr)
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
                expression = std::make_unique<BinaryExpression>(std::move(sourceRange), std::move(leftOprand), *op, std::move(rightOperand));
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
    std::unique_ptr<Expression> ParseAdditiveExpression(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> preExpression = nullptr)
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
                expression = std::make_unique<BinaryExpression>(std::move(sourceRange), std::move(leftOprand), *op, std::move(rightOperand));
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
    std::unique_ptr<Expression> ParseMultiplicativeExpression(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> preExpression = nullptr)
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
                expression = std::make_unique<BinaryExpression>(std::move(sourceRange), std::move(leftOprand), *op, std::move(rightOperand));
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
    std::unique_ptr<Expression> ParsePrimaryExpression(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> preExpression = nullptr)
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
            return std::make_unique<UnaryExpression>(expression->sourceRange, UnaryOp::Bracket, std::move(expression));
        } else {
            return ParseFunctionCallExpression(scope, lexer);
        }
    }

    // function_call_expression
    //  : identifier_expression
    //  | identifier_expression '(' ')'
    //  | identifier_expression '(' (expression ',')* expression ')'
    std::unique_ptr<Expression> ParseFunctionCallExpression(Scope& scope, Lexer& lexer, std::unique_ptr<IdentifierExpression> preExpression = nullptr)
    {
        auto funcExpression = preExpression ? std::unique_ptr<Expression> { preExpression.release() } : ParseIdentifierExpression(scope, lexer);
        if (lexer.PeekToken().type != '(') {
            return std::move(funcExpression);
        } else {
            auto sourceRange = funcExpression->sourceRange;

            lexer.GetRequiredToken('(');

            std::vector<std::unique_ptr<Expression>> args {};
            while (lexer.PeekToken().type != ')') {
                if (!args.empty()) {
                    lexer.GetRequiredToken(',');
                }
                args.push_back(ParseExpression(scope, lexer));
            }

            auto endToken = lexer.GetRequiredToken(')');
            sourceRange.endLine = endToken.sourceRange.endLine;
            sourceRange.endColumn = endToken.sourceRange.endColumn;

            return std::make_unique<FunctionCallExpression>(std::move(sourceRange), std::move(funcExpression), std::move(args));
        }
    }

    // identifier_expression
    //  : (IDENTIFIER '::')* IDENTIFIER
    std::unique_ptr<Expression> ParseIdentifierExpression(Scope& scope, Lexer& lexer)
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

        return std::make_unique<IdentifierExpression>(std::move(sourceRange), std::move(fullName));
    }

    // for_statement
    //  : FOR '(' declaration_or_expression_statement expression? ';' expression ')' '{' statements* '}'
    //  : FOR '(' expression? ';' expression? ';' expression ')' '{' statements* '}'
    void ParseForStatement(Scope& scope, Lexer& lexer)
    {
        const auto& startToken = lexer.GetRequiredToken(TOKEN_FOR);

        // Parse header.
        auto forInitScope = Scope { &scope };
        lexer.GetRequiredToken('(');
        if (lexer.PeekToken().type != ';') {
            ParseVariableDeclarationOrExpressionStatement(forInitScope, lexer);
        } else {
            lexer.GetRequiredToken(';');
        }

        auto conditionalExpression = std::unique_ptr<Expression> {};
        if (lexer.PeekToken().type != ';') {
            conditionalExpression = ParseExpression(forInitScope, lexer);
        }
        lexer.GetRequiredToken(';');

        auto iterationExpression = std::unique_ptr<Expression> {};
        if (lexer.PeekToken().type != ')') {
            iterationExpression = ParseExpression(forInitScope, lexer);
        }
        lexer.GetRequiredToken(')');

        // Parse body.
        auto forBodyScope = Scope { &forInitScope };
        lexer.GetRequiredToken('{');
        while (lexer.PeekToken().type != '}') {
            ParseStatement(forBodyScope, lexer);
        }

        const auto& lastToken = lexer.GetRequiredToken('}');

        // Finish for statement parsing, add to parent scope.
        scope.statements.push_back(std::make_unique<ForLoopStatement>(
            SourceRange { startToken.sourceRange, lastToken.sourceRange }, std::move(forInitScope), std::move(conditionalExpression), std::move(iterationExpression), std::move(forBodyScope)));
    }

    // if_statement
    //  : TOKEN_IF '(' expression ')' '{' statements* '}'
    //  | TOKEN_IF '(' expression ')' '{' statements* '}' ELSE '{' statements* '}'
    //  : TOKEN_IF '(' expression ')' '{' statements* '}' TOKEN_ELSE if_statement
    void ParseIfStatement(Scope& scope, Lexer& lexer)
    {
        auto startSourceRange = lexer.GetRequiredToken(TOKEN_IF).sourceRange;

        lexer.GetRequiredToken('(');
        auto conditionalExpression = ParseExpression(scope, lexer);
        lexer.GetRequiredToken(')');

        auto trueScope = Scope { &scope };
        lexer.GetRequiredToken('{');
        while (lexer.PeekToken().type != '}') {
            ParseStatement(trueScope, lexer);
        }
        auto lastSourceRange = lexer.GetRequiredToken('}').sourceRange;

        auto falseScope = Scope { &scope };
        if (lexer.PeekToken().type == TOKEN_ELSE) {
            lexer.GetToken();
            if (lexer.PeekToken().type == TOKEN_IF) {
                ParseIfStatement(falseScope, lexer);
                lastSourceRange = falseScope.statements.back()->sourceRange;
            } else {
                lexer.GetRequiredToken('{');
                while (lexer.PeekToken().type != '}') {
                    ParseStatement(falseScope, lexer);
                }
                lastSourceRange = lexer.GetRequiredToken('}').sourceRange;
            }
        }

        scope.statements.push_back(std::make_unique<ConditionalStatement>(SourceRange { startSourceRange, lastSourceRange }, std::move(conditionalExpression), std::move(trueScope), std::move(falseScope)));
    }

    // integer_literal_expression
    //  : TOKEN_INTEGER
    std::unique_ptr<Expression> ParseIntegerLiteralExpression(Scope& scope, Lexer& lexer)
    {
        auto token = lexer.GetRequiredToken(TOKEN_INTEGER);
        return std::make_unique<IntegerLiteralExpression>(std::move(token.sourceRange), token.integer());
    }

    // string_literal_expression
    //  : TOKEN_STRING
    std::unique_ptr<Expression> ParseStringLiteralExpression(Scope& scope, Lexer& lexer)
    {
        auto token = lexer.GetRequiredToken(TOKEN_STRING);
        return std::make_unique<StringLiteralExpression>(std::move(token.sourceRange), std::move(token.string()));
    }
};

}