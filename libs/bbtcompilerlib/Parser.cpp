#include "Parser.h"
#include <iterator>
#include <iostream>

namespace BBTCompiler
{
    Parser::Parser(std::vector<Token>& tokens)
        : m_Tokens{ tokens }, m_Current{ tokens.begin() }
    {
    }

    Token& Parser::advance()
    {
        if(!isAtEnd())
            std::advance(m_Current, 1);
        return previous();
    }

    Token& Parser::previous()
    {
        return m_Current != m_Tokens.begin() ? *std::prev(m_Current) : *m_Current;
    }

    Token& Parser::peek()
    {
        return *m_Current;
    }

    Token& Parser::consume(TokenType type, const std::string& errorMsg)
    {
        if(check(type))
            return advance();
        throw std::runtime_error(syntaxErrorMsg("file", peek(), errorMsg));
    }

    bool Parser::isAtEnd()
    {
        return m_Current->type == TokenType::END;
    }

    bool Parser::check(TokenType type)
    {
        return isAtEnd() ? false : peek().type == type;
    }

    bool Parser::match(const std::vector<TokenType>& types)
    {
        for (TokenType type : types)
        {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    Expr* Parser::parse()
    {
        try
        {
            return parseExpression();
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << '\n';
        }
        return nullptr;
    }

    Expr* Parser::parseExpression()
    {
        return parseEqualityExpr();
    }

    Expr* Parser::parseAssignmentExpr()
    {
        Expr* expr = parseComparisonExpr();
        while(match({}))
        {
        }
        return nullptr;
    }

    Expr* Parser::parseEqualityExpr()
    {
        Expr* expression{parseComparisonExpr()};
        while (match({ TokenType::NOT_EQ, TokenType::EQ_EQ }))
        {
            Token& op{ previous() };
            Expr* right{ parseComparisonExpr() };
            expression = new BinaryExpr(expression, op, right);
        }
        return expression;
    }

    Expr* Parser::parseComparisonExpr()
    {
        Expr* expression{parseAdditiveExpr()};
        while (match({ TokenType::GREATER, TokenType::LESS,TokenType::GREATER_EQ, TokenType::LESS_EQ }))
        {
            Token& op{ previous() };
            Expr* right{ parseAdditiveExpr() };
            expression = new BinaryExpr(expression, op, right);
        }
        return expression;
    }

    Expr* Parser::parseAdditiveExpr()
    {
        Expr* expression{parseMultiplicativeExpr()};
        while (match({ TokenType::MINUS, TokenType::PLUS }))
        {
            Token& op{ previous() };
            Expr* right{ parseMultiplicativeExpr() };
            expression = new BinaryExpr(expression, op, right);
        }
        return expression;
    }

    Expr* Parser::parseMultiplicativeExpr()
    {
        Expr* expression{parseUnaryExpr()};
        while (match({ TokenType::SLASH, TokenType::STAR }))
        {
            Token& op{ previous() };
            Expr* right{ parseUnaryExpr() };
            expression = new BinaryExpr(expression, op, right);
        }
        return expression;
    }

    Expr* Parser::parseUnaryExpr()
    {
        if(match({TokenType::NOT, TokenType::MINUS}))
        {
            Token& op{ previous() };
            Expr* right{ parseUnaryExpr() };
            return new UnaryExpr(op, right);
        }

        return parsePrimaryExpr();
    }

    Expr* Parser::parsePrimaryExpr()
    {
        if (match({
                TokenType::FALSE, TokenType::FALSE, TokenType::NIL, TokenType::IDENTIFIER,
                TokenType::INT_LITERAL, TokenType::FLOAT_LITERAL, TokenType::STRING_LITERAL
            }))
        {
            return new PrimaryExpr(previous());
        }

        if (match({ TokenType::LEFT_PAREN }))
        {
            Expr* expr = parseExpression();
            consume(TokenType::RIGHT_PAREN, "expected ')' after expression.");
            return new GroupedExpr(expr);
        }

        throw std::runtime_error(syntaxErrorMsg("file", peek(), "expected expression."));
    }

    std::string Parser::syntaxErrorMsg(const std::string& filename, const Token& token, const std::string& msg)
    {
        return "<"+filename+">:" +
                std::to_string(token.position.line) + ":" + std::to_string(token.position.column) +
                ": syntax error: " + msg;
    }
}