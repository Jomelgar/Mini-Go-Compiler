cmake_minimum_required(VERSION 3.20)
project(Gol)

set(CMAKE_CXX_STANDARD 20)
add_executable( Gol
    main.cpp 
    lexer.hpp 
    lexer.cpp
    parser.hpp
    parser.cpp
)