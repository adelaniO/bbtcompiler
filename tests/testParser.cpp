#include <iostream>
#include <fstream>
#include <filesystem>
#include "catch.hpp"

#include "testConfigureFile.h"
#include "Parser.h"


using namespace BBTCompiler;
namespace fs = std::filesystem;

TEST_CASE("ParseExpression", "[Expression]")
{
    const auto testDataDir = BBT_SOURCE_DIR_STRING "/tests/testData/parser";
    INFO("test parser directory: " << testDataDir);
    REQUIRE(fs::is_directory(testDataDir));
    for (const auto& entry : fs::directory_iterator(testDataDir)) {
        const auto filenameStr = entry.path().string();
        const auto fileextentionStr = entry.path().filename().extension().string();
        if (entry.is_regular_file() && fileextentionStr == ".txt") {
            auto expectedFilePath{ entry.path() };
            expectedFilePath.replace_extension(".json");
            REQUIRE(fs::is_regular_file(expectedFilePath));
            //SECTION(filenameStr)
            {
                Parser parser;
                AST& tree = parser.parse(std::ifstream(filenameStr));
                json resultjson = tree;
                json expectedjson = json::parse(std::ifstream(expectedFilePath));
                CHECK(json::diff(resultjson, expectedjson) == json::array({}));
                std::cout << resultjson.dump(4) << std::endl;
                std::cout << expectedjson.dump(4) << std::endl;
            }
        }
    }


    //Parser parser;
    //AST& tree = parser.parse("1*2-3;");
    //json j = tree;
    //std::cout << j.dump(4) << std::endl;
}