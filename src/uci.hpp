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
const std::string HorizontalName[8] = {"h", "g", "f", "e", "d", "c", "b", "a"};
const std::string VerticalName[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};
const std::map<char, int> PromoteMap = {{'n', 0}, {'b', 1}, {'r', 2}, {'q', 3},
                                        {'N', 0}, {'B', 1}, {'R', 2}, {'Q', 3}};
const std::string PromoteName[8] = {"n", "b", "r", "q", "N", "B", "R", "Q"};
const std::string BoardName[12] = {"p", "n", "b", "r", "q", "k",
                                   "P", "N", "B", "R", "Q", "K"};