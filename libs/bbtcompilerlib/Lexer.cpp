#include "Lexer.h"
#include <iostream>
#include <string>
#include <algorithm>


namespace BBTCompiler
{
    void Lexer::reset()
    {
        m_Tokens.clear();
        m_CurrentToken = Token{}; 
        m_Position = TokenPosition{ 1, 1 };
    }

    void Lexer::scan(std::istream& stream)
    {
        while(stream >> std::noskipws >> m_CurrentChar)
        {
            if(m_CurrentChar == '\n')
                incrementLine();
            else if(std::isdigit(m_CurrentChar))
                processNumLiteral(stream);
            else if(m_CurrentChar == '"')
                processStringLiteral(stream);
            else if(isOperator(m_CurrentChar))
                processOperator(stream);
            else if(std::isalpha(m_CurrentChar) || m_CurrentChar == '_')
                processIdentifier(stream);

            incrementColumn();
        }
        m_Tokens.emplace_back(Token{TokenType::END, m_Position, ""});
    }

    Token& Lexer::newToken(TokenType type, TokenPosition position)
    {
        return m_Tokens.emplace_back(Token{type, position, ""});
    }

    void Lexer::processNumLiteral(std::istream& stream)
    {
        Token& token = newToken(TokenType::INT_LITERAL, m_Position);
        token.value += m_CurrentChar;
        bool hasDot{false};
        while(stream >> std::noskipws >> m_CurrentChar)
        {
            if(std::isdigit(m_CurrentChar))
            {
                token.value += m_CurrentChar;
            }
            else if(m_CurrentChar == '.' && !hasDot)
            {
                token.value += m_CurrentChar;
                token.type = TokenType::FLOAT_LITERAL;
                hasDot = true;
            }
            else
            {
                stream.putback(m_CurrentChar);
                break;
            }
        }
        incrementColumn(token.value.size() - 1);
    }

    void Lexer::processStringLiteral(std::istream& stream)
    {
        Token& token = newToken(TokenType::STRING_LITERAL, m_Position);
        bool escape{false};
        incrementColumn();
        while(stream >> std::noskipws >> m_CurrentChar)
        {
            if(m_CurrentChar == '\\')
            {
                escape = true;
            }
            else if(escape || m_CurrentChar != '"')
            {
                token.value += m_CurrentChar;
                escape = false;
            } 
            else
            {
                break;
            }
            incrementColumn();
        }
    }

    void Lexer::processOperator(std::istream& stream)
    {
        TokenType type = isOperator(m_CurrentChar) ? Operators.find(m_CurrentChar)->second: TokenType::INVALID;
        Token& token = newToken(type, m_Position);
        const unsigned char nextChar = stream.peek();
        std::string pairString;
        pairString.push_back(m_CurrentChar);
        pairString.push_back(nextChar);
        if(isPairedOperator(pairString))
        {
            stream >> std::noskipws >> m_CurrentChar;
            token.type = PairedOperators.find(pairString)->second;
            incrementColumn();
        }
    }

    void Lexer::processIdentifier(std::istream& stream)
    {
        Token& token = newToken(TokenType::IDENTIFIER, m_Position);
        token.value += m_CurrentChar;
        while(stream >> std::noskipws >> m_CurrentChar)
        {
            if(isalnum(m_CurrentChar) || m_CurrentChar == '_')
            {
                token.value += m_CurrentChar;
                incrementColumn();
            }
            else
            {
                stream.putback(m_CurrentChar);
                break;
            }
        }
        if (isKeyword(token.value))
        {
            token.type = Keywords.find(token.value)->second;
            token.value.clear();
        }
    }

    bool Lexer::isOperator(unsigned char c)
    {
        return Operators.find(c) != Operators.end();
    }

    bool Lexer::isPairedOperator(const std::string& pairString)
    {
        const auto search = PairedOperators.find(pairString);
        return search != PairedOperators.cend();
    }

    bool Lexer::isKeyword(const std::string& word)
    {
         return Keywords.find(word) != Keywords.end();
    }

    void Lexer::incrementLine(int count)
    {
        m_Position.column = 0;
        m_Position.line += count;
    }
    void Lexer::incrementColumn(int count)
    {
        m_Position.column += count;
    }
}
