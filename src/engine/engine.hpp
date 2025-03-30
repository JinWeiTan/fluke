#pragma once
#include "board.hpp"
#include <optional>

using Eval = int16_t;
const Eval EvalMax = INT16_MAX;
const Eval EvalMin = INT16_MIN;

const Eval PieceValue[6] = {10, 30, 30, 50, 90, 0};

struct BestMove {
  Eval eval;
  bool end;
  Move move;
};

extern uint64_t NodeCount;

struct Engine {
  Board board;
  Move move;
  static Engine init();
  BestMove search_moves(int depth);
  void perft(int depth);
  static Eval evaluate(Board &board, Colour colour);
  static uint64_t get_timestamp();
};