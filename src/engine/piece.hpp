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
  std::string format();
};

struct Attacks;
struct Board;
struct Move;

struct Piece {
  uint8_t id;
  PieceType type;
  Colour colour;
  Square square;
  bool taken;
  void get_moves(Board &board, std::vector<Move> &moves, Attacks &attacks);
  void get_attacks(Board &board, Attacks &attacks);
};

struct Move {
  uint8_t move_id;
  PieceType piece;
  Colour colour;
  Square from, to;
  MoveType type;
  bool takes;
  int8_t score;
  std::string format();
};

const std::string PromoteName[8] = {"n", "b", "r", "q", "N", "B", "R", "Q"};
const std::string HorizontalName[8] = {"h", "g", "f", "e", "d", "c", "b", "a"};
const std::string VerticalName[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};
