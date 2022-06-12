set(
    HEADER_LIST
    "Lexer.h"
    "Parser.h"
    "Expression.h"
    "ASTVisitor.h"
    "Statement.h"
    "JsonVisitor.h" 
    "SymbolTable.h")
set(
    SRC_LIST
    "Lexer.cpp"
    "Parser.cpp"
    "JsonVisitor.cpp")
add_library(bbtcompilerlib ${HEADER_LIST} ${SRC_LIST})
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(bbtcompilerlib PRIVATE nlohmann_json nlohmann_json::nlohmann_json)
