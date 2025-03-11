#pragma once
#include <optional>
#include "board.hpp"

struct Engine {
    Board* board;
    Position* move;
    static Engine init();
    void get_moves(int depth);
    int search_moves(int depth);
    void clean_moves(int except);
};