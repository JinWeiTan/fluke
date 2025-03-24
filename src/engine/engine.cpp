#pragma once

#include "engine.hpp"
#include <iostream>

struct BestMove {
  Eval eval;
  int move;
};

Eval WhitePawnWeights[8][8];
Eval BlackPawnWeights[8][8];
Eval PieceWeights[8][8];
Eval KingWeights[8][8];

Engine Engine::init() {
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      int value = std::max(std::abs(3.5 - i), std::abs(3.5 - j));
      WhitePawnWeights[i][j] = i;
      BlackPawnWeights[i][j] = 7 - i;
      PieceWeights[i][j] = 3 - value + 3;
      KingWeights[i][j] = value + 3;
    }
  }
  Position *move = new Position{Move{}};
  return Engine{Board::init(), move};
}

BestMove search_moves_inner(int depth, Position *move, Board &board, int alpha,
                            int beta) {
  if (depth == 0) {
    return BestMove{Engine::evaluate(board, move->move.colour), -1};
  }
  if (!move->generated) {
    board.get_moves(move->next, opposite(move->move.colour));
    move->generated = true;
  }
  if (move->next.size() == 0) {
    return BestMove{-10000, -1};
  }

  BestMove best = BestMove{EvalMin, -1};
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
  return search_moves_inner(depth, this->move, this->board, EvalMin, EvalMax)
      .move;
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

Eval Engine::evaluate(Board &board, Colour colour) {
  Eval eval = 0;
  for (auto &&piece : board.pieces) {
    if (!piece.taken) {
      Eval weights;
      if (piece.type == PieceType::Pawn) {
        if (piece.colour == Colour::White) {
          weights = WhitePawnWeights[piece.square.x][piece.square.y];
        } else {
          weights = BlackPawnWeights[piece.square.x][piece.square.y];
        }
      } else if (piece.type == PieceType::King) {
        weights = KingWeights[piece.square.x][piece.square.y];
      } else {
        weights = PieceWeights[piece.square.x][piece.square.y];
      }
      eval += (PieceValue[piece.type] + weights) *
              ((piece.colour == colour) ? -1 : 1);
    }
  }
  return eval;
}
