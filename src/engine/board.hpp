#pragma once

#include "piece.hpp"
#include "attack.hpp"
#include <optional>
#include <vector>

const uint8_t EMPTY = 32;

struct Castling {
  bool white_kingside;
  bool white_queenside;
  bool black_kingside;
  bool black_queenside;
};

struct Board {
  Piece pieces[32];
  uint8_t board[8][8];
  Castling castling;
  uint8_t double_step;
  
  static Board init();
  Board make_move(Move &move);
  void get_moves(std::vector<Move> &moves, Colour colour);
  bool get_move(std::vector<Move> &moves, Square &from, Square &to,
           MoveType type, Attacks &attacks);
  bool is_occupied(Square &square);
  bool is_occupied(Square &square, Colour colour);
  bool in_bounds(Square &square);
  bool is_check(Colour colour);
};