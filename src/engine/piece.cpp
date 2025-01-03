#include "piece.hpp"
#include "board.hpp"
#include <optional>

void get_move_inner(Piece &piece, Board &board, std::vector<Move> &moves, int a,
                    int b) {
  Square square(piece.square.x + a, piece.square.y + b);
  while (board.in_bounds(square) && !board.is_occupied(square, piece.colour)) {
    Move move = board.get_move(piece.square, square, MoveType::Step);
    moves.push_back(move);
    square.x += a, square.y += b;
    if (move.takes != std::nullopt) {
      break;
    }
  }
};

void get_move_inner_single(Piece &piece, Board &board, std::vector<Move> &moves,
                           int a, int b) {
  Square square(piece.square.x + a, piece.square.y + b);
  if (board.in_bounds(square) && !board.is_occupied(square, piece.colour)) {
    moves.push_back(board.get_move(piece.square, square, MoveType::Step));
  }
};

std::vector<Move> get_pawn_moves(Piece &piece, Board &board) {
  std::vector<Move> moves;
  int step = piece.colour == Colour::White ? 1 : -1;
  // Single pawn move
  Square square(piece.square.x, piece.square.y + step);
  if (board.in_bounds(square) && !board.is_occupied(square)) {
    moves.push_back(board.get_move(piece.square, square, MoveType::Step));
  }

  // Diagonal pawn capture
  square = Square(piece.square.x + 1, piece.square.y + step);
  if (board.in_bounds(square) &&
      board.is_occupied(square, opposite(piece.colour))) {
    moves.push_back(board.get_move(piece.square, square, MoveType::Step));
  }
  square = Square(piece.square.x - 1, piece.square.y + step);
  if (board.in_bounds(square) &&
      board.is_occupied(square, opposite(piece.colour))) {
    moves.push_back(board.get_move(piece.square, square, MoveType::Step));
  }

  // Double pawn move
  if (piece.square.y == (piece.colour == Colour::White ? 1 : 6)) {
    Square square1(piece.square.x, piece.colour == Colour::White ? 3 : 4);
    Square square2(piece.square.x, piece.colour == Colour::White ? 2 : 5);
    if (board.in_bounds(square1) && !board.is_occupied(square1) &&
        !board.is_occupied(square2)) {
      Move move = board.get_move(piece.square, square1, MoveType::DoubleStep);
      moves.push_back(move);
    }
  }

  // En passant
  if (board.moves.size() > 0) {
    square = Square(piece.square.x + 1, piece.square.y);
    if (board.moves.back().to == square &&
        board.moves.back().type == MoveType::DoubleStep) {
      square = Square(piece.square.x + 1, piece.square.y + 1);
      moves.push_back(
          board.get_move(piece.square, square, MoveType::EnPassant));
    }
    square = Square(piece.square.x - 1, piece.square.y);
    if (board.moves.back().to == square &&
        board.moves.back().type == MoveType::DoubleStep) {
      square = Square(piece.square.x - 1, piece.square.y + 1);
      moves.push_back(
          board.get_move(piece.square, square, MoveType::EnPassant));
    }
  }

  // Promotion
  if (piece.square.y == (piece.colour == Colour::White ? 6 : 1)) {
    std::vector<Move> promotion;
    promotion.reserve(moves.size() * 4);
    for (int i = 0; i < moves.size(); i += 1) {
      for (int j = 0; j < 4; j += 1) {
        Move move = moves[i];
        move.type = MoveType::Promotion;
        move.piece.type = static_cast<PieceType>(j + 1);
        promotion.push_back(move);
      }
    }
    return promotion;
  }

  return moves;
}

std::vector<Move> get_knight_moves(Piece &piece, Board &board) {
  std::vector<Move> moves;
  get_move_inner_single(piece, board, moves, 1, 2);
  get_move_inner_single(piece, board, moves, 1, -2);
  get_move_inner_single(piece, board, moves, -1, 2);
  get_move_inner_single(piece, board, moves, -1, -2);
  get_move_inner_single(piece, board, moves, 2, 1);
  get_move_inner_single(piece, board, moves, 2, -1);
  get_move_inner_single(piece, board, moves, -2, 1);
  get_move_inner_single(piece, board, moves, -2, -1);
  return moves;
}

std::vector<Move> get_bishop_moves(Piece &piece, Board &board) {
  std::vector<Move> moves;
  get_move_inner(piece, board, moves, 1, 1);
  get_move_inner(piece, board, moves, -1, 1);
  get_move_inner(piece, board, moves, 1, -1);
  get_move_inner(piece, board, moves, -1, -1);
  return moves;
}

std::vector<Move> get_rook_moves(Piece &piece, Board &board) {
  std::vector<Move> moves;
  get_move_inner(piece, board, moves, 1, 0);
  get_move_inner(piece, board, moves, -1, 0);
  get_move_inner(piece, board, moves, 0, 1);
  get_move_inner(piece, board, moves, 0, -1);
  return moves;
}

std::vector<Move> get_queen_moves(Piece &piece, Board &board) {
  std::vector<Move> moves;
  get_move_inner(piece, board, moves, 1, 1);
  get_move_inner(piece, board, moves, -1, 1);
  get_move_inner(piece, board, moves, 1, -1);
  get_move_inner(piece, board, moves, -1, -1);
  get_move_inner(piece, board, moves, 1, 0);
  get_move_inner(piece, board, moves, -1, 0);
  get_move_inner(piece, board, moves, 0, 1);
  get_move_inner(piece, board, moves, 0, -1);
  return moves;
}

std::vector<Move> get_king_moves(Piece &piece, Board &board) {
  std::vector<Move> moves;
  get_move_inner_single(piece, board, moves, 1, 1);
  get_move_inner_single(piece, board, moves, -1, 1);
  get_move_inner_single(piece, board, moves, 1, -1);
  get_move_inner_single(piece, board, moves, -1, -1);
  get_move_inner_single(piece, board, moves, 1, 0);
  get_move_inner_single(piece, board, moves, -1, 0);
  get_move_inner_single(piece, board, moves, 0, 1);
  get_move_inner_single(piece, board, moves, 0, -1);
  return moves;
}

std::vector<Move> Piece::get_moves(Board &board) {
  switch (this->type) {
  case PieceType::Pawn:
    return get_pawn_moves(*this, board);
  case PieceType::Knight:
    return get_knight_moves(*this, board);
  case PieceType::Bishop:
    return get_bishop_moves(*this, board);
  case PieceType::Rook:
    return get_rook_moves(*this, board);
  case PieceType::Queen:
    return get_queen_moves(*this, board);
  case PieceType::King:
    return get_king_moves(*this, board);
  }
}