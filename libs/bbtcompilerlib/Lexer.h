#pragma once
#include <istream>
#include <vector>

#include "Token.h"

namespace BBTCompiler
{
    class Lexer
    {
    public:
        void scan(std::istream& file);
        void scan(std::string_view file);
        std::vector<Token>& getTokens() { return m_Tokens; }
        const std::vector<Token>& getTokens() const { return m_Tokens; }
        void reset();
    private:
        Token m_CurrentToken;
        unsigned char m_CurrentChar{};
        std::vector<Token> m_Tokens;
        TokenPosition m_Position{};

        Token& newToken(TokenType type, TokenPosition position);
        void processNumLiteral(std::istream& stream);
        void processStringLiteral(std::istream& stream);
        void processOperator(std::istream& stream);
        void processIdentifier(std::istream& stream);

        bool isOperator(unsigned char c);
        bool isPairedOperator(const std::string& pairString);
        bool isKeyword(const std::string& word);
        void incrementLine(int count = 1);
        void incrementColumn(int count = 1);
    };
}