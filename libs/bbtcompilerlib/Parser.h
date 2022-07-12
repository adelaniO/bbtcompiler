#pragma once

#include <string>
#include "Lexer.h"
#include "AST.h"
#include "SymbolTable.h"


namespace BBTCompiler
{
    class SymbolTable;

    class Parser
    {
    public:
        Parser() = default;
        AST& parse(std::string_view file);
        AST& parse(std::istream& file);
    private:
        Token& advance();
        Token& previous();
        Token& peek();
        Token& consume(TokenType type, const std::string& errorMsg);
        Token& parseType();
        bool isAtEnd();
        bool check(TokenType type);
        bool check(const std::vector<TokenType>& types);
        bool match(const TokenType& type);
        bool match(const std::vector<TokenType>& types);
        bool isValidNode(size_t& node) { return node < m_Tree.size(); }
        size_t parseBlock();
        size_t parseDeclaration();
        std::pair<size_t, size_t> parseNewVariable();
        size_t parseVariableDeclaration();
        size_t parseStatement();
        size_t parseExpressionStatement();
        size_t parseFunctionStatement(const std::string& kind);
        size_t parseReturnStatement();
        size_t parsePrintStatement();
        size_t parseForStatement();
        size_t parseWhileStatement();
        size_t parseIfStatement();
        size_t parseExpression();
        size_t parseAssignmentExpr();
        size_t parseOrExpr();
        size_t parseAndExpr();
        size_t parseEqualityExpr();
        size_t parseComparisonExpr();
        size_t parseAdditiveExpr();
        size_t parseMultiplicativeExpr();
        size_t parseUnaryExpr();
        size_t parseCallExpr();
        size_t parsePrimaryExpr();
        size_t finishCall(size_t callee);
        std::string syntaxErrorMsg(const std::string& filename, const Token& token, const std::string& msg);
        void synchronize();
    private:
        AST m_Tree;
        std::vector<Token> m_Tokens;
        Lexer lexer;
        SymbolTable m_SymbolTable;
        std::vector<Token>::iterator m_Current;
    };
}