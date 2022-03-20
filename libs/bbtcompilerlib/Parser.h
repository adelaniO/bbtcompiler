#pragma once

#include <vector>
#include <string>
#include "Lexer.h"
#include "Expression.h"
#include "Statement.h"

namespace BBTCompiler
{
    class Parser
    {
    public:
        Parser(std::vector<Token>& tokens);
        std::vector<std::unique_ptr<Stmt>>& parse();
    private:
        Token& advance();
        Token& previous();
        Token& peek();
        Token& consume(TokenType type, const std::string& errorMsg);
        bool isAtEnd();
        bool check(TokenType type);
        bool match(const std::vector<TokenType>& types);
        std::unique_ptr<Stmt> parseStatement();
        std::unique_ptr<Stmt> parseExpressionStatement();
        std::unique_ptr<Stmt> parsePrintStatement();
        Expr* parseExpression();
        Expr* parseAssignmentExpr();
        Expr* parseEqualityExpr();
        Expr* parseComparisonExpr();
        Expr* parseAdditiveExpr();
        Expr* parseMultiplicativeExpr();
        Expr* parseUnaryExpr();
        Expr* parsePrimaryExpr();
        std::string syntaxErrorMsg(const std::string& filename, const Token& token, const std::string& msg);
        void synchronize();
    private:
        std::vector<Token>& m_Tokens;
        std::vector<Token>::iterator m_Current;
        std::vector<std::unique_ptr<Stmt>> m_Statements;
    };
}