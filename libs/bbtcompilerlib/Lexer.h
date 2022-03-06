#pragma once
#include <istream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace BBTCompiler
{
    struct TokenPosition
    {
        size_t line{ 1 };
        size_t column{ 1 };
    };

    const std::unordered_set<std::string> Keywords{
        "do","if","else","while","continue","break",
        "int","char","boolean","long","float",
        "const","static","void",
        "true","false","null",
        "case","switch","return"
    };

    const std::unordered_set<unsigned char> Operators{
        '{','}','(',')','[',']','.',',',';','+','-','*','/','&','<','>','=','-'
    };

    const std::unordered_map<unsigned char, std::unordered_set<unsigned char>> PairedOperators{
        { '+', {'+','='} },
        { '-', {'-','='} }
    };

    enum class TokenType { INT_LITERAL, FLOAT_LITERAL, STRING_LITERAL, OPERATOR, KEYWORD, IDENTIFIER, INVALID };
    struct Token
    {
        TokenType type{ TokenType::INVALID };
        TokenPosition position{};
        std::string value{};
        friend bool operator==(const Token& l, const Token& r)
        {
            return l.type == r.type
                && l.position.column == r.position.column
                && l.position.line == r.position.line
                && l.value == r.value;
        }
    };

    enum class LexerState { NORMAL, LINE_COMMENT, BLOCK_COMMENT, STRING, INT, FLOAT, OPERATOR, ERROR };

    class Lexer
    {
    public:
        void scan(std::istream& stream);
        std::vector<Token>& getTokens() { return m_Tokens; }
        const std::vector<Token>& getTokens() const { return m_Tokens; }
        void reset();
    private:
        Token m_CurrentToken;
        unsigned char m_CurrentChar;
        std::vector<Token> m_Tokens;
        TokenPosition m_Position{};
        LexerState m_State{ LexerState::NORMAL };

        Token& newToken(TokenType type, TokenPosition position);
        void processNumLiteral(std::istream& stream);
        void processStringLiteral(std::istream& stream);
        void processOperator(std::istream& stream);
        void processIdentifier(std::istream& stream);

        bool isOperator(unsigned char c);
        bool isPairedOperator(unsigned char a, unsigned char b);
        bool isKeyword(const std::string& word);
        void incrementLine(int count = 1);
        void incrementColumn(int count = 1);
    };
}