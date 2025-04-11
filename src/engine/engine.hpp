#pragma once
#include "board.hpp"
#include "table.hpp"
#include <optional>

const Eval PieceValue[6] = {100, 300, 300, 500, 900, 0};

struct BestMove {
  Eval eval;
  bool end;
  Move move;
};

struct Engine {
  Board board;
  Move move;
  uint64_t static NodeCount;
  
  static Table table;
  static Engine init();
  BestMove search_moves(uint8_t max_depth, double time, bool debug);
  void perft(uint8_t depth);
  static Eval evaluate(Board &board, Colour colour);
  static uint64_t get_timestamp();
};