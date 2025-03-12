#pragma once

#include "engine.hpp"
#include <iostream>

struct BestMove {
  int eval;
  int move;
};

Engine Engine::init() {
  Board *board = new Board{};
  *board = Board::init();
  Position *move = new Position{board, Move{}};
  Board *display = new Board{};
  *display = Board::init();
  return Engine{display, move};
}

BestMove search_moves_inner(int depth, Position *move, int alpha, int beta) {
  if (depth == 0) {
    return BestMove{move->board->evaluate(move->move.piece.colour), -1};
  }
  move->get_moves();
  BestMove best = BestMove{-120, -1};
  for (int i = 0; i < move->next.size(); i++) {
    int eval = -search_moves_inner(depth - 1, move->next[i], -beta, -alpha).eval;
    if (eval > best.eval) {
      best.eval = eval;
      best.move = i;
    }
    if (best.eval > alpha) {
      alpha = best.eval;
    }
    if (alpha > beta) {
      break;
    }
  }
  return best;
}

int Engine::search_moves(int depth) {
  return search_moves_inner(depth, this->move, -120, 120).move;
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

void Engine::make_move(int move) {
  this->move = this->move->next[move];
  this->board->make_move(this->move->move);
}