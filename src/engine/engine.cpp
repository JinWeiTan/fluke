#pragma once

#include "engine.hpp"
#include <iostream>

Eval WhitePawnWeights[8][8];
Eval BlackPawnWeights[8][8];
Eval PieceWeights[8][8];
Eval KingWeights[8][8];

Engine Engine::init() {
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      // int value = std::max(std::abs(3.5 - i), std::abs(3.5 - j));
      int value = sqrt(pow((3.5 - i), 2) + pow((3.5 - j), 2));
      WhitePawnWeights[i][j] = i;
      BlackPawnWeights[i][j] = 7 - i;
      PieceWeights[i][j] = 4 - value + 3;
      KingWeights[i][j] = value + 3;
    }
  }
  return Engine{Board::init(), Move{}};
}

BestMove search_moves_inner(int depth, Move &move, Board &board, int alpha,
                            int beta) {
  if (depth == 0) {
    return BestMove{Engine::evaluate(board, move.colour), false};
  }
  std::vector<Move> moves;
  board.get_moves(moves, opposite(move.colour));
  if (moves.size() == 0) {
    return BestMove{-10000, true};
  }

  BestMove best = BestMove{EvalMin, false};
  for (int i = 0; i < moves.size(); i++) {
    // Autoqueen optimization
    if (moves[i].type >= MoveType::PromoteKnight &&
        moves[i].type != MoveType::PromoteQueen) {
      continue;
    }

    Board new_board = board.make_move(moves[i]);
    BestMove result =
        search_moves_inner(depth - 1, moves[i], new_board, -beta, -alpha);
    if (-result.eval > best.eval) {
      best.eval = -result.eval;
      best.move = moves[i];
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

BestMove Engine::search_moves(int depth) {
  BestMove move = search_moves_inner(depth, this->move, this->board, EvalMin, EvalMax);
  return move;
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
