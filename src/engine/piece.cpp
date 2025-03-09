#include "piece.hpp"
#include "board.hpp"
#include "engine.hpp"
#include <optional>

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
  // Single pawn move
  Square square(piece.square.x, piece.square.y + step);
  if (board.in_bounds(square) && !board.is_occupied(square)) {
    board.get_move(moves, piece.square, square, MoveType::Step);
  }

  // Diagonal pawn capture
  square = Square(piece.square.x + 1, piece.square.y + step);
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
    for (int i = 0; i < moves.size(); i += 1) {
      for (int j = 0; j < 4; j += 1) {
        Position *move = new Position{moves[i]->board, moves[i]->move};
        move->move.type = MoveType::Promotion;
        move->move.piece.type = static_cast<PieceType>(j + 1);
        move->board.pieces[move->move.piece.id].type = move->move.piece.type; 
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
