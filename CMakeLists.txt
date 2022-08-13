# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)
project ("bbtcompiler" LANGUAGES CXX VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#===============Libraries===============
add_subdirectory(libs/bbtcompilerlib)

#===============Applications============
add_subdirectory ("apps/bbtcompiler")
target_link_libraries(bbtcompiler PRIVATE bbtcompilerlib)
target_include_directories(bbtcompiler PRIVATE libs/bbtcompilerlib)

#===============Tests===================
find_package(Catch2 REQUIRED)
add_executable(tests "tests/testMain.cpp" "tests/testlexer.cpp" "tests/testParser.cpp" "tests/testParserOld.cpp")
target_link_libraries(tests PRIVATE Catch2::Catch2 bbtcompilerlib)
target_include_directories(tests PRIVATE libs/bbtcompilerlib)

include(CTest)
include(Catch)
catch_discover_tests(tests)
