#pragma once

#include "engine.hpp"
#include <iostream>

struct BestMove {
  int eval;
  int move;
};

Engine Engine::init() {
  Position *move = new Position{Move{}};
  return Engine{Board::init(), move};
}

BestMove search_moves_inner(int depth, Position *move, Board &board, int alpha,
                            int beta) {
  if (depth == 0) {
    return BestMove{board.evaluate(move->move.piece.colour), -1};
  }
  board.get_moves(move->next, opposite(move->move.piece.colour));
  BestMove best = BestMove{-120, -1};
  for (int i = 0; i < move->next.size(); i++) {
    Board next = board.make_move(move->next[i]->move);
    BestMove result =
        search_moves_inner(depth - 1, move->next[i], next, -beta, -alpha);
    if (-result.eval > best.eval) {
      best.eval = -result.eval;
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
  return search_moves_inner(depth, this->move, this->board, -120, 120).move;
}

void clean_moves_inner(Position *move) {
  for (int i = 0; i < move->next.size(); i++) {
    clean_moves_inner(move->next[i]);
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
  this->board = this->board.make_move(this->move->move);
}