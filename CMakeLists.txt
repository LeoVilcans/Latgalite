cmake_minimum_required(VERSION 3.16)
project(Latgalite)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Latgalite main.cpp
        token.h
        lexer.h
        lexer.cpp
        interpreter.h
        interpreter.cpp)

include(CTest)

if(BUILD_TESTING)
    add_executable(LatgaliteTests
            tests/interpreter_tests.cpp
            lexer.cpp
            interpreter.cpp)
    target_include_directories(LatgaliteTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME LatgaliteBuiltins COMMAND LatgaliteTests)
endif()
