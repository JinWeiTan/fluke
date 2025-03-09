#pragma once
#include <optional>
#include "board.hpp"

struct Engine {
    Position* move;
    static Engine init();
};