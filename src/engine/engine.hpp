#pragma once
#include "board.hpp"
#include <optional>

using Eval = int16_t;
const Eval EvalMax = INT16_MAX;
const Eval EvalMin = INT16_MIN;

const Eval PieceValue[6] = {100, 300, 300, 500, 900, 0};

struct BestMove {
  Eval eval;
  bool end;
  Move move;
};

struct Engine {
  Board board;
  Move move;
  static Engine init();
  BestMove search_moves(int depth);
  void bench(int depth, bool debug);
  static Eval evaluate(Board &board, Colour colour);
};