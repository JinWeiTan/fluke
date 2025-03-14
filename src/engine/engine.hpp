#pragma once
#include <optional>
#include "board.hpp"

using Eval = int16_t;

const Eval PieceValue[6] = {10, 30, 30, 50, 90, 800};

struct Engine {
    Board board;
    Position* move;
    static Engine init();
    void make_move(int move);
    int search_moves(int depth);
    void clean_moves(int except);
    static Eval evaluate(Board &board, Colour colour);
};