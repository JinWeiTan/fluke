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


if (WIN32)

target_include_directories(AnchovyServer PRIVATE vcpkg_installed/x64-windows/include)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(AnchovyServer
    PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
)

endif (WIN32)


if (UNIX)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")

target_include_directories(AnchovyServer PRIVATE vcpkg_installed/x64-linux/include)
target_link_directories(AnchovyServer PRIVATE vcpkg_installed/x64-linux/lib)
target_link_libraries(AnchovyServer PRIVATE uSockets z)

endif (UNIX)