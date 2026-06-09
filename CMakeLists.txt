cmake_minimum_required(VERSION 3.20)
project(Gol)

set(CMAKE_CXX_STANDARD 20)
add_executable( Gol
    main.cpp 
    include/lexer.hpp 
    src/lexer.cpp
    include/parser.hpp
    src/parser.cpp
    include/tree.hpp
    include/enums/NodeKind.hpp
    include/enums/type.hpp
)