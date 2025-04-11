#pragma once

#include <unordered_map>
#include "board.hpp"

using Eval = int16_t;
const Eval EvalMax = INT16_MAX;
const Eval EvalMin = INT16_MIN;

enum EntryType : uint8_t { Exact, Upper, Lower };

struct TableEntry {
  Eval eval;
  int8_t depth;
  EntryType type;
  uint64_t hash;
};

struct Random {
  uint64_t a;
  uint64_t generate();
};

struct Hash {
  uint64_t board[8][8][2][6];
  uint64_t colour;
  uint64_t castling[4];
  uint64_t double_step[4];
};

static const size_t TableSize = 0x10000;

struct Table {
  std::vector<TableEntry> table;
  Random random;
  Hash hash;

  Table static init();
  bool has_entry(uint64_t hash);
  TableEntry get_entry(uint64_t hash);
  void set_entry(uint64_t hash, TableEntry entry);
  uint64_t get_hash(Board &board, Colour colour);
};