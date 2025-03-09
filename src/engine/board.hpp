#pragma once

#include "piece.hpp"
#include <optional>
#include <vector>

const uint8_t EMPTY = 32;

struct Board {
  std::vector<Piece> pieces;
  std::vector<std::vector<uint8_t>> board;
  bool last_move_double_step;
  bool black_castle_king;
  bool white_castle_king;
  
  static Board init();
  void make_move(Move &move);
  void get_moves(std::vector<Position *> &moves, Colour colour);
  void get_move(std::vector<Position*> &moves, Square &to, Square &from,
           MoveType type);
  bool is_occupied(Square &square);
  bool is_occupied(Square &square, Colour colour);
  bool in_bounds(Square &square);
  bool is_check(Colour colour);
  void display();
};

struct Position {
  Board board;
  Move move;
  std::vector<Position*> next;
};