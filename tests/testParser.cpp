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
                        "lhs": { "type": "PrimaryExpression", "value": "1" },
                        "operator": "*",
                        "rhs": { "type": "PrimaryExpression", "value": "2" }
                    },
                    "type": "BinaryExpression",
                    "operator": "-",
                    "rhs": { "type": "PrimaryExpression", "value": "3" }
                }
            }
        )"_json;
        lexer.scan(std::stringstream("1*2-3;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }

    SECTION("Variable Declaration")
    {
        const auto result = R"(
            {
                "type": "VariableStatement",
                "name": "a",
                "initializer": {}
            }
        )"_json;
        lexer.scan(std::stringstream("let a;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }

    SECTION("Assignment Expression")
    {
        const auto result = R"(
            {
                "type": "ExpressionStatement",
                "expression": {
                    "name": "a",
                    "type": "AssignmentExpression",
                    "value": { "type": "PrimaryExpression", "value": "5" }
                }
            }
        )"_json;
        lexer.scan(std::stringstream("a = 5;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }

    SECTION("Assignment Expression With Initializer")
    {
        const auto result = R"(
            {
                "type": "VariableStatement",
                "initializer": { "type": "Variable", "value": "b" },
                "name": "a"
            }
        )"_json;
        lexer.scan(std::stringstream("let a = b;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
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
                "expression": { "type": "PrimaryExpression", "value": "5" }
            }
        )"_json;
        lexer.scan(std::stringstream("print 5;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
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
                        "type": "PrintStatement",
                        "expression": { "type": "PrimaryExpression", "value": "5" }
                    },
                    {
                        "type": "PrintStatement",
                        "expression": { "type": "PrimaryExpression", "value": "10" }
                    }
                ]
            }
        )"_json;
        lexer.scan(std::stringstream("{print 5;print 10;}"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("ParseIfStatement", "[Stmt][If]")
{
    Lexer lexer;
    ASTJSonVisitor jsonVisitor;
    SECTION("If Statement")
    {
        const auto result = R"(
            {
                "type": "IfStatement",
                "condition": { "type": "PrimaryExpression", "value": "true" },
                "then": 
                    {
                        "type": "PrintStatement",
                        "expression": { "type": "PrimaryExpression", "value": "10" }
                    },
                "else": {}
            }
        )"_json;
        lexer.scan(std::stringstream("if(true) print 10;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
    SECTION("If Else Statement")
    {
        const auto result = R"(
            {
                "type": "IfStatement",
                "condition": { "type": "PrimaryExpression", "value": "true" },
                "then": 
                    {
                        "type": "PrintStatement",
                        "expression": { "type": "PrimaryExpression", "value": "10" }
                    },
                "else":
                    {
                        "type": "PrintStatement",
                        "expression": { "type": "PrimaryExpression", "value": "5" }
                    }
            }
        )"_json;
        lexer.scan(std::stringstream("if(true) print 10; else print 5;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }

    SECTION("Nested If Else Statement")
    {
        const auto result = R"(
            {
                "type": "IfStatement",
                "condition": { "type": "PrimaryExpression", "value": "true" },
                "then": 
                    {
                        "type": "IfStatement",
                        "condition": { "type": "PrimaryExpression", "value": "false" },
                        "then": 
                            {
                                "type": "PrintStatement",
                                "expression": { "type": "PrimaryExpression", "value": "5" }
                            },
                        "else":
                            {
                                "type": "PrintStatement",
                                "expression": { "type": "PrimaryExpression", "value": "10" }
                            }
                    },
                "else": {}
            }
        )"_json;
        lexer.scan(std::stringstream("if (true) if (false) print 5; else print 10;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("ParseLogicStatement", "[Stmt][And][Or]")
{
    Lexer lexer;
    ASTJSonVisitor jsonVisitor;
    SECTION("Logical And Statement")
    {
        const auto result = R"(
            {
                "type": "ExpressionStatement",
                "expression": {
                    "type": "BinaryExpression",
                    "lhs": { "type": "Variable", "value": "a" },
                    "operator": "&&",
                    "rhs": { "type": "Variable", "value": "b" }
                }
            }
        )"_json;
        lexer.scan(std::stringstream("a && b;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
     SECTION("Logical Or Statement")
    {
        const auto result = R"(
            {
                "type": "ExpressionStatement",
                "expression": {
                    "type": "BinaryExpression",
                    "lhs": { "type": "Variable", "value": "a" },
                    "operator": "||",
                    "rhs": { "type": "Variable", "value": "b" }
                }
            }
        )"_json;
        lexer.scan(std::stringstream("a || b;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("ParseLoopStatements", "[Stmt][Loop][While][For]")
{
    Lexer lexer;
    ASTJSonVisitor jsonVisitor;
    SECTION("While Loop")
    {
        const auto result = R"(
            {
                "type": "WhileStatement",
                "condition": { "type": "PrimaryExpression", "value": "true" },
                "body": {
                    "type": "PrintStatement",
                    "expression": { "type": "PrimaryExpression", "value": "10" }
                }
            }
        )"_json;
        lexer.scan(std::stringstream("while(true) print 10;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
    SECTION("For Loop")
    {
        const auto result = R"(
            {
                "type": "BlockStatement",
                "statements": [
                    {
                        "type": "VariableStatement",
                        "name": "i",
                        "initializer": { "type": "PrimaryExpression", "value": "0" }
                    },
                    {
                        "type": "WhileStatement",
                        "condition": {
                            "type": "BinaryExpression",
                            "operator": "<",
                            "lhs": { "type": "Variable", "value": "i" },
                            "rhs": { "type": "PrimaryExpression", "value": "10" }
                        },
                        "body": {
                            "type": "BlockStatement",
                            "statements": [
                                {
                                    "type": "PrintStatement",
                                    "expression": { "type": "Variable", "value": "i" }
                                },
                                {
                                    "type": "ExpressionStatement",
                                    "expression": {
                                        "type": "AssignmentExpression",
                                        "name": "i",
                                        "value": {
                                            "type": "BinaryExpression",
                                            "operator": "+",
                                            "lhs": { "type": "Variable", "value": "i" },
                                            "rhs": { "type": "PrimaryExpression", "value": "1" }
                                        }
                                    }
                                }
                            ]
                        }
                    }
                ]
            }
        )"_json;
        lexer.scan(std::stringstream("for (let i = 0; i < 10; i = i + 1) print i;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}