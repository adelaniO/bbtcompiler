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
        m_State = LexerState::NORMAL;
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
        while(stream >> std::noskipws >> m_CurrentChar)
        {
            if(m_CurrentChar == '\\')
            {
                incrementColumn();
                escape = true;
            }
            else if(escape || m_CurrentChar != '"')
            {
                token.value += m_CurrentChar;
                escape = false;
            } 
            else
            {
                incrementColumn();
                break;
            }
        }
        incrementColumn(token.value.size());
    }

    void Lexer::processOperator(std::istream& stream)
    {
        Token& token = newToken(TokenType::OPERATOR, m_Position);
        token.value += m_CurrentChar;
        const unsigned char nextChar = stream.peek();
        if(isPairedOperator(m_CurrentChar, nextChar))
        {
            stream >> std::noskipws >> m_CurrentChar;
            token.value += m_CurrentChar;
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
            token.type = TokenType::KEYWORD;
    }

    bool Lexer::isOperator(unsigned char c)
    {
        return Operators.find(c) != Operators.end();
    }

    bool Lexer::isPairedOperator(unsigned char a, unsigned char b)
    {
        const auto search = PairedOperators.find(a);
        if(search != PairedOperators.cend())
        {
            const auto secondSearch = search->second.find(b);
            return secondSearch != search->second.cend();
        }
        return false;
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
