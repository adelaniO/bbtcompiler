#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "catch.hpp"
#include "Parser.h"
#include "Statement.h"
#include "JsonVisitor.h"

using BBTCompiler::Lexer;
using BBTCompiler::Parser;
using BBTCompiler::Expr;
using BBTCompiler::BinaryExpr;
using BBTCompiler::ASTJSonVisitor;
using BBTCompiler::Stmt;

TEST_CASE("ParseExpression", "[Expression]")
{
    Lexer lexer;
    ASTJSonVisitor jsonVisitor;
    SECTION("Binary Expression")
    {
        const auto result = R"(
            {
                "type": "ExpressionStatement",
                "expression": {
                    "lhs": {
                        "type": "BinaryExpression",
                        "lhs": {
                            "type": "PrimaryExpression",
                            "value": "1"
                        },
                        "operator": "*",
                        "rhs": {
                            "type": "PrimaryExpression",
                            "value": "2"
                        }
                    },
                    "type": "BinaryExpression",
                    "operator": "-",
                    "rhs": {
                        "type": "PrimaryExpression",
                        "value": "3"
                    }
                }
            }
        )"_json;
        lexer.scan(std::stringstream("1*2-3;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(jsonVisitor.getJson() == result);
    }

    SECTION("Variable Declaration")
    {
        const auto result = R"(
            {
                "type": "VariableStatement",
                "initializer": {},
                "name": "a"
            }
        )"_json;
        lexer.scan(std::stringstream("let a;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(jsonVisitor.getJson() == result);
    }

    SECTION("Assignment Expression")
    {
        const auto result = R"(
            {
                "type": "ExpressionStatement",
                "expression": {
                    "name": "a",
                    "type": "AssignmentExpression",
                    "value": {
                        "type": "PrimaryExpression",
                        "value": "5"
                    }
                }
            }
        )"_json;
        lexer.scan(std::stringstream("a = 5;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(jsonVisitor.getJson() == result);
    }

    SECTION("Assignment Expression With Initializer")
    {
        const auto result = R"(
            {
                "type": "VariableStatement",
                "initializer": {
                    "type": "Variable",
                    "value": "b"
                },
                "name": "a"
            }
        )"_json;
        lexer.scan(std::stringstream("let a = b;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(jsonVisitor.getJson() == result);
    }
}

TEST_CASE("ParseStatements", "[Stmt][Statements]")
{
    Lexer lexer;
    ASTJSonVisitor jsonVisitor;
    SECTION("Binary Expression")
    {
        const auto result = R"(
            {
                "type": "PrintStatement",
                "expression": {
                    "type": "PrimaryExpression",
                    "value": "5"
                }
            }
        )"_json;
        lexer.scan(std::stringstream("print 5;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(jsonVisitor.getJson() == result);
    }
}

TEST_CASE("ParseBlockStatements", "[Stmt][Block]")
{
    Lexer lexer;
    ASTJSonVisitor jsonVisitor;
    SECTION("Block Statements")
    {
        const auto result = R"(
            {
                "type": "BlockStatement",
                "statements": [
                    {
                        "expression": {
                            "type": "PrimaryExpression",
                            "value": "5"
                        },
                        "type": "PrintStatement"
                    },
                    {
                        "expression": {
                            "type": "PrimaryExpression",
                            "value": "10"
                        },
                        "type": "PrintStatement"
                    }
                ]
            }
        )"_json;
        lexer.scan(std::stringstream("{print 5;print 10;}"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(jsonVisitor.getJson() == result);
    }
}