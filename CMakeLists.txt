cmake_minimum_required(VERSION 3.14)

project(AnchovyEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Anchovy 
    src/main.cpp
    src/uci.hpp
    src/uci.cpp
    src/engine/piece.hpp
    src/engine/piece.cpp
    src/engine/board.hpp
    src/engine/board.cpp
    src/engine/engine.hpp
    src/engine/engine.cpp
    src/engine/attack.hpp
    src/engine/attack.cpp
)