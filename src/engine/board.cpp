#include "board.hpp"
#include "engine.hpp"
#include "piece.hpp"
#include <type_traits>

Board Board::init() {
  std::vector<Piece> pieces = {
      Piece(0, PieceType::Pawn, Colour::White, Square(0, 1)),
      Piece(1, PieceType::Pawn, Colour::White, Square(1, 1)),
      Piece(2, PieceType::Pawn, Colour::White, Square(2, 1)),
      Piece(3, PieceType::Pawn, Colour::White, Square(3, 1)),
      Piece(4, PieceType::Pawn, Colour::White, Square(4, 1)),
      Piece(5, PieceType::Pawn, Colour::White, Square(5, 1)),
      Piece(6, PieceType::Pawn, Colour::White, Square(6, 1)),
      Piece(7, PieceType::Pawn, Colour::White, Square(7, 1)),
      Piece(8, PieceType::Rook, Colour::White, Square(0, 0)),
      Piece(9, PieceType::Knight, Colour::White, Square(1, 0)),
      Piece(10, PieceType::Bishop, Colour::White, Square(2, 0)),
      Piece(11, PieceType::Queen, Colour::White, Square(3, 0)),
      Piece(12, PieceType::King, Colour::White, Square(4, 0)),
      Piece(13, PieceType::Bishop, Colour::White, Square(5, 0)),
      Piece(14, PieceType::Knight, Colour::White, Square(6, 0)),
      Piece(15, PieceType::Rook, Colour::White, Square(7, 0)),
      Piece(16, PieceType::Pawn, Colour::Black, Square(0, 6)),
      Piece(17, PieceType::Pawn, Colour::Black, Square(1, 6)),
      Piece(18, PieceType::Pawn, Colour::Black, Square(2, 6)),
      Piece(19, PieceType::Pawn, Colour::Black, Square(3, 6)),
      Piece(20, PieceType::Pawn, Colour::Black, Square(4, 6)),
      Piece(21, PieceType::Pawn, Colour::Black, Square(5, 6)),
      Piece(22, PieceType::Pawn, Colour::Black, Square(6, 6)),
      Piece(23, PieceType::Pawn, Colour::Black, Square(7, 6)),
      Piece(24, PieceType::Rook, Colour::Black, Square(0, 7)),
      Piece(25, PieceType::Knight, Colour::Black, Square(1, 7)),
      Piece(26, PieceType::Bishop, Colour::Black, Square(2, 7)),
      Piece(27, PieceType::Queen, Colour::Black, Square(3, 7)),
      Piece(28, PieceType::King, Colour::Black, Square(4, 7)),
      Piece(29, PieceType::Bishop, Colour::Black, Square(5, 7)),
      Piece(30, PieceType::Knight, Colour::Black, Square(6, 7)),
      Piece(31, PieceType::Rook, Colour::Black, Square(7, 7))};

  std::vector<std::vector<uint8_t>> board(8, std::vector<uint8_t>(8, EMPTY));
  for (auto &piece : pieces) {
    board[piece.square.x][piece.square.y] = piece.id;
  }

  return Board{pieces, board, false, false, false};
}

void Board::make_move(Move &move) {
  Piece &piece = this->pieces[this->board[move.from.x][move.from.y]];
  piece.square = move.to;
  piece.type = move.piece.type;
  if (this->board[move.to.x][move.to.y] != EMPTY) {
    this->pieces[this->board[move.to.x][move.to.y]].taken = true;
  }
  this->board[move.to.x][move.to.y] = this->board[move.from.x][move.from.y];
  this->board[move.from.x][move.from.y] = EMPTY;
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

void Board::get_move(std::vector<Position *> &moves, Square &from, Square &to,
                     MoveType type) {
  Piece &piece = this->pieces[this->board[from.x][from.y]];
  Move move = Move{piece, std::nullopt, from, to, type};
  Position *position = new Position{*this, move};
  position->board.make_move(move);
  if (this->board[to.x][to.y] != EMPTY) {
    position->move.takes = this->pieces[this->board[to.x][to.y]];
  }
  if (!position->board.is_check(piece.colour)) {
    moves.push_back(position);
  }
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
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
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

bool is_check_inner(Piece &piece, Board &board, int a, int b,
                    PieceType expect) {
  Square square(piece.square.x + a, piece.square.y + b);
  while (board.in_bounds(square)) {
    if (board.is_occupied(square)) {
      Piece &target = board.pieces[board.board[square.x][square.y]];
      if (target.colour != piece.colour &&
          (target.type == PieceType::Queen || target.type == expect)) {
        return true;
      }
      return false;
    }
    square.x += a, square.y += b;
  }
  return false;
};

bool is_check_inner_single(Piece &piece, Board &board, int a, int b,
                           PieceType expect) {
  Square square(piece.square.x + a, piece.square.y + b);
  if (board.in_bounds(square) && board.is_occupied(square)) {
    Piece &target = board.pieces[board.board[square.x][square.y]];
    if ((target.colour != piece.colour) && (target.type == expect)) {
      return true;
    }
  }
  return false;
};

bool Board::is_check(Colour colour) {
  Piece &piece = this->pieces[colour == Colour::White ? 12 : 28];
  int step = colour == Colour::White ? 1 : -1;
  return is_check_inner(piece, *this, 1, 1, PieceType::Bishop) ||
         is_check_inner(piece, *this, -1, 1, PieceType::Bishop) ||
         is_check_inner(piece, *this, 1, -1, PieceType::Bishop) ||
         is_check_inner(piece, *this, -1, -1, PieceType::Bishop) ||
         is_check_inner(piece, *this, 1, 0, PieceType::Rook) ||
         is_check_inner(piece, *this, -1, 0, PieceType::Rook) ||
         is_check_inner(piece, *this, 0, 1, PieceType::Rook) ||
         is_check_inner(piece, *this, 0, -1, PieceType::Rook) ||
         is_check_inner_single(piece, *this, 1, 2, PieceType::Knight) ||
         is_check_inner_single(piece, *this, 1, -2, PieceType::Knight) ||
         is_check_inner_single(piece, *this, -1, 2, PieceType::Knight) ||
         is_check_inner_single(piece, *this, -1, -2, PieceType::Knight) ||
         is_check_inner_single(piece, *this, 2, 1, PieceType::Knight) ||
         is_check_inner_single(piece, *this, 2, -1, PieceType::Knight) ||
         is_check_inner_single(piece, *this, -2, 1, PieceType::Knight) ||
         is_check_inner_single(piece, *this, -2, -1, PieceType::Knight) ||
         is_check_inner_single(piece, *this, 1, step, PieceType::Pawn) ||
         is_check_inner_single(piece, *this, -1, step, PieceType::Pawn);
}