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
  Square(uint8_t x, uint8_t y) : x(x), y(y){};
  bool operator==(const Square &square) {
    return this->x == square.x && this->y == square.y;
  };
};

struct Move;
struct Board;

struct Piece {
  uint8_t id;
  PieceType type;
  Colour colour;
  Square square;
  bool taken;
  Piece(uint8_t id, PieceType type, Colour colour, Square square)
      : id(id), type(type), colour(colour), square(square), taken(false){};
  std::vector<Move> get_moves(Board &board);
};

struct Move {
  Piece piece;
  std::optional<Piece> takes;
  Square from, to;
  MoveType type;
  Move(Piece piece, std::optional<Piece> takes, Square from, Square to, MoveType type)
      : piece(piece), takes(takes), from(from), to(to), type(type){};
};