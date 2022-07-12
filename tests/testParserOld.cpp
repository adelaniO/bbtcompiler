#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "catch.hpp"
#include "ParserOld.h"
#include "Statement.h"
#include "JsonVisitor.h"

using BBTCompiler::Lexer;
using BBTCompiler::ParserOld;
using BBTCompiler::Expr;
using BBTCompiler::BinaryExpr;
using BBTCompiler::ASTJSonVisitor;
using BBTCompiler::Stmt;

TEST_CASE("Old-ParseExpression", "[Expression]")
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
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
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
        lexer.scan(std::stringstream("let a: int;"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
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
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
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
        lexer.scan(std::stringstream("let a : int = b;"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("Old-ParseStatements", "[Stmt][Statements]")
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
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("Old-ParseBlockStatements", "[Stmt][Block]")
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
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("Old-ParseIfStatement", "[Stmt][If]")
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
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
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
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
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
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("Old-ParseLogicStatement", "[Stmt][And][Or]")
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
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
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
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("Old-ParseLoopStatements", "[Stmt][Loop][While][For]")
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
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
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
        lexer.scan(std::stringstream("for (let i : int = 0; i < 10; i = i + 1) print i;"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("Old-CallExpression", "[Call][Functions]")
{
    Lexer lexer;
    ASTJSonVisitor jsonVisitor;
    SECTION("Call Without Arguements")
    {
        const auto result = R"(
            {
                "type": "ExpressionStatement",
                "expression": {
                    "type": "CallExpression",
                    "callee": { "type": "Variable", "value": "test" },
                    "arguements": []
                }
            }
        )"_json;
        lexer.scan(std::stringstream("test();"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }

    SECTION("Call With 3 Arguements")
    {
        const auto result = R"(
            {
                "type": "ExpressionStatement",
                "expression": {
                    "type": "CallExpression",
                    "callee": { "type": "Variable", "value": "test" },
                    "arguements": [
                            { "type": "Variable", "value": "a" },
                            { "type": "PrimaryExpression", "value": "1" },
                            { "type": "Variable", "value": "b" }
                        ]
                }
            }
        )"_json;
        lexer.scan(std::stringstream("test(a, 1, b);"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("Old-FunctionStatment", "[definition][Functions]")
{
    Lexer lexer;
    ASTJSonVisitor jsonVisitor;
    SECTION("Function With 0 Args")
    {
        const auto result = R"(
            {
                "type": "FunctionStatement",
                "name": "test",
                "returnType": "int",
                "parameters": [],
                "statements": []
            }
        )"_json;
        lexer.scan(std::stringstream("fn test() -> int {}"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
    SECTION("Function With 1 Arg")
    {
        const auto result = R"(
            {
                "type": "FunctionStatement",
                "name": "test",
                "returnType": "void",
                "parameters": [ { "name" : "a", "type" : "int" } ],
                "statements": []
            }
        )"_json;
        lexer.scan(std::stringstream("fn test(a: int) {}"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
    SECTION("Function With 3 Args")
    {
        const auto result = R"(
            {
                "type": "FunctionStatement",
                "name": "test",
                "returnType": "void",
                "parameters": [
                    { "name" : "a", "type" : "int" },
                    { "name" : "b", "type" : "char" },
                    { "name" : "c", "type" : "float" }
                ],
                "statements": []
            }
        )"_json;
        lexer.scan(std::stringstream("fn test(a: int, b: char,c:float) {}"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

TEST_CASE("Old-ReturnStatment", "[return]")
{
    Lexer lexer;
    ASTJSonVisitor jsonVisitor;
    SECTION("Empty return")
    {
        const auto result = R"(
            {
                "type": "ReturnStatement",
                "body": {}
            }
        )"_json;
        lexer.scan(std::stringstream("return;"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
    SECTION("Return Constant")
    {
        const auto result = R"(
            {
                "type": "ReturnStatement",
                "body": {
                    "type": "PrimaryExpression",
                    "value": "4"
                }
            }
        )"_json;
        lexer.scan(std::stringstream("return 4;"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
    SECTION("Return Expression")
    {
        const auto result = R"(
            {
                "type": "ReturnStatement",
                "body": {
                    "type": "BinaryExpression",
                    "lhs": {
                        "type": "PrimaryExpression",
                        "value": "2"
                    },
                    "operator": "*",
                    "rhs": {
                        "type": "PrimaryExpression",
                        "value": "2"
                    }
                }
            }
        )"_json;
        lexer.scan(std::stringstream("return 2*2;"));
        auto parser = ParserOld(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        INFO(jsonVisitor.toString());
        CHECK(nlohmann::json::diff(result, jsonVisitor.getJson()) == nlohmann::json::array({}));
    }
}

