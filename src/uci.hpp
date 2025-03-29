#pragma once

#include "engine/engine.hpp"
#include <map>
#include <string>

struct UCI {
  Engine engine;
  bool started;

  void static init();
};

template <typename T, typename U> struct Iterator {
  T list;
  int count;

  U next();
  bool end();
};

using Commands = Iterator<std::vector<std::string>, std::string>;

using FEN = Iterator<std::string, char>;

const std::map<char, int> FENPieceMap = {
    {'p', 16}, {'n', 24}, {'b', 26}, {'r', 28}, {'q', 30}, {'k', 31},
    {'P', 0},  {'N', 8},  {'B', 10}, {'R', 12}, {'Q', 14}, {'K', 15}};
const std::map<char, int> PromoteMap = {{'n', 0}, {'b', 1}, {'r', 2}, {'q', 3},
                                        {'N', 0}, {'B', 1}, {'R', 2}, {'Q', 3}};