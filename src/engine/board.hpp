#pragma once

#include "piece.hpp"
#include <optional>
#include <vector>

const uint8_t EMPTY = 32;

struct Board {
  std::vector<Piece> pieces;
  std::vector<std::vector<uint8_t>> board;
  std::vector<Move> moves;
  Board();
  std::vector<Move> get_moves(Colour colour);
  Move get_move(Square &to, Square &from, MoveType type);
  std::vector<Move> get_moves_depth(Colour colour);
  void make_move(Move &move);
  bool is_occupied(Square &square);
  bool is_occupied(Square &square, Colour colour);
  bool in_bounds(Square &square);
  bool is_check(Colour colour);
  void display();
};