#include "Lexer.h"
#include <iostream>
#include <string>
#include <algorithm>


namespace BBTCompiler
{
    void Lexer::clear()
    {
        m_Tokens.clear();
        m_IsInMultiLineComment = false;
        m_IsInSingleLineComment = false;
        m_CurrentToken = Token{}; 
        m_State = LexerState::NORMAL;
    }

    void Lexer::parse(std::istream& stream)
    {
        m_Position = Position{ 1, 0 };
        char currentChar;
        char nextChar;
        while(stream >> std::noskipws >> currentChar)
        {
            incrementPosition(1);
            nextChar = stream.peek();
            if(std::isspace(currentChar) && m_State != LexerState::LINE_COMMENT && m_State != LexerState::BLOCK_COMMENT)
            {
                processToken(m_CurrentToken);
                continue;
            }
            else if(m_State == LexerState::NORMAL)
            {
                if(std::isdigit(currentChar))
                    m_State = LexerState::INT;
                if(currentChar == '"')
                {
                    m_State = LexerState::STRING;
                    continue;
                }
                if(isOperator(currentChar))
                {
                    processToken(m_CurrentToken); // Process what was before
                    m_State = LexerState::OPERATOR;
                    m_CurrentToken.value += currentChar;
                    processToken(m_CurrentToken);
                    continue;
                }
            }
            else if(m_State == LexerState::STRING)
            {
                if(currentChar == '\\')
                {
                    if(nextChar == EOF)
                    {
                        m_State = LexerState::ERROR;
                        return;
                    }
                    m_CurrentToken.value += stream.get();
                    incrementPosition(1);
                    continue;
                }
                else if(currentChar == '"')
                {
                    processToken(m_CurrentToken);
                    m_State = LexerState::NORMAL;
                    continue;
                }
            }
            else if(m_State == LexerState::FLOAT)
            {
                if(currentChar == '.' || !std::isdigit(currentChar))
                {
                    processToken(m_CurrentToken);
                    stream.unget();
                    continue;
                }
            }
            else if(m_State == LexerState::INT)
            {
                if(currentChar == '.')
                {
                    m_State = LexerState::FLOAT;
                }
                else if(!std::isdigit(currentChar))
                {
                    processToken(m_CurrentToken);
                    stream.unget();
                    continue;
                }
            }
            m_CurrentToken.value += currentChar;
        }
        processToken(m_CurrentToken);
    }
    
    void Lexer::processToken(Token& token)
    {
        if(token.value.empty())
        {
            token = Token{};
            m_State = LexerState::NORMAL;
            return;
        }
        if(m_State == LexerState::NORMAL)
        {
            if(isKeyword(token.value))
                token.type = TokenType::KEYWORD;
            else
                token.type = TokenType::IDENTIFIER;
        }
        else if(isInt(token.value) && m_State == LexerState::INT)
        {
            token.type = TokenType::INT_LITERAL;
        }
        else if(isFloat(token.value) && m_State == LexerState::FLOAT)
        {
            token.type = TokenType::FLOAT_LITERAL;
        }
        else if(m_State == LexerState::STRING)
        {
            token.type = TokenType::STRING_LITERAL;
        }
        else if(m_State == LexerState::OPERATOR)
        {
            token.type = TokenType::OPERATOR;
        }
        token.position = m_Position;
        token.position.column-= token.value.size();
        m_Tokens.push_back(token);
        token = Token{};
        m_State = LexerState::NORMAL;
    }

    bool Lexer::isInt(std::string_view word)
    {
        return std::all_of(word.begin(), word.end(), [](const char c){ return std::isdigit(c); });
    }

    bool Lexer::isFloat(std::string_view word)
    {
        int numDots{ 0 };
        bool canBeFloat = std::all_of(word.begin(), word.end(), [&numDots](const unsigned char c)
        {
            if(c == '.')
            {
                numDots++;
                return numDots < 2;
            }
            else if(std::isdigit(c))
                return true;
            else
                return false;
        });
        return canBeFloat && numDots == 1;
    }

    bool Lexer::isString(std::string_view word)
    {
        if(word.size() < 2)
            return false;
        if(word.front() != '"' && word.back() != '"')
            return false;
        return true;
    }

    void Lexer::incrementPosition(size_t column, size_t line)
    {
        m_Position.column += column;
        m_Position.line += line;
    }
}
