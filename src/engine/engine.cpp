#pragma once

#include "engine.hpp"
#include <iostream>

Engine Engine::init() {
  Board *board = new Board{};
  *board = Board::init();
  Position *move = new Position{board, Move{}};
  Board *display = new Board{};
  *display = Board::init();
  return Engine{display, move};
}

void get_moves_inner(int depth, Position *move) {
  if (depth > 0) {
    if (move->board != NULL) {
      move->board->get_moves(move->next, opposite(move->move.piece.colour));
      delete move->board;
      move->board = NULL;
    }
    for (size_t i = 0; i < move->next.size(); i++) {
      get_moves_inner(depth - 1, move->next[i]);
    }
  }
}

void Engine::get_moves(int depth) { get_moves_inner(depth, this->move); }

int8_t search_moves_inner(int depth, Position *&move) {
  if (depth == 0) {
    // std::cout << static_cast<int16_t>(move->board->evaluate(move->move.piece.colour)) << ", ";
    return move->board->evaluate(move->move.piece.colour);
  }
  int8_t max = -120;
  for (auto &move : move->next) {
    // std::cout << move->move.get_name() << " ";
    int score = -search_moves_inner(depth - 1, move);
    if (score > max) {
      max = score;
    }
  }
  // printf("\n");
  return max;
}

Position *Engine::search_moves(int depth) {
  Position *best = NULL;
  int8_t max = -120;
  for (auto &move : this->move->next) {
    // std::cout << move->move.get_name() << " - ";
    move->board = NULL;
    int8_t score = -search_moves_inner(depth - 1, move);
    if (score > max) {
      max = score;
      best = move;
    }
  }
  // std::cout << best->move.get_name();
  return best;
}