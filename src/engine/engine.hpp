#pragma once
#include "board.hpp"
#include "table.hpp"
#include <optional>

const Eval PieceValue[6] = {100, 300, 300, 500, 900, 0};

struct Mode {
  int8_t depth;
  int qsearch;
};

struct BestMove {
  Eval eval;
  bool end;
  Move move;
};

extern uint64_t NodeCount;

struct Engine {
  Board board;
  Move move;
  
  static Table table;
  static Engine init();
  BestMove search_moves(Mode mode);
  void perft(int8_t depth);
  static Eval evaluate(Board &board, Colour colour);
  static uint64_t get_timestamp();
};