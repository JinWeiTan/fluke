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
    return move->board->evaluate(move->move.piece.colour);
  }
  int8_t max = -120;
  for (auto &move : move->next) {
    int score = -search_moves_inner(depth - 1, move);
    if (score > max) {
      max = score;
    }
  }
  return max;
}

int Engine::search_moves(int depth) {
  int best = -1;
  int8_t max = -120;
  for (int i = 0; i < this->move->next.size(); i++) {
    Position *move = this->move->next[i];
    move->board = NULL;
    int8_t score = -search_moves_inner(depth - 1, move);
    if ((move->move.to.x == 3 || move->move.to.x == 4) &&
        (move->move.to.y == 3 || move->move.to.y == 4)) {
      score += 1;
    }
    if (score > max) {
      max = score;
      best = i;
    }
  }
  return best;
}

void clean_moves_inner(Position *move) {
  for (int i = 0; i < move->next.size(); i++) {
    clean_moves_inner(move->next[i]);
  }
  if (move->board != NULL) {
    delete move->board;
  }
  delete move;
}

void Engine::clean_moves(int except) {
  for (int i = 0; i < this->move->next.size(); i++) {
    if (i != except) {
      clean_moves_inner(this->move->next[i]);
    }
  }
}