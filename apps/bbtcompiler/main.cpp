#pragma once

#include "iostream"
#include "Lexer.h"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: bbtcompiler filename\n";
    }

    std::cout << "Hello CMake." << std::endl;
    return 0;
}