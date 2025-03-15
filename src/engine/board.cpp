#include "board.hpp"
#include "engine.hpp"
#include "piece.hpp"
#include <iostream>
#include <type_traits>

Board Board::init() {
  Board board =
      Board{{Piece(0, PieceType::Pawn, Colour::White, Square(4, 1)),
             Piece(1, PieceType::Pawn, Colour::White, Square(3, 1)),
             Piece(2, PieceType::Pawn, Colour::White, Square(5, 1)),
             Piece(3, PieceType::Pawn, Colour::White, Square(2, 1)),
             Piece(4, PieceType::Pawn, Colour::White, Square(6, 1)),
             Piece(5, PieceType::Pawn, Colour::White, Square(1, 1)),
             Piece(6, PieceType::Pawn, Colour::White, Square(7, 1)),
             Piece(7, PieceType::Pawn, Colour::White, Square(0, 1)),
             Piece(8, PieceType::Rook, Colour::White, Square(0, 0)),
             Piece(9, PieceType::Knight, Colour::White, Square(1, 0)),
             Piece(10, PieceType::Bishop, Colour::White, Square(2, 0)),
             Piece(11, PieceType::Queen, Colour::White, Square(3, 0)),
             Piece(12, PieceType::King, Colour::White, Square(4, 0)),
             Piece(13, PieceType::Bishop, Colour::White, Square(5, 0)),
             Piece(14, PieceType::Knight, Colour::White, Square(6, 0)),
             Piece(15, PieceType::Rook, Colour::White, Square(7, 0)),
             Piece(16, PieceType::Pawn, Colour::Black, Square(4, 6)),
             Piece(17, PieceType::Pawn, Colour::Black, Square(3, 6)),
             Piece(18, PieceType::Pawn, Colour::Black, Square(5, 6)),
             Piece(19, PieceType::Pawn, Colour::Black, Square(2, 6)),
             Piece(20, PieceType::Pawn, Colour::Black, Square(6, 6)),
             Piece(21, PieceType::Pawn, Colour::Black, Square(1, 6)),
             Piece(22, PieceType::Pawn, Colour::Black, Square(7, 6)),
             Piece(23, PieceType::Pawn, Colour::Black, Square(0, 6)),
             Piece(24, PieceType::Rook, Colour::Black, Square(0, 7)),
             Piece(25, PieceType::Knight, Colour::Black, Square(1, 7)),
             Piece(26, PieceType::Bishop, Colour::Black, Square(2, 7)),
             Piece(27, PieceType::Queen, Colour::Black, Square(3, 7)),
             Piece(28, PieceType::King, Colour::Black, Square(4, 7)),
             Piece(29, PieceType::Bishop, Colour::Black, Square(5, 7)),
             Piece(30, PieceType::Knight, Colour::Black, Square(6, 7)),
             Piece(31, PieceType::Rook, Colour::Black, Square(7, 7))}};
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      board.board[i][j] = EMPTY;
    }
  }
  for (auto &piece : board.pieces) {
    board.board[piece.square.x][piece.square.y] = piece.id;
  }
  board.castling = Castling{true, true};
  board.double_step = 9;
  return board;
}

void move_piece(Board &board, Square from, Square to) {
  Piece &piece = board.pieces[board.board[from.x][from.y]];
  piece.square = to;
  board.board[to.x][to.y] = board.board[from.x][from.y];
  board.board[from.x][from.y] = EMPTY;
}

Board Board::make_move(Move &move) {
  Board board = *this;
  Piece &piece = board.pieces[board.board[move.from.x][move.from.y]];
  piece.type = move.piece;
  if (board.board[move.to.x][move.to.y] != EMPTY) {
    board.pieces[board.board[move.to.x][move.to.y]].taken = true;
  }
  move_piece(board, move.from, move.to);

  board.double_step = 9;
  if (move.type == MoveType::Castle) {
    if (piece.colour == Colour::White) {
      if (move.to.x == 2) {
        move_piece(board, Square{0, 0}, Square{3, 0});
      } else {
        move_piece(board, Square{7, 0}, Square{5, 0});
      }
    } else {
      if (move.to.x == 2) {
        move_piece(board, Square{0, 7}, Square{3, 7});
      } else {
        move_piece(board, Square{7, 7}, Square{5, 7});
      }
    }
  } else if (move.type == MoveType::EnPassant) {
    board.pieces[board.board[move.to.x][move.from.y]].taken = true;
  } else if (move.type == MoveType::DoubleStep) {
    board.double_step = move.to.x;
  } 

  if (move.piece == PieceType::Rook) {
    if (move.colour == Colour::White) {
      board.castling.white = false;
    } else {
      board.castling.black = false;
    }
  } else if (move.piece == PieceType::King) {
    if (move.colour == Colour::White) {
      board.castling.white = false;
    } else {
      board.castling.black = false;
    }
  }
  return board;
}

void Board::get_moves(std::vector<Position *> &moves, Colour colour) {
  moves = {};
  for (size_t i = 0; i < 16; i++) {
    int index = i + (colour == Colour::White ? 0 : 16);
    if (!this->pieces[index].taken) {
      this->pieces[index].get_moves(*this, moves);
    }
  }
}

bool Board::get_move(std::vector<Position *> &moves, Square &from, Square &to,
                     MoveType type) {
  Piece &piece = this->pieces[this->board[from.x][from.y]];
  Move move = Move{piece.id, piece.type, piece.colour, from, to, type};
  Board board = this->make_move(move);
  bool is_check = board.is_check(piece.colour);
  if (!is_check) {
    moves.push_back(new Position{move});
  }
  return is_check;
}

bool Board::is_occupied(Square &square) {
  return this->board[square.x][square.y] != EMPTY;
}

bool Board::is_occupied(Square &square, Colour colour) {
  return this->is_occupied(square) &&
         this->pieces[this->board[square.x][square.y]].colour == colour;
}

bool Board::in_bounds(Square &square) {
  return square.x >= 0 && square.x <= 7 && square.y >= 0 && square.y <= 7;
}

void Board::display() {
  for (int i = 7; i >= 0; i--) {
    for (int j = 0; j <= 7; j++) {
      if (this->board[j][i] == EMPTY) {
        printf("x ");
      } else {
        Piece &piece = this->pieces[this->board[j][i]];
        printf("%i ", piece.type);
      }
    }
    printf("\n");
  }
  printf("\n");
}

bool is_check_inner(Square &from, Colour colour, Board &board, int a, int b,
                    PieceType expect) {
  Square square(from.x + a, from.y + b);
  while (board.in_bounds(square)) {
    if (board.is_occupied(square)) {
      Piece &target = board.pieces[board.board[square.x][square.y]];
      if (target.colour != colour &&
          (target.type == PieceType::Queen || target.type == expect)) {
        return true;
      }
      return false;
    }
    square.x += a, square.y += b;
  }
  return false;
};

bool is_check_single(Square &from, Colour colour, Board &board, int a, int b,
                     PieceType expect) {
  Square square(from.x + a, from.y + b);
  if (board.in_bounds(square) && board.is_occupied(square)) {
    Piece &target = board.pieces[board.board[square.x][square.y]];
    if ((target.colour != colour) && (target.type == expect)) {
      return true;
    }
  }
  return false;
};

bool Board::is_check(Colour colour) {
  Piece &piece = this->pieces[colour == Colour::White ? 12 : 28];
  return this->is_check_at(piece.square, colour);
}

bool Board::is_check_at(Square &square, Colour colour) {
  int step = colour == Colour::White ? 1 : -1;
  return is_check_inner(square, colour, *this, 1, 1, PieceType::Bishop) ||
         is_check_inner(square, colour, *this, -1, 1, PieceType::Bishop) ||
         is_check_inner(square, colour, *this, 1, -1, PieceType::Bishop) ||
         is_check_inner(square, colour, *this, -1, -1, PieceType::Bishop) ||
         is_check_inner(square, colour, *this, 1, 0, PieceType::Rook) ||
         is_check_inner(square, colour, *this, -1, 0, PieceType::Rook) ||
         is_check_inner(square, colour, *this, 0, 1, PieceType::Rook) ||
         is_check_inner(square, colour, *this, 0, -1, PieceType::Rook) ||
         is_check_single(square, colour, *this, 1, 2, PieceType::Knight) ||
         is_check_single(square, colour, *this, 1, -2, PieceType::Knight) ||
         is_check_single(square, colour, *this, -1, 2, PieceType::Knight) ||
         is_check_single(square, colour, *this, -1, -2, PieceType::Knight) ||
         is_check_single(square, colour, *this, 2, 1, PieceType::Knight) ||
         is_check_single(square, colour, *this, 2, -1, PieceType::Knight) ||
         is_check_single(square, colour, *this, -2, 1, PieceType::Knight) ||
         is_check_single(square, colour, *this, -2, -1, PieceType::Knight) ||
         is_check_single(square, colour, *this, 1, step, PieceType::Pawn) ||
         is_check_single(square, colour, *this, -1, step, PieceType::Pawn);
}