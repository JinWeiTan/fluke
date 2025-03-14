#include "piece.hpp"
#include "board.hpp"
#include "engine.hpp"
#include <optional>
#include <string>

void get_move_inner(Piece &piece, Board &board, std::vector<Position *> &moves,
                    int a, int b) {
  Square square(piece.square.x + a, piece.square.y + b);
  while (board.in_bounds(square) && !board.is_occupied(square, piece.colour)) {
    board.get_move(moves, piece.square, square, MoveType::Step);
    if (board.is_occupied(square, opposite(piece.colour))) {
      break;
    }
    square.x += a, square.y += b;
  }
};

void get_move_inner_single(Piece &piece, Board &board,
                           std::vector<Position *> &moves, int a, int b) {
  Square square(piece.square.x + a, piece.square.y + b);
  if (board.in_bounds(square) && !board.is_occupied(square, piece.colour)) {
    board.get_move(moves, piece.square, square, MoveType::Step);
  }
};

void get_pawn_moves(Piece &piece, Board &board,
                    std::vector<Position *> &moves) {
  int step = piece.colour == Colour::White ? 1 : -1;
  // Diagonal pawn capture
  Square square(piece.square.x + 1, piece.square.y + step);
  if (board.in_bounds(square) &&
      board.is_occupied(square, opposite(piece.colour))) {
    board.get_move(moves, piece.square, square, MoveType::Step);
  }
  square = Square(piece.square.x - 1, piece.square.y + step);
  if (board.in_bounds(square) &&
      board.is_occupied(square, opposite(piece.colour))) {
    board.get_move(moves, piece.square, square, MoveType::Step);
  }

  // Double pawn move
  if (piece.square.y == (piece.colour == Colour::White ? 1 : 6)) {
    Square square1(piece.square.x, piece.colour == Colour::White ? 3 : 4);
    Square square2(piece.square.x, piece.colour == Colour::White ? 2 : 5);
    if (board.in_bounds(square1) && !board.is_occupied(square1) &&
        !board.is_occupied(square2)) {
      board.get_move(moves, piece.square, square1, MoveType::DoubleStep);
    }
  }

  // Single pawn move
  square = Square(piece.square.x, piece.square.y + step);
  if (board.in_bounds(square) && !board.is_occupied(square)) {
    board.get_move(moves, piece.square, square, MoveType::Step);
  }

  // En passant
  square = Square(piece.square.x + 1, piece.square.y);
  if (board.last_move_double_step) {
    square = Square(piece.square.x + 1, piece.square.y + 1);
    board.get_move(moves, piece.square, square, MoveType::EnPassant);
  }
  square = Square(piece.square.x - 1, piece.square.y);
  if (board.last_move_double_step) {
    square = Square(piece.square.x - 1, piece.square.y + 1);
    board.get_move(moves, piece.square, square, MoveType::EnPassant);
  }

  // Promotion
  if (piece.square.y == (piece.colour == Colour::White ? 6 : 1)) {
    std::vector<Position *> promotion;
    int len = moves.size();
    for (int i = 0; i < len; i += 1) {
      for (int j = 3; j >= 0; j -= 1) {
        Position *move = new Position{moves[i]->move};
        move->move.type = MoveType::Promotion;
        move->move.piece.type = static_cast<PieceType>(j + 1);
        //fix
        promotion.push_back(move);
      }
    }
    moves = promotion;
  }
}

void get_knight_moves(Piece &piece, Board &board,
                      std::vector<Position *> &moves) {
  get_move_inner_single(piece, board, moves, 1, 2);
  get_move_inner_single(piece, board, moves, 1, -2);
  get_move_inner_single(piece, board, moves, -1, 2);
  get_move_inner_single(piece, board, moves, -1, -2);
  get_move_inner_single(piece, board, moves, 2, 1);
  get_move_inner_single(piece, board, moves, 2, -1);
  get_move_inner_single(piece, board, moves, -2, 1);
  get_move_inner_single(piece, board, moves, -2, -1);
}

void get_bishop_moves(Piece &piece, Board &board,
                      std::vector<Position *> &moves) {
  get_move_inner(piece, board, moves, 1, 1);
  get_move_inner(piece, board, moves, -1, 1);
  get_move_inner(piece, board, moves, 1, -1);
  get_move_inner(piece, board, moves, -1, -1);
}

void get_rook_moves(Piece &piece, Board &board,
                    std::vector<Position *> &moves) {
  get_move_inner(piece, board, moves, 1, 0);
  get_move_inner(piece, board, moves, -1, 0);
  get_move_inner(piece, board, moves, 0, 1);
  get_move_inner(piece, board, moves, 0, -1);
}

void get_queen_moves(Piece &piece, Board &board,
                     std::vector<Position *> &moves) {
  get_move_inner(piece, board, moves, 1, 1);
  get_move_inner(piece, board, moves, -1, 1);
  get_move_inner(piece, board, moves, 1, -1);
  get_move_inner(piece, board, moves, -1, -1);
  get_move_inner(piece, board, moves, 1, 0);
  get_move_inner(piece, board, moves, -1, 0);
  get_move_inner(piece, board, moves, 0, 1);
  get_move_inner(piece, board, moves, 0, -1);
}

void get_king_moves(Piece &piece, Board &board,
                    std::vector<Position *> &moves) {
  get_move_inner_single(piece, board, moves, 1, 1);
  get_move_inner_single(piece, board, moves, -1, 1);
  get_move_inner_single(piece, board, moves, 1, -1);
  get_move_inner_single(piece, board, moves, -1, -1);
  get_move_inner_single(piece, board, moves, 1, 0);
  get_move_inner_single(piece, board, moves, -1, 0);
  get_move_inner_single(piece, board, moves, 0, 1);
  get_move_inner_single(piece, board, moves, 0, -1);

  // Castling
  if (piece.colour == Colour::White) {
    if (board.castling.white) {
      Square square1 = Square{5, 0}, square2 = Square{6, 0};
      if (!board.is_occupied(square1) && !board.is_occupied(square2)) {
        if (!board.is_check_at(square1, Colour::White) &&
            !board.is_check_at(square2, Colour::White)) {
          board.get_move(moves, piece.square, square2, MoveType::Castle);
        }
      }
      square1 = Square{3, 0}, square2 = Square{2, 0};
      Square square3 = Square{1, 0};
      if (!board.is_occupied(square1) && !board.is_occupied(square2) &&
          !board.is_occupied(square3)) {
        if (!board.is_check_at(square1, Colour::White) &&
            !board.is_check_at(square2, Colour::White)) {
          board.get_move(moves, piece.square, square2, MoveType::Castle);
        }
      }
    }
  } else {
    if (board.castling.black) {
      Square square1 = Square{5, 7}, square2 = Square{6, 7};
      if (!board.is_occupied(square1) && !board.is_occupied(square2)) {
        if (!board.is_check_at(square1, Colour::Black) &&
            !board.is_check_at(square2, Colour::Black)) {
          board.get_move(moves, piece.square, square2, MoveType::Castle);
        }
      }
      square1 = Square{3, 7}, square2 = Square{2, 7};
      Square square3 = Square{1, 7};
      if (!board.is_occupied(square1) && !board.is_occupied(square2) &&
          !board.is_occupied(square3)) {
        if (!board.is_check_at(square1, Colour::Black) &&
            !board.is_check_at(square2, Colour::Black)) {
          board.get_move(moves, piece.square, square2, MoveType::Castle);
        }
      }
    }
  }
}

void Piece::get_moves(Board &board, std::vector<Position *> &moves) {
  switch (this->type) {
  case PieceType::Pawn:
    return get_pawn_moves(*this, board, moves);
  case PieceType::Knight:
    return get_knight_moves(*this, board, moves);
  case PieceType::Bishop:
    return get_bishop_moves(*this, board, moves);
  case PieceType::Rook:
    return get_rook_moves(*this, board, moves);
  case PieceType::Queen:
    return get_queen_moves(*this, board, moves);
  case PieceType::King:
    return get_king_moves(*this, board, moves);
  }
}

std::string Move::get_name() {
  return PieceName[this->piece.type] + this->to.get_name();
}

std::string Square::get_name() { return FileName[this->x] + RankName[this->y]; }