#include "engine.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <math.h>

Eval WhitePawnWeights[8][8];
Eval BlackPawnWeights[8][8];
Eval PieceWeights[8][8];
Eval WhiteKingWeights[8][8];
Eval BlackKingWeights[8][8];
// clang-format off
Eval KingEndWeights[8][8] = {
  -20, -10, -10, -10, -10, -10, -10, -20,
  -5,   0,   5,   5,   5,   5,   0,  -5,
  -10, -5,   20,  30,  30,  20,  -5, -10,
  -15, -10,  35,  45,  45,  35, -10, -15,
  -20, -15,  30,  40,  40,  30, -15, -20,
  -25, -20,  20,  25,  25,  20, -20, -25,
  -30, -25,   0,   0,   0,   0, -25, -30,
  -50, -30, -30, -30, -30, -30, -30, -50
};
// clang-format on

uint64_t NodeCount = 0;
Table Engine::table = Table::init();

Engine Engine::init() {
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      // int value = std::max(std::abs(3.5 - i), std::abs(3.5 - j));
      int value = std::sqrt(std::pow((3.5 - i), 2) + std::pow((3.5 - j), 2));
      WhitePawnWeights[i][j] = j * 5;
      BlackPawnWeights[i][j] = (7 - j) * 5;
      if ((i == 2 || i == 3) && !(j == 3 || j == 4)) {
        WhitePawnWeights[j][i] = -5;
      };
      if ((i == 4 || i == 5) && !(j == 3 || j == 4)) {
        BlackPawnWeights[j][i] = -5;
      };
      PieceWeights[i][j] = (4 - value) * 5;
      BlackKingWeights[i][j] = j * 20;
      WhiteKingWeights[i][j] = (7 - j) * 20;
    }
  }
  return Engine{Board::init(), Move{}};
}

BestMove search_moves_inner(int8_t depth, Move &move, Board &board, int alpha,
                            int beta, Mode mode) {
  NodeCount += 1;

  int alphaOrig = alpha;
  if (Engine::table.has_entry(board.hash)) {
    TableEntry entry = Engine::table.get_entry(board.hash);
    if (entry.depth >= depth) {
      if (entry.type == EntryType::Exact) {
        return BestMove{entry.eval, false};
      } else if (entry.type == EntryType::Lower) {
        alpha = std::max(alpha, int(entry.eval));
      } else if (entry.type == EntryType::Upper) {
        beta = std::min(beta, int(entry.eval));
      }
      if (alpha >= beta) {
        return BestMove{entry.eval, false};
      }
    }
  }

  if (depth == 0) {
    Eval eval = Engine::evaluate(board, move.colour);
    // Engine::table.set_entry(board.hash, TableEntry{eval, 0, EntryType::Exact});
    return BestMove{eval, false};
  }

  std::vector<Move> moves;
  bool check = board.get_moves(moves, opposite(move.colour));
  std::sort(moves.begin(), moves.end(),
            [](Move &a, Move &b) { return a.score > b.score; });

  if (moves.size() == 0) {
    Eval eval = check ? (-10000 - depth) : -depth;
    // Engine::table.set_entry(board.hash, TableEntry{eval, 0, EntryType::Exact});
    return BestMove{eval, true};
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
        search_moves_inner(depth - 1, moves[i], new_board, -beta, -alpha, mode);
    if (-result.eval > best.eval) {
      best.eval = -result.eval;
      best.move = moves[i];
    }
    if (best.eval > alpha) {
      alpha = best.eval;
    }
    if (alpha >= beta) {
      break;
    }
  }

  EntryType type = (best.eval <= alphaOrig) ? EntryType::Upper
                   : (best.eval >= beta)    ? EntryType::Lower
                                            : EntryType::Exact;
  Engine::table.set_entry(board.hash, TableEntry{best.eval, depth, type});
  return best;
}

BestMove Engine::search_moves(Mode mode) {
  BestMove move = search_moves_inner(mode.depth, this->move, this->board,
                                     EvalMin, EvalMax, mode);
  return move;
}

uint64_t Engine::get_timestamp() {
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
      .count();
}

void perft_inner(int8_t depth, Move &move, Board &board, bool debug) {
  std::vector<Move> moves;
  board.get_moves(moves, opposite(move.colour));
  if (depth == 1) {
    NodeCount += moves.size();
    return;
  }
  uint64_t count = 0;
  for (int i = 0; i < moves.size(); i++) {
    Board new_board = board.make_move(moves[i]);
    uint64_t before = NodeCount;
    perft_inner(depth - 1, moves[i], new_board, false);
    if (debug) {
      std::cout << moves[i].format() << ": " << (NodeCount - before) << "\n";
    }
  }
}

void Engine::perft(int8_t depth) {
  NodeCount = 0;
  uint64_t start = Engine::get_timestamp();
  perft_inner(depth, this->move, this->board, true);
  uint64_t end = Engine::get_timestamp();
  uint64_t nps = NodeCount / (end == start ? 1 : end - start) * 1000;
  std::cout << NodeCount << " nodes " << nps << " nps\n";
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
        if (board.piece_count > 4) {
          if (piece.colour == Colour::White) {
            weights = WhiteKingWeights[piece.square.x][piece.square.y];
          } else {
            weights = BlackKingWeights[piece.square.x][piece.square.y];
          }
        } else {
          weights = KingEndWeights[piece.square.x][piece.square.y];
        }
      } else {
        weights = PieceWeights[piece.square.x][piece.square.y];
      }
      eval += (PieceValue[piece.type] + weights) *
              ((piece.colour == colour) ? -1 : 1);
    }
  }
  return eval;
}
