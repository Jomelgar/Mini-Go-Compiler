cmake_minimum_required(VERSION 3.20)
project(Gol)

set(CMAKE_CXX_STANDARD 20)
add_executable( Gol
    src/main.cpp 
    src/lexer.hpp 
    src/lexer.cpp
    src/parser.hpp
    src/parser.cpp
)