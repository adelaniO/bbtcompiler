#include "catch.hpp"
#include "Lexer.h"
#include <sstream>
#include <iostream>

using BBTCompiler::Lexer;
using BBTCompiler::TokenType;

TEST_CASE("LexerInteger", "[Literals]")
{
    std::stringstream ss("54321");
    Lexer lexer;
    const auto& tokens = lexer.getTokens();
    lexer.parse(ss);
    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].value == "54321");
    CHECK(tokens[0].type == TokenType::INT_LITERAL);

    lexer.clear();
    ss = std::stringstream("54 321");
    lexer.parse(ss);
    REQUIRE(tokens.size() == 2);
    CHECK(tokens[0].value == "54");
    CHECK(tokens[0].type == TokenType::INT_LITERAL);
    
    CHECK(tokens[1].value == "321");
    CHECK(tokens[1].type == TokenType::INT_LITERAL);
}

TEST_CASE("LexerFloat", "[Literals]")
{
    std::stringstream ss("54.321");
    Lexer lexer;
    const auto& tokens = lexer.getTokens();
    lexer.parse(ss);
    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].value == "54.321");
    CHECK(tokens[0].type == TokenType::FLOAT_LITERAL);

    lexer.clear();
    ss = std::stringstream("5.4 3.21");
    lexer.parse(ss);
    REQUIRE(tokens.size() == 2);
    CHECK(tokens[0].value == "5.4");
    CHECK(tokens[0].type == TokenType::FLOAT_LITERAL);
    
    CHECK(tokens[1].value == "3.21");
    CHECK(tokens[1].type == TokenType::FLOAT_LITERAL);
}

TEST_CASE("LexerString", "[Literals]")
{
    std::stringstream ss("\"abcdef\"");
    Lexer lexer;
    const auto& tokens = lexer.getTokens();
    lexer.parse(ss);
    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].value == "abcdef");
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);

    lexer.clear();
    ss = std::stringstream("\"abc\" \"def\" \"gh\\\"i\"");
    lexer.parse(ss);
    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].value == "abc");
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    
    CHECK(tokens[1].value == "def");
    CHECK(tokens[1].type == TokenType::STRING_LITERAL);

    CHECK(tokens[2].value == "gh\"i");
    CHECK(tokens[2].type == TokenType::STRING_LITERAL);
}