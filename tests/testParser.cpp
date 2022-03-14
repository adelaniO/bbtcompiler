#include <sstream>
#include <iostream>
#include "catch.hpp"
#include "Parser.h"
#include "Expression.h"

using BBTCompiler::Lexer;
using BBTCompiler::Parser;
using BBTCompiler::Expr;
using BBTCompiler::BinaryExpr;
using BBTCompiler::ASTJSonVisitor;

TEST_CASE("ParseExpression", "[Expression]")
{
    std::stringstream ss("1*-2+3-(90-68)");
    Lexer lexer;
    lexer.scan(ss);
    auto parser = Parser(lexer.getTokens());
    std::unique_ptr<Expr> expression{parser.parse()};
    ASTJSonVisitor jsonVisitor;
    expression->accept(&jsonVisitor);
    jsonVisitor.print();
}