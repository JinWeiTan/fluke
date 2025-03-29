#pragma once

#include "piece.hpp"

enum PinType : uint8_t { None, Horizontal, Vertical, RDiagonal, LDiagonal };

struct Attacks {
  PinType pins[32];
  bool attacks[8][8];
  bool checks[8][8];
  bool check;
  bool double_check;
  bool is_safe(Square &square);
};