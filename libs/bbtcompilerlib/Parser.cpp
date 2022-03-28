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

    bool Parser::match(const TokenType& type)
    {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    bool Parser::match(const std::vector<TokenType>& types)
    {
        for (TokenType type : types)
        {
            if (match(type))
                return true;
        }
        return false;
    }

    std::vector<std::unique_ptr<Stmt>>& Parser::parse()
    {
        while(!isAtEnd())
        {
            if(std::unique_ptr<Stmt> statement{ parseDeclaration() })
                m_Statements.push_back(std::move(statement));
        }
        return m_Statements;
    }

    std::vector<std::unique_ptr<Stmt>> Parser::parseBlock()
    {
        std::vector<std::unique_ptr<Stmt>> statements;
        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd())
            statements.push_back(std::move(parseDeclaration()));
        
        consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
        return statements;
    }

    std::unique_ptr<Stmt> Parser::parseDeclaration()
    {
        try
        {
            if(match(TokenType::LET))
                return parseVariableDeclaration();
            return parseStatement();
        }
        catch (const std::exception& e)
        {
            synchronize();
            std::cerr << e.what();
        }
        return nullptr;
    }
    
    std::unique_ptr<Stmt> Parser::parseVariableDeclaration()
    {
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

        std::unique_ptr<Expr> initializer{ match(TokenType::EQ) ? parseExpression() : nullptr };

        consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
        return std::make_unique<VariableStmt>(VariableStmt{ name, std::move(initializer) });
    }

    std::unique_ptr<Stmt> Parser::parseStatement()
    {
        if(match(TokenType::IF)) return parseIfStatement();
        if(match(TokenType::PRINT)) return parsePrintStatement();
        if(match(TokenType::LEFT_BRACE)) return std::make_unique<BlockStmt>(BlockStmt{parseBlock()});
        return parseExpressionStatement();
    }

    std::unique_ptr<Stmt> Parser::parseExpressionStatement()
    {
        auto expression = parseExpression();
        consume(TokenType::SEMICOLON, "Expect ';' after value.");
        return std::make_unique<ExprStmt>(expression.release());
    }

    std::unique_ptr<Stmt> Parser::parsePrintStatement()
    {
        std::unique_ptr<Expr> expression{ parseExpression() };
        consume(TokenType::SEMICOLON, "Expect ';' after value.");
        return std::make_unique<PrintStmt>(PrintStmt{ std::move(expression) });
    }

    std::unique_ptr<Stmt> Parser::parseIfStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        auto condition = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

        auto thenBranch = parseStatement();
        std::unique_ptr<Stmt> elseBranch{ match(TokenType::ELSE) ? parseStatement() : nullptr };
        return std::make_unique<IfStmt>(IfStmt{ std::move(condition), std::move(thenBranch), std::move(elseBranch) });
    }

    std::unique_ptr<Expr> Parser::parseExpression()
    {
        return parseAssignmentExpr();
    }

    std::unique_ptr<Expr> Parser::parseAssignmentExpr()
    {
        auto expr = parseEqualityExpr();
        if(match(TokenType::EQ))
        {
            Token equals = previous();
            auto value = parseAssignmentExpr();

            if(auto varExpr = dynamic_cast<VariableExpr*>(expr.get()))
            {
                Token name = varExpr->m_Name;
                return std::make_unique<AssignmentExpr>(AssignmentExpr(name, value.release()));
            }

            throw std::runtime_error(syntaxErrorMsg("file", equals, "Invalid assignment target."));
        }
        return std::move(expr);
    }

    std::unique_ptr<Expr> Parser::parseEqualityExpr()
    {
        std::unique_ptr<Expr> expression{parseComparisonExpr()};
        while (match({ TokenType::NOT_EQ, TokenType::EQ_EQ }))
        {
            Token& op{ previous() };
            std::unique_ptr<Expr> right{ parseComparisonExpr() };
            expression = std::make_unique<BinaryExpr>(BinaryExpr(expression.release(), op, right.release()));
        }
        return expression;
    }

    std::unique_ptr<Expr> Parser::parseComparisonExpr()
    {
        std::unique_ptr<Expr> expression{parseAdditiveExpr()};
        while (match({ TokenType::GREATER, TokenType::LESS,TokenType::GREATER_EQ, TokenType::LESS_EQ }))
        {
            Token& op{ previous() };
            std::unique_ptr<Expr> right{ parseAdditiveExpr() };
            expression = std::make_unique<BinaryExpr>(BinaryExpr(expression.release(), op, right.release()));
        }
        return expression;
    }

    std::unique_ptr<Expr> Parser::parseAdditiveExpr()
    {
        std::unique_ptr<Expr> expression{parseMultiplicativeExpr()};
        while (match({ TokenType::MINUS, TokenType::PLUS }))
        {
            Token& op{ previous() };
            std::unique_ptr<Expr> right{ parseMultiplicativeExpr() };
            expression = std::make_unique<BinaryExpr>(BinaryExpr(expression.release(), op, right.release()));
        }
        return expression;
    }

    std::unique_ptr<Expr> Parser::parseMultiplicativeExpr()
    {
        std::unique_ptr<Expr> expression{parseUnaryExpr()};
        while (match({ TokenType::SLASH, TokenType::STAR }))
        {
            Token& op{ previous() };
            std::unique_ptr<Expr> right{ parseUnaryExpr() };
            expression = std::make_unique<BinaryExpr>(BinaryExpr(expression.release(), op, right.release()));
        }
        return expression;
    }

    std::unique_ptr<Expr> Parser::parseUnaryExpr()
    {
        if(match({TokenType::NOT, TokenType::MINUS}))
        {
            Token& op{ previous() };
            std::unique_ptr<Expr> right{ parseUnaryExpr() };
            return std::make_unique<UnaryExpr>(UnaryExpr(op, right.release()));
        }

        return parsePrimaryExpr();
    }

    std::unique_ptr<Expr> Parser::parsePrimaryExpr()
    {
        if (match({
                TokenType::TRUE, TokenType::FALSE, TokenType::NIL, TokenType::INT_LITERAL,
                TokenType::FLOAT_LITERAL, TokenType::STRING_LITERAL
            }))
        {
            return std::make_unique<LiteralExpr>(LiteralExpr(previous()));
        }

        if (match(TokenType::IDENTIFIER))
        {
            return std::make_unique<VariableExpr>(VariableExpr(previous()));
        }

        if (match(TokenType::LEFT_PAREN))
        {
            std::unique_ptr<Expr> expr = parseExpression();
            consume(TokenType::RIGHT_PAREN, "expected ')' after expression.");
            return std::make_unique<GroupedExpr>(GroupedExpr(expr.release()));
        }

        throw std::runtime_error(syntaxErrorMsg("file", peek(), "expected expression."));
    }

    std::string Parser::syntaxErrorMsg(const std::string& filename, const Token& token, const std::string& msg)
    {
        return "<"+filename+">:" +
                std::to_string(token.position.line) + ":" + std::to_string(token.position.column) +
                ": syntax error: " + msg;
    }

    void Parser::synchronize()
    {
        advance();
        while(!isAtEnd())
        {
            if(previous().type == TokenType::SEMICOLON)
                return;
            
            switch (peek().type)
            {
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
                return;
            }

            advance();
        }
    }
}