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
