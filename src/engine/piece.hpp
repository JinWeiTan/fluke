#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

enum Colour : uint8_t { Black, White };

inline Colour opposite(Colour colour) { return static_cast<Colour>(!colour); };

enum PieceType : uint8_t { Pawn, Knight, Bishop, Rook, Queen, King };

enum MoveType : uint8_t {
  Step,
  DoubleStep,
  EnPassant,
  Castle,
  PromoteKnight,
  PromoteBishop,
  PromoteRook,
  PromoteQueen
};

struct Square {
  uint8_t x, y;
  bool operator==(const Square &square) {
    return this->x == square.x && this->y == square.y;
  };
};

struct Attacks;
struct Board;
struct Position;

struct Piece {
  uint8_t id;
  PieceType type;
  Colour colour;
  Square square;
  bool taken;
  void get_moves(Board &board, std::vector<Position *> &moves,
                 Attacks &attacks);
  void get_attacks(Board &board, Attacks &attacks);
};

struct Move {
  uint8_t piece_id;
  PieceType piece;
  Colour colour;
  Square from, to;
  MoveType type;
  bool takes;
};