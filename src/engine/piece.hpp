#pragma once

#include <cstdint>
#include <optional>
#include <vector>

enum Colour { Black, White };

inline Colour opposite(Colour colour) { return static_cast<Colour>(!colour); };

enum PieceType { Pawn, Knight, Bishop, Rook, Queen, King };

enum MoveType { Step, DoubleStep, EnPassant, Castle, Promotion };

struct Square {
  uint8_t x, y;
  bool operator==(const Square &square) {
    return this->x == square.x && this->y == square.y;
  };
};

struct Move;
struct Board;
struct Position;

struct Piece {
  uint8_t id;
  PieceType type;
  Colour colour;
  Square square;
  bool taken;
  void get_moves(Board &board, std::vector<Position*> &moves);
};

struct Move {
  Piece piece;
  std::optional<Piece> takes;
  Square from, to;
  MoveType type;
};