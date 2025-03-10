#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <string>

enum Colour { Black, White };

inline Colour opposite(Colour colour) { return static_cast<Colour>(!colour); };

enum PieceType { Pawn, Knight, Bishop, Rook, Queen, King };

const int PieceValue[6] = {1, 3, 3, 5, 9, 80};
const std::string PieceName[6] = {"", "N", "B", "R", "Q", "K"};
const std::string FileName[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
const std::string RankName[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};

enum MoveType { Step, DoubleStep, EnPassant, Castle, Promotion };

struct Square {
  uint8_t x, y;
  std::string get_name();
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
  void get_moves(Board &board, std::vector<Position *> &moves);
};

struct Move {
  Piece piece;
  std::optional<Piece> takes;
  Square from, to;
  MoveType type;
  std::string get_name();
};