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

    enum class TokenType
    {
        IDENTIFIER,
        INT_LITERAL, FLOAT_LITERAL, STRING_LITERAL,
        //Keywords
        IF, ELSE, WHILE, CONTINUE, BREAK, INT, CHAR, BOOL,
        FLOAT, CONST, STATIC, VOID, TRUE, FALSE, NIL, RETURN,
        // Operators
        LEFT_BRACE, RIGHT_BRACE, LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACKET, RIGHT_BRACKET, DOT, COMMA, SEMICOLON, COLON,
        PLUS, MINUS, STAR, SLASH, AMPERSAND, LESS, MORE, EQ,
        // Double Operators
        PLUS_PLUS, MINUS_MINUS, PLUS_EQ, MINUS_EQ,

        END,
        INVALID
    };

    const std::unordered_map<std::string, TokenType> Keywords{
        {"if",          TokenType::IF},
        {"else",        TokenType::ELSE},
        {"while",       TokenType::WHILE},
        {"continue",    TokenType::CONTINUE},
        {"break",       TokenType::BREAK},
        {"int",         TokenType::INT},
        {"char",        TokenType::CHAR},
        {"boolean",     TokenType::BOOL},
        {"float",       TokenType::FLOAT},
        {"const",       TokenType::CONST},
        {"static",      TokenType::STATIC},
        {"void",        TokenType::VOID},
        {"true",        TokenType::TRUE},
        {"false",       TokenType::FALSE},
        {"null",        TokenType::NIL},
        {"return",      TokenType::RETURN}
    };

    const std::unordered_map<unsigned char, TokenType> Operators{
        {'{', TokenType::LEFT_BRACE},
        {'}', TokenType::RIGHT_BRACE},
        {'(', TokenType::LEFT_PAREN},
        {')', TokenType::RIGHT_PAREN},
        {'[', TokenType::LEFT_BRACKET},
        {']', TokenType::RIGHT_BRACKET},
        {'.', TokenType::DOT},
        {',', TokenType::COMMA},
        {';', TokenType::SEMICOLON},
        {':', TokenType::COLON},
        {'+', TokenType::PLUS},
        {'-', TokenType::MINUS},
        {'*', TokenType::STAR},
        {'/', TokenType::SLASH},
        {'&', TokenType::AMPERSAND},
        {'<', TokenType::LESS},
        {'>', TokenType::MORE},
        {'=', TokenType::EQ}
    };

    const std::unordered_map<std::string, TokenType> PairedOperators{
        {"++", TokenType::PLUS_PLUS},
        {"--", TokenType::MINUS_MINUS},
        {"+=", TokenType::PLUS_EQ},
        {"-+", TokenType::MINUS_EQ}
    };

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