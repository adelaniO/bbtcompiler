#include <sstream>
#include <iostream>
#include "catch.hpp"

#include "Parser.h"

using namespace BBTCompiler;


    json print(const size_t iNode, const AST& tree)
    {
        const ASTVariant& astNode{ tree[iNode].getVariant() };
        json j;
        if (std::holds_alternative<Token>(astNode)) {
            const auto& node{ std::get<Token>(astNode) };
            j = json{ {"type", "Token" }, {"value", node.value }, {"line", node.position.line}, {"column", node.position.column} };
        }
        else if (std::holds_alternative<BinaryExpr>(astNode)) {
            const auto& node{ std::get<BinaryExpr>(astNode) };
            ;
            node.m_operator;
            node.m_right;
            j = json{ {"type", "BinaryExpr"}};
            j["left"] = print(node.m_left, tree);
            j["operator"] = print(node.m_operator, tree);
            j["right"] = print(node.m_right, tree);
        }
        else if(tree[iNode].hasNext())
        {
            j = print(tree[iNode].next(), tree);
        }
        else {
            j["type"] = nullptr;
        }
        return j;
    }

TEST_CASE("ParseExpression", "[Expression]")
{
    Parser parser;
    AST& tree = parser.parse("1*2-3;");
    const auto j = print(0, tree);
    std::cout << j.dump(4) << std::endl;
}