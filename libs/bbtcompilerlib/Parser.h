#pragma once

#include <string>
#include "Lexer.h"
#include "Expression.h"
#include "Statement.h"
#include "SymbolTable.h"


namespace BBTCompiler
{
    class SymbolTable;

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
        bool check(const std::vector<TokenType>& types);
        bool match(const TokenType& type);
        bool match(const std::vector<TokenType>& types);
        std::vector<std::unique_ptr<Stmt>> parseBlock();
        std::unique_ptr<Stmt> parseDeclaration();
        std::pair<Token, Token> parseNewVariable();
        Token parseType();
        std::unique_ptr<Stmt> parseVariableDeclaration();
        std::unique_ptr<Stmt> parseStatement();
        std::unique_ptr<Stmt> parseExpressionStatement();
        std::unique_ptr<Stmt> parseFunctionStatement(const std::string& kind);
        std::unique_ptr<Stmt> parseReturnStatement();
        std::unique_ptr<Stmt> parsePrintStatement();
        std::unique_ptr<Stmt> parseForStatement();
        std::unique_ptr<Stmt> parseWhileStatement();
        std::unique_ptr<Stmt> parseIfStatement();
        std::unique_ptr<Expr> parseExpression();
        std::unique_ptr<Expr> parseAssignmentExpr();
        std::unique_ptr<Expr> parseOrExpr();
        std::unique_ptr<Expr> parseAndExpr();
        std::unique_ptr<Expr> parseEqualityExpr();
        std::unique_ptr<Expr> parseComparisonExpr();
        std::unique_ptr<Expr> parseAdditiveExpr();
        std::unique_ptr<Expr> parseMultiplicativeExpr();
        std::unique_ptr<Expr> parseUnaryExpr();
        std::unique_ptr<Expr> parseCallExpr();
        std::unique_ptr<Expr> parsePrimaryExpr();
        std::unique_ptr<Expr> finishCall(std::unique_ptr<Expr> callee);
        std::string syntaxErrorMsg(const std::string& filename, const Token& token, const std::string& msg);
        void synchronize();
    private:
        std::vector<Token>& m_Tokens;
        SymbolTable m_SymbolTable;
        std::vector<Token>::iterator m_Current;
        std::vector<std::unique_ptr<Stmt>> m_Statements;
    };
}