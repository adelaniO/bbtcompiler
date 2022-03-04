#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include "Lexer.h"

namespace fs = std::filesystem;

void processFile(const char* filepath)
{
    const auto path = fs::path(filepath);
    BBTCompiler::Lexer lexer;
    std::ifstream fileStream(filepath);
    if(fileStream)
        lexer.scan(fileStream);
};

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: bbtcompiler filename\n";
    }

    const auto pathType = fs::status(argv[1]);
    if(pathType.type() == fs::file_type::regular)
    {
        processFile(argv[0]);
    }
    return 0;
}