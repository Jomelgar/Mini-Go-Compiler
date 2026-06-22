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
    src/tree.cpp
    src/interpreter.cpp
    include/enums/NodeKind.hpp
    include/enums/relational.hpp
    include/enums/unary.hpp
    include/enums/type.hpp
)