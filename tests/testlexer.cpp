#include "catch.hpp"
#include "Lexer.h"
#include <sstream>
#include <iostream>

using BBTCompiler::Lexer;
using BBTCompiler::TokenType;
using BBTCompiler::TokenPosition;

TEST_CASE("LexerInteger", "[Literals]")
{
    std::stringstream ss("54321");
    Lexer lexer;
    const auto& tokens = lexer.getTokens();
    lexer.scan(ss);
    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].value == "54321");
    CHECK(tokens[0].type == TokenType::INT_LITERAL);

    lexer.clear();
    ss = std::stringstream("54 321");
    lexer.scan(ss);
    REQUIRE(tokens.size() == 2);
    CHECK(tokens[0].value == "54");
    CHECK(tokens[0].type == TokenType::INT_LITERAL);
    CHECK(tokens[0].position.column == 1);
    CHECK(tokens[0].position.line == 1);
    
    CHECK(tokens[1].value == "321");
    CHECK(tokens[1].type == TokenType::INT_LITERAL);
    CHECK(tokens[1].position.column == 4);
    CHECK(tokens[1].position.line == 1);
}

TEST_CASE("LexerFloat", "[Literals]")
{
    std::stringstream ss("54.321");
    Lexer lexer;
    const auto& tokens = lexer.getTokens();
    lexer.scan(ss);
    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].value == "54.321");
    CHECK(tokens[0].type == TokenType::FLOAT_LITERAL);

    lexer.clear();
    ss = std::stringstream("5.4 3.21");
    lexer.scan(ss);
    REQUIRE(tokens.size() == 2);
    CHECK(tokens[0].value == "5.4");
    CHECK(tokens[0].type == TokenType::FLOAT_LITERAL);
    CHECK(tokens[0].position.column == 1);
    CHECK(tokens[0].position.line == 1);
    
    CHECK(tokens[1].value == "3.21");
    CHECK(tokens[1].type == TokenType::FLOAT_LITERAL);
    CHECK(tokens[1].position.column == 5);
    CHECK(tokens[1].position.line == 1);
}

TEST_CASE("LexerString", "[Literals]")
{
    std::stringstream ss("\"abcdef\"");
    Lexer lexer;
    const auto& tokens = lexer.getTokens();
    lexer.scan(ss);
    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0].value == "abcdef");
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);

    lexer.clear();
    const char* multiStr = R"("abc" "def" "gh\"i")";
    ss = std::stringstream(multiStr);
    lexer.scan(ss);
    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].value == "abc");
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].position.column == 1);
    CHECK(tokens[0].position.line == 1);
    
    CHECK(tokens[1].value == "def");
    CHECK(tokens[1].type == TokenType::STRING_LITERAL);
    CHECK(tokens[1].position.column == 7);
    CHECK(tokens[1].position.line == 1);

    CHECK(tokens[2].value == "gh\"i");
    CHECK(tokens[2].type == TokenType::STRING_LITERAL);
    CHECK(tokens[2].position.column == 13);
    CHECK(tokens[2].position.line == 1);
}

TEST_CASE("LexerBinaryOperators", "[Operators]")
{
    std::stringstream ss("1+1.0");
    Lexer lexer;
    const auto& tokens = lexer.getTokens();
    lexer.scan(ss);
    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].value == "1");
    CHECK(tokens[0].type == TokenType::INT_LITERAL);
    CHECK(tokens[0].position.column == 1);
    CHECK(tokens[0].position.line == 1);
    CHECK(tokens[1].value == "+");
    CHECK(tokens[1].type == TokenType::OPERATOR);
    CHECK(tokens[1].position.column == 2);
    CHECK(tokens[1].position.line == 1);
    CHECK(tokens[2].value == "1.0");
    CHECK(tokens[2].type == TokenType::FLOAT_LITERAL);
    CHECK(tokens[2].position.column == 3);
    CHECK(tokens[2].position.line == 1);

    lexer.clear();
    ss = std::stringstream(R"(15.2 /="string")");
    lexer.scan(ss);
    REQUIRE(tokens.size() == 4);
    CHECK(tokens[0].value == "15.2");
    CHECK(tokens[0].type == TokenType::FLOAT_LITERAL);
    CHECK(tokens[0].position.column == 1);
    CHECK(tokens[0].position.line == 1);
    CHECK(tokens[1].value == "/");
    CHECK(tokens[1].type == TokenType::OPERATOR);
    CHECK(tokens[1].position.column == 6);
    CHECK(tokens[1].position.line == 1);
    CHECK(tokens[2].value == "=");
    CHECK(tokens[2].type == TokenType::OPERATOR);
    CHECK(tokens[2].position.column == 7);
    CHECK(tokens[2].position.line == 1);
    CHECK(tokens[3].value == "string");
    CHECK(tokens[3].type == TokenType::STRING_LITERAL);
    CHECK(tokens[3].position.column == 8);
    CHECK(tokens[3].position.line == 1);
}

TEST_CASE("LexerUnaryOperators", "[Operators]")
{
    std::stringstream ss("+= -- ++");
    Lexer lexer;
    const auto& tokens = lexer.getTokens();
    lexer.scan(ss);
    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].value == "+=");
    CHECK(tokens[0].type == TokenType::OPERATOR);
    CHECK(tokens[0].position.column == 1);
    CHECK(tokens[0].position.line == 1);
    CHECK(tokens[1].value == "--");
    CHECK(tokens[1].type == TokenType::OPERATOR);
    CHECK(tokens[1].position.column == 4);
    CHECK(tokens[1].position.line == 1);
    CHECK(tokens[2].value == "++");
    CHECK(tokens[2].type == TokenType::OPERATOR);
    CHECK(tokens[2].position.column == 7);
    CHECK(tokens[2].position.line == 1);

    lexer.clear();
    ss = std::stringstream(R"(str+="a"+"b")");
    lexer.scan(ss);
    REQUIRE(tokens.size() == 5);
    CHECK(tokens[0].value == "str");
    CHECK(tokens[0].type == TokenType::IDENTIFIER);
    CHECK(tokens[0].position.column == 1);
    CHECK(tokens[0].position.line == 1);
    CHECK(tokens[1].value == "+=");
    CHECK(tokens[1].type == TokenType::OPERATOR);
    CHECK(tokens[1].position.column == 4);
    CHECK(tokens[1].position.line == 1);
    CHECK(tokens[2].value == "a");
    CHECK(tokens[2].type == TokenType::STRING_LITERAL);
    CHECK(tokens[2].position.column == 6);
    CHECK(tokens[2].position.line == 1);
    CHECK(tokens[3].value == "+");
    CHECK(tokens[3].type == TokenType::OPERATOR);
    CHECK(tokens[3].position.column == 9);
    CHECK(tokens[3].position.line == 1);
    CHECK(tokens[4].value == "b");
    CHECK(tokens[4].type == TokenType::STRING_LITERAL);
    CHECK(tokens[4].position.column == 10);
    CHECK(tokens[4].position.line == 1);
}

TEST_CASE("LexerIdentifiers", "[Keywords][Identifiers]")
{
    std::stringstream ss("if _test void t_10");
    Lexer lexer;
    const auto& tokens = lexer.getTokens();
    lexer.scan(ss);
    REQUIRE(tokens.size() == 4);
    CHECK(tokens[0].value == "if");
    CHECK(tokens[0].type == TokenType::KEYWORD);
    CHECK(tokens[0].position.column == 1);
    CHECK(tokens[0].position.line == 1);
    CHECK(tokens[1].value == "_test");
    CHECK(tokens[1].type == TokenType::IDENTIFIER);
    CHECK(tokens[1].position.column == 4);
    CHECK(tokens[1].position.line == 1);
    CHECK(tokens[2].value == "void");
    CHECK(tokens[2].type == TokenType::KEYWORD);
    CHECK(tokens[2].position.column == 10);
    CHECK(tokens[2].position.line == 1);
    CHECK(tokens[3].value == "t_10");
    CHECK(tokens[3].type == TokenType::IDENTIFIER);
    CHECK(tokens[3].position.column == 15);
    CHECK(tokens[3].position.line == 1);
}

TEST_CASE("LexerFunctions", "[functions]")
{
    const char* mainFunctionStr = R"(
    int main(int argc, const char* argv[])
    {
        return 0;
    }
)";
    std::stringstream ss(mainFunctionStr);
    Lexer lexer;
    const auto& tokens = lexer.getTokens();
    lexer.scan(ss);
    REQUIRE(tokens.size() == 18);
    CHECK(tokens[0].value == "int");
    CHECK(tokens[0].type == TokenType::KEYWORD);
    CHECK(tokens[0].position.line == 2); CHECK(tokens[0].position.column == 5);
    CHECK(tokens[1].value == "main");
    CHECK(tokens[1].type == TokenType::IDENTIFIER);
    CHECK(tokens[1].position.line == 2); CHECK(tokens[1].position.column == 9);
    CHECK(tokens[2].value == "(");
    CHECK(tokens[2].type == TokenType::OPERATOR);
    CHECK(tokens[2].position.line == 2); CHECK(tokens[2].position.column == 13);
    CHECK(tokens[3].value == "int");
    CHECK(tokens[3].type == TokenType::KEYWORD);
    CHECK(tokens[3].position.line == 2); CHECK(tokens[3].position.column == 14);
    CHECK(tokens[4].value == "argc");
    CHECK(tokens[4].type == TokenType::IDENTIFIER);
    CHECK(tokens[4].position.line == 2); CHECK(tokens[4].position.column == 18);
    CHECK(tokens[5].value == ",");
    CHECK(tokens[5].type == TokenType::OPERATOR);
    CHECK(tokens[5].position.line == 2); CHECK(tokens[5].position.column == 22);
    CHECK(tokens[6].value == "const");
    CHECK(tokens[6].type == TokenType::KEYWORD);
    CHECK(tokens[6].position.line == 2); CHECK(tokens[6].position.column == 24);
    CHECK(tokens[7].value == "char");
    CHECK(tokens[7].type == TokenType::KEYWORD);
    CHECK(tokens[7].position.line == 2); CHECK(tokens[7].position.column == 30);
    CHECK(tokens[8].value == "*");
    CHECK(tokens[8].type == TokenType::OPERATOR);
    CHECK(tokens[8].position.line == 2); CHECK(tokens[8].position.column == 34);
    CHECK(tokens[9].value == "argv");
    CHECK(tokens[9].type == TokenType::IDENTIFIER);
    CHECK(tokens[9].position.line == 2); CHECK(tokens[9].position.column == 36);
    CHECK(tokens[10].value == "[");
    CHECK(tokens[10].type == TokenType::OPERATOR);
    CHECK(tokens[10].position.line == 2); CHECK(tokens[10].position.column == 40);
    CHECK(tokens[11].value == "]");
    CHECK(tokens[11].type == TokenType::OPERATOR);
    CHECK(tokens[11].position.line == 2); CHECK(tokens[11].position.column == 41);
    CHECK(tokens[12].value == ")");
    CHECK(tokens[12].type == TokenType::OPERATOR);
    CHECK(tokens[12].position.line == 2); CHECK(tokens[12].position.column == 42);
    CHECK(tokens[13].value == "{");
    CHECK(tokens[13].type == TokenType::OPERATOR);
    CHECK(tokens[13].position.line == 3); CHECK(tokens[13].position.column == 5);
    CHECK(tokens[14].value == "return");
    CHECK(tokens[14].type == TokenType::KEYWORD);
    CHECK(tokens[14].position.line == 4); CHECK(tokens[14].position.column == 9);
    CHECK(tokens[15].value == "0");
    CHECK(tokens[15].type == TokenType::INT_LITERAL);
    CHECK(tokens[15].position.line == 4); CHECK(tokens[15].position.column == 16);
    CHECK(tokens[16].value == ";");
    CHECK(tokens[16].type == TokenType::OPERATOR);
    CHECK(tokens[16].position.line == 4); CHECK(tokens[16].position.column == 17);
    CHECK(tokens[17].value == "}");
    CHECK(tokens[17].type == TokenType::OPERATOR);
    CHECK(tokens[17].position.line == 5); CHECK(tokens[17].position.column == 5);
}

//TEST_CASE("LexerComments", "[Comments]")
//{
//    REQUIRE(false);
//}