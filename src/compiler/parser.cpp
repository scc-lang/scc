module;

#include <cassert>
#include <memory>
#include <vector>

export module scc.compiler:parser;
import :ast_expression;
import :ast_expression_statement;
import :ast_function_call_expression;
import :ast_identifier_expression;
import :ast_string_literal_expression;
import :ast_scope;
import :ast_statement;
import :lexer;
import :source_range;

namespace scc::compiler {

export struct Parser {
    // compile_unit
    //  : /* empty */
    //  : compile_unit statement
    void ParseCompileUnit(std::shared_ptr<AstScope> scope, Lexer& lexer)
    {
        assert(scope);

        while (lexer.PeekToken().type != TOKEN_EOF) {
            ParseStatement(scope, lexer);
        }
    }

    // statement
    //  : /* empty statement */ ';'
    //  : expression_statement
    void ParseStatement(std::shared_ptr<AstScope>& scope, Lexer& lexer)
    {
        assert(scope);

        if (lexer.PeekToken().type == ';') {
            lexer.GetToken();
            return;
        }

        ParseExpressionStatement(scope, lexer);
    }

    // expression_statement
    //  : expression ';'
    void ParseExpressionStatement(std::shared_ptr<AstScope>& scope, Lexer& lexer)
    {
        assert(scope);

        auto expression = ParseExpression(scope, lexer);
        assert(expression);

        auto sourceRange = expression->sourceRange;

        auto lastToken = lexer.GetRequiredToken(';');

        sourceRange.endLine = lastToken.endLine;
        sourceRange.endColumn = lastToken.endColumn;

        scope->statements.push_back(std::make_unique<AstExpressionStatement>(std::move(sourceRange), std::move(expression)));
    }

    // expression
    //  : string_literal_expression
    //  | function_call_expression
    std::unique_ptr<AstExpression> ParseExpression(std::shared_ptr<AstScope>& scope, Lexer& lexer)
    {
        assert(scope);

        if (lexer.PeekToken().type == TOKEN_STRING) {
            return ParseStringLiteralExpression(scope, lexer);
        } else {
            return ParseFunctionCallExpression(scope, lexer);
        }
    }

    // function_call_expression
    //  : identifier_expression '(' ')'
    //  | identifier_expression '(' (expression ',')* expression ')'
    std::unique_ptr<AstExpression> ParseFunctionCallExpression(std::shared_ptr<AstScope>& scope, Lexer& lexer)
    {
        assert(scope);

        auto funcExpression = ParseIdentifierExpression(scope, lexer);
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
        sourceRange.endLine = endToken.endLine;
        sourceRange.endColumn = endToken.endColumn;

        return std::make_unique<AstFunctionCallExpression>(std::move(sourceRange), std::move(funcExpression), std::move(args));
    }

    // identifier_expression
    //  : (IDENTIFIER '::')* IDENTIFIER
    std::unique_ptr<AstExpression> ParseIdentifierExpression(std::shared_ptr<AstScope>& scope, Lexer& lexer)
    {
        assert(scope);

        auto token = lexer.GetRequiredToken(TOKEN_IDENTIFIER);
        auto sourceRange = SourceRange { token.startLine, token.startColumn, token.endLine, token.endColumn };
        auto fullName = std::string { std::move(token.string()) };

        while (lexer.PeekToken().type == TOKEN_SCOPE) {
            lexer.GetToken();
            token = lexer.GetRequiredToken(TOKEN_IDENTIFIER);
            sourceRange.endLine = token.endLine;
            sourceRange.endColumn = token.endColumn;
            fullName += "::";
            fullName += std::move(token.string());
        }

        return std::make_unique<AstIdentifierExpression>(std::move(sourceRange), std::move(fullName));
    }

    // string_literal_expression
    //  : TOKEN_STRING
    std::unique_ptr<AstExpression> ParseStringLiteralExpression(std::shared_ptr<AstScope>& scope, Lexer& lexer)
    {
        assert(scope);

        auto token = lexer.GetRequiredToken(TOKEN_STRING);
        return std::make_unique<AstStringLiteralExpression>(SourceRange { token.startLine, token.startColumn, token.endLine, token.endColumn }, std::move(token.string()));
    }
};

}