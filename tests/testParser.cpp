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
        lexer.scan(std::stringstream("1*2-3;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        jsonVisitor.print();
        const auto& exprJson{ jsonVisitor.getJson() };
        CHECK(exprJson.contains("expression"));
    }

    SECTION("Assignment Expression")
    {
        lexer.scan(std::stringstream("let a = b;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        jsonVisitor.print();
        const auto& exprJson{ jsonVisitor.getJson() };
        CHECK(exprJson.contains("expression"));
    }
}

TEST_CASE("ParseStatements", "[Stmt][Statements]")
{
    Lexer lexer;
    ASTJSonVisitor jsonVisitor;
    SECTION("Binary Expression")
    {
        lexer.scan(std::stringstream("print 5;"));
        auto parser = Parser(lexer.getTokens());
        std::vector<std::unique_ptr<Stmt>>& statements{ parser.parse() };
        REQUIRE(statements.size() == 1);
        statements[0]->accept(jsonVisitor);
        jsonVisitor.print();
        const auto& exprJson{ jsonVisitor.getJson() };
        CHECK(exprJson.contains("expression"));
    }
}