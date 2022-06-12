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
        // Types
        INT, CHAR, BOOL, FLOAT,
        // Statements
        IF, ELSE, FOR, WHILE, CONTINUE, BREAK,
        TRUE, FALSE, NIL, RETURN,
        PRINT, LET,
        // Declarations
        FN, CLASS,
        // Type Qualifiers
        CONST,
        // Operators
        LEFT_BRACE, RIGHT_BRACE, LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACKET, RIGHT_BRACKET, DOT, COMMA, SEMICOLON, COLON,
        PLUS, MINUS, STAR, SLASH, AMPERSAND, LESS, GREATER, EQ, NOT,
        PIPE,
        // Double Operators
        PLUS_PLUS, MINUS_MINUS, PLUS_EQ, MINUS_EQ, EQ_EQ, NOT_EQ,
        GREATER_EQ, LESS_EQ, OR, AND, RIGHT_ARROW,

        END,
        INVALID
    };

    const std::unordered_map<std::string, TokenType> Keywords{
        // Type Qualifiers
        {"const",       TokenType::CONST},
        // types
        {"int",         TokenType::INT},
        {"char",        TokenType::CHAR},
        {"bool",     TokenType::BOOL},
        {"float",       TokenType::FLOAT},
        // bool literals
        {"true",        TokenType::TRUE},
        {"false",       TokenType::FALSE},
        {"null",        TokenType::NIL},
        // Statements
        {"return",      TokenType::RETURN},
        {"print",       TokenType::PRINT},
        {"let",         TokenType::LET},
        {"if",          TokenType::IF},
        {"else",        TokenType::ELSE},
        {"while",       TokenType::WHILE},
        {"for",         TokenType::FOR},
        {"continue",    TokenType::CONTINUE},
        {"break",       TokenType::BREAK},
        // Declarations
        {"fn",          TokenType::FN},
        {"class",          TokenType::CLASS}
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
        {'>', TokenType::GREATER},
        {'<', TokenType::LESS},
        {'=', TokenType::EQ},
        {'!', TokenType::NOT},
        {'|', TokenType::PIPE}
    };

    const std::unordered_map<std::string, TokenType> PairedOperators{
        {"==", TokenType::EQ_EQ},
        {"!=", TokenType::NOT_EQ},
        {"++", TokenType::PLUS_PLUS},
        {"--", TokenType::MINUS_MINUS},
        {"+=", TokenType::PLUS_EQ},
        {"-+", TokenType::MINUS_EQ},
        {">=", TokenType::GREATER_EQ},
        {"<=", TokenType::LESS_EQ},
        {"||", TokenType::OR},
        {"&&", TokenType::AND},
        {"->", TokenType::RIGHT_ARROW},
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