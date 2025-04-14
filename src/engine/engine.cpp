#include "engine.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <math.h>
#include <float.h>

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

uint64_t Engine::NodeCount = 0;
Table Engine::table = Table::init();

void Timer::start() { this->start_time = std::chrono::steady_clock::now(); }

double Timer::stop() {
  auto end_time = std::chrono::steady_clock::now();
  return std::chrono::duration<double>(end_time - this->start_time).count();
}

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
                            int beta, uint8_t max_depth, double allocated,
                            Timer &timer) {
  Engine::NodeCount += 1;

  if (timer.stop() > allocated) {
    return BestMove{0, false, Move{}, true};
  }

  int alphaOrig = alpha;
  uint8_t bestMove = UINT8_MAX;
  if (Engine::table.has_entry(board.hash)) {
    TableEntry entry = Engine::table.get_entry(board.hash);
    if (entry.depth >= depth && depth != max_depth) {
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
    bestMove = entry.move;
  }

  if ((depth <= 0 && !move.takes) || depth <= -1) {
    Eval eval = Engine::evaluate(board, move.colour);
    // Engine::table.set_entry(board.hash, TableEntry{eval, 0,
    // EntryType::Exact});
    return BestMove{eval, false};
  }

  std::vector<Move> moves;
  bool check = board.get_moves(moves, opposite(move.colour));
  if (bestMove != UINT8_MAX) {
    moves[bestMove].score = INT8_MAX;
  }
  std::sort(moves.begin(), moves.end(),
            [](Move &a, Move &b) { return a.score > b.score; });

  if (moves.size() == 0) {
    Eval eval = check ? (-10000 - depth) : -depth;
    // Engine::table.set_entry(board.hash, TableEntry{eval, 0,
    // EntryType::Exact});
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
    BestMove result = search_moves_inner(depth - 1, moves[i], new_board, -beta,
                                         -alpha, max_depth, allocated, timer);
    if(result.cancelled) {
      return result;
    }
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
  Engine::table.set_entry(
      board.hash, TableEntry{best.eval, depth, type, best.move.move_id});
  return best;
}

BestMove Engine::search_moves_depth(uint8_t max_depth) {
  BestMove move;
  for (size_t depth = 1; depth <= max_depth; depth++) {
    move = search_moves_inner(depth, this->move, this->board, EvalMin, EvalMax,
                              depth, DBL_MAX, this->timer);
  }
  return move;
}

BestMove Engine::search_moves(uint8_t max_depth, double time, bool debug) {
  if (debug) {
    Engine::NodeCount = 0;
  }
  double allocated = time * 0.025;
  this->timer.start();
  BestMove move;
  for (size_t depth = 1; depth <= max_depth; depth++) {
    BestMove best_move = search_moves_inner(depth, this->move, this->board, EvalMin, EvalMax,
                              depth, allocated, this->timer);
    auto duration = this->timer.stop();
    if (best_move.cancelled) {
      if (debug) {
        uint64_t nps = Engine::NodeCount / duration;
        std::cout << "info time " << duration << "s " << nps << " nps\n";
      }
      break;
    }
    move = best_move;
    if (debug) {
      std::cout << "info depth " << depth << " best " << move.move.format()
                << " time " << duration << "s\n";
    }
  }
  return move;
}

void perft_inner(int8_t depth, Move &move, Board &board, bool debug) {
  std::vector<Move> moves;
  board.get_moves(moves, opposite(move.colour));
  if (depth == 1) {
    Engine::NodeCount += moves.size();
    return;
  }
  uint64_t count = 0;
  for (int i = 0; i < moves.size(); i++) {
    Board new_board = board.make_move(moves[i]);
    uint64_t before = Engine::NodeCount;
    perft_inner(depth - 1, moves[i], new_board, false);
    if (debug) {
      std::cout << moves[i].format() << ": " << (Engine::NodeCount - before)
                << "\n";
    }
  }
}

void Engine::perft(int8_t depth) {
  Engine::NodeCount = 0;
  this->timer.start();
  perft_inner(depth, this->move, this->board, true);
  uint64_t nps = Engine::NodeCount / this->timer.stop();
  std::cout << Engine::NodeCount << " nodes " << nps << " nps\n";
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
