#include <sstream>
#include <iostream>
#include <filesystem>
#include "catch.hpp"

#include "Parser.h"

using namespace BBTCompiler;
namespace fs = std::filesystem;

TEST_CASE("ParseExpression", "[Expression]")
{
    std::cout << "Current path is " << fs::current_path() << '\n'; // (1)

    Parser parser;
    AST& tree = parser.parse("1*2-3;");
    json j = tree;
    std::cout << j.dump(4) << std::endl;
}