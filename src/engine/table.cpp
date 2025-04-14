#include "table.hpp"
#include <iostream>

uint64_t Random::generate() {
  uint64_t x = this->a;
  x ^= x << 13;
  x ^= x >> 7;
  x ^= x << 17;
  return this->a = x;
}

Table Table::init() {
  std::vector<TableEntry> vtable(TableSize);
  Table table = Table{vtable, Random{0x123456789ABCDEF1}};
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      for (size_t k = 0; k < 2; k++) {
        for (size_t l = 0; l < 6; l++) {
          table.hash.board[i][j][k][l] = table.random.generate();
        }
      }
    }
  }
  table.hash.colour = table.random.generate();
  for (size_t i = 0; i < 4; i++) {
    table.hash.castling[i] = table.random.generate();
  }
  for (size_t i = 0; i < 8; i++) {
    table.hash.double_step[i] = table.random.generate();
  }
  return table;
}

uint64_t Table::get_hash(Board &board, Colour colour) {
  uint64_t hash = 0;
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      if (board.board[i][j] != EMPTY) {
        Piece &piece = board.pieces[board.board[i][j]];
        hash ^= this->hash.board[i][j][piece.colour][piece.type];
      }
    }
  }
  if (colour == Colour::White) {
    hash ^= this->hash.colour;
  }
  if (board.double_step < 9) {
    hash ^= this->hash.double_step[board.double_step];
  }
  if (board.castling.white_kingside) {
    hash ^= this->hash.castling[0];
  }
  if (board.castling.white_queenside) {
    hash ^= this->hash.castling[1];
  }
  if (board.castling.black_kingside) {
    hash ^= this->hash.castling[2];
  }
  if (board.castling.black_queenside) {
    hash ^= this->hash.castling[3];
  }
  return hash;
}

bool Table::has_entry(uint64_t hash) {
  return this->table[hash % TableSize].hash == hash;
};

TableEntry Table::get_entry(uint64_t hash) {
  return this->table[hash % TableSize];
};

void Table::set_entry(uint64_t hash, TableEntry entry) {
  entry.hash = hash;
  this->table[hash % TableSize] = entry;
};