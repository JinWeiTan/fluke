cmake_minimum_required(VERSION 3.14)

project(AnchovyEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AnchovyServer 
    src/main.cpp
    src/server.hpp
    src/server.cpp
    src/message.hpp
    src/message.cpp
    src/engine/piece.hpp
    src/engine/piece.cpp
    src/engine/board.hpp
    src/engine/board.cpp
    src/engine/engine.hpp
    src/engine/engine.cpp
)

target_include_directories(AnchovyServer PRIVATE "${CMAKE_CURRENT_SOURCE}vcpkg_installed/x64-windows/include")

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(AnchovyServer
    PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
)
