#pragma once

#include "piece.hpp"
#include <optional>
#include <vector>

const uint8_t EMPTY = 32;

struct Castling {
  bool white;
  bool black;
};

struct Board {
  std::vector<Piece> pieces;
  std::vector<std::vector<uint8_t>> board;
  Castling castling;
  bool last_move_double_step;
  
  static Board init();
  void make_move(Move &move);
  void get_moves(std::vector<Position *> &moves, Colour colour);
  void get_move(std::vector<Position*> &moves, Square &from, Square &to,
           MoveType type);
  bool is_occupied(Square &square);
  bool is_occupied(Square &square, Colour colour);
  bool in_bounds(Square &square);
  bool is_check_at(Square &square, Colour colour);
  bool is_check(Colour colour);
  int8_t evaluate(Colour colour);
  void display();
};

struct Position {
  Board* board;
  Move move;
  std::vector<Position*> next;
  bool evaluated;
  void get_moves();
};