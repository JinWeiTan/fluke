#pragma once
#include <optional>
#include "board.hpp"

using Eval = int16_t;
const Eval EvalMax = INT16_MAX;
const Eval EvalMin = INT16_MIN;

const Eval PieceValue[6] = {10, 30, 30, 50, 90, 800};

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
    static Eval evaluate(Board &board, Colour colour);
};