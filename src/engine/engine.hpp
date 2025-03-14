#pragma once
#include <optional>
#include "board.hpp"

struct Engine {
    Board board;
    Position* move;
    static Engine init();
    void make_move(int move);
    int search_moves(int depth);
    void clean_moves(int except);
};