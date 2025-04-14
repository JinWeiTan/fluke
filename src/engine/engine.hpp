#pragma once
#include "board.hpp"
#include "table.hpp"
#include <optional>
#include <chrono>

const Eval PieceValue[6] = {100, 300, 300, 500, 900, 0};

struct BestMove {
  Eval eval;
  bool end;
  Move move;
  bool cancelled;
};

struct Timer {
  std::chrono::steady_clock::time_point start_time;
  void start();
  double stop();
};

struct Engine {
  Board board;
  Move move;
  Timer timer;
  uint64_t static NodeCount;

  static Table table;
  static Engine init();
  BestMove search_moves(uint8_t max_depth, double time, bool debug);
  BestMove search_moves_depth(uint8_t max_depth);
  void perft(int8_t depth);
  static Eval evaluate(Board &board, Colour colour);
};