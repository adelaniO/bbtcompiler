#pragma once

#include <vector>
#include <string>
#include "Lexer.h"
#include "Expression.h"

namespace BBTCompiler
{
    class Parser
    {
    public:
        Parser(std::vector<Token>& tokens);
        Expr* parse();
    private:
        Token& advance();
        Token& previous();
        Token& peek();
        Token& consume(TokenType type, const std::string& errorMsg);
        bool isAtEnd();
        bool check(TokenType type);
        bool match(const std::vector<TokenType>& types);
        Expr* parseExpression();
        Expr* parseAssignmentExpr();
        Expr* parseEqualityExpr();
        Expr* parseComparisonExpr();
        Expr* parseAdditiveExpr();
        Expr* parseMultiplicativeExpr();
        Expr* parseUnaryExpr();
        Expr* parsePrimaryExpr();
        std::string syntaxErrorMsg(const std::string& filename, const Token& token, const std::string& msg);
    private:
        std::vector<Token>& m_Tokens;
        std::vector<Token>::iterator m_Current;
    };
}