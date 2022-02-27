#pragma once
#include <istream>
#include <vector>
#include <unordered_set>

namespace BBTCompiler
{
    struct Position
    {
        size_t line{ std::numeric_limits<size_t>::max() };
        size_t column{ std::numeric_limits<size_t>::max() };
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

    enum class TokenType { INT_LITERAL, FLOAT_LITERAL, STRING_LITERAL, OPERATOR, KEYWORD, IDENTIFIER, INVALID };
    struct Token
    {
        TokenType type{ TokenType::INVALID };
        Position position{};
        std::string value{};
    };

    enum class LexerState { NORMAL, LINE_COMMENT, BLOCK_COMMENT, STRING, INT, FLOAT, OPERATOR, ERROR };

    class Lexer
    {
    public:
        void parse(std::istream& stream);
        void processToken(Token& token);
        std::vector<Token>& getTokens() { return m_Tokens; }
        const std::vector<Token>& getTokens() const { return m_Tokens; }
        void clear();
    private:
        Token m_CurrentToken;
        std::vector<Token> m_Tokens;
        Position m_Position{};
        bool m_IsInSingleLineComment{}, m_IsInMultiLineComment{};
        LexerState m_State{ LexerState::NORMAL };

        bool isInt(std::string_view word);
        bool isFloat(std::string_view word);
        bool isString(std::string_view word);
        void setPosition(size_t column, size_t line);
        void incrementPosition(size_t column, size_t line = 0);
        bool isOperator(unsigned char c) { return Operators.find(c) != Operators.end(); }
        bool isKeyword(const std::string& word) { return Keywords.find(word) != Keywords.end(); }
    };
}