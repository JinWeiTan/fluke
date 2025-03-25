#include "piece.hpp"
#include "board.hpp"
#include "engine.hpp"
#include <optional>
#include <string>

void get_move_inner(Piece &piece, Board &board, std::vector<Move> &moves,
                    uint8_t a, uint8_t b, Attacks &attacks) {
  Square square{piece.square.x + a, piece.square.y + b};
  while (board.in_bounds(square) && !board.is_occupied(square, piece.colour)) {
    board.get_move(moves, piece.square, square, MoveType::Step, attacks);
    if (board.is_occupied(square, opposite(piece.colour))) {
      break;
    }
    square.x += a, square.y += b;
  }
};

void get_move_inner_single(Piece &piece, Board &board,
                           std::vector<Move> &moves, uint8_t a, uint8_t b,
                           Attacks &attacks) {
  Square square{piece.square.x + a, piece.square.y + b};
  if (board.in_bounds(square) && !board.is_occupied(square, piece.colour)) {
    board.get_move(moves, piece.square, square, MoveType::Step, attacks);
  }
};

void get_promotion_moves(Piece &piece, std::vector<Move> &moves,
                         Attacks &attacks) {
  if (piece.square.y == (piece.colour == Colour::White ? 6 : 1)) {
    Move last_move = moves[moves.size() - 1];
    last_move.type = MoveType::PromoteQueen;
    last_move.piece = PieceType::Queen;
    for (size_t i = 0; i <= 3; i++) {
      Move move = last_move;
      move.type = MoveType(7 - i);
      move.piece = PieceType(4 - i);
      moves.push_back(move);
    }
  }
}

void get_pawn_moves(Piece &piece, Board &board, std::vector<Move> &moves,
                    Attacks &attacks) {
  PinType pin = attacks.pins[piece.id];
  uint8_t step = piece.colour == Colour::White ? 1 : -1;
  // Diagonal pawn capture
  if (pin == PinType::None || pin == PinType::LDiagonal) {
    Square square{piece.square.x + step, piece.square.y + step};
    if (board.in_bounds(square) &&
        board.is_occupied(square, opposite(piece.colour))) {
      bool is_legal =
          board.get_move(moves, piece.square, square, MoveType::Step, attacks);
      if (is_legal) {
        get_promotion_moves(piece, moves, attacks);
      }
    }
  }
  if (pin == PinType::None || pin == PinType::RDiagonal) {
    Square square2{piece.square.x - step, piece.square.y + step};
    if (board.in_bounds(square2) &&
        board.is_occupied(square2, opposite(piece.colour))) {
      bool is_legal =
          board.get_move(moves, piece.square, square2, MoveType::Step, attacks);
      if (is_legal) {
        get_promotion_moves(piece, moves, attacks);
      }
    }
  }

  if (pin == PinType::None || pin == PinType::Vertical) {
    // Double pawn move
    if (piece.square.y == (piece.colour == Colour::White ? 1 : 6)) {
      Square square1{piece.square.x, piece.colour == Colour::White ? 3 : 4};
      Square square2{piece.square.x, piece.colour == Colour::White ? 2 : 5};
      if (board.in_bounds(square1) && !board.is_occupied(square1) &&
          !board.is_occupied(square2)) {
        board.get_move(moves, piece.square, square1, MoveType::DoubleStep,
                       attacks);
      }
    }

    // Single pawn move
    Square square3{piece.square.x, piece.square.y + step};
    if (board.in_bounds(square3) && !board.is_occupied(square3)) {
      bool is_legal =
          board.get_move(moves, piece.square, square3, MoveType::Step, attacks);
      if (is_legal) {
        get_promotion_moves(piece, moves, attacks);
      }
    }
  }

  // En passant
  if (piece.square.y == (piece.colour == Colour::White ? 4 : 3)) {
    if (pin == PinType::None || pin == PinType::LDiagonal) {
      if (board.double_step == piece.square.x + step) {
        Square square{piece.square.x + step, piece.square.y + step};
        board.get_move(moves, piece.square, square, MoveType::EnPassant,
                       attacks);
      }
    }
    if (pin == PinType::None || pin == PinType::RDiagonal) {
      if (board.double_step == piece.square.x - step) {
        Square square{piece.square.x - step, piece.square.y + step};
        board.get_move(moves, piece.square, square, MoveType::EnPassant,
                       attacks);
      }
    }
  }
}

void get_knight_moves(Piece &piece, Board &board,
                      std::vector<Move> &moves, Attacks &attacks) {
  PinType pin = attacks.pins[piece.id];
  if (pin == PinType::None) {
    get_move_inner_single(piece, board, moves, 1, 2, attacks);
    get_move_inner_single(piece, board, moves, 1, -2, attacks);
    get_move_inner_single(piece, board, moves, -1, 2, attacks);
    get_move_inner_single(piece, board, moves, -1, -2, attacks);
    get_move_inner_single(piece, board, moves, 2, 1, attacks);
    get_move_inner_single(piece, board, moves, 2, -1, attacks);
    get_move_inner_single(piece, board, moves, -2, 1, attacks);
    get_move_inner_single(piece, board, moves, -2, -1, attacks);
  }
}

void get_bishop_moves(Piece &piece, Board &board,
                      std::vector<Move> &moves, Attacks &attacks) {
  PinType pin = attacks.pins[piece.id];
  if (pin == PinType::None || pin == PinType::LDiagonal) {
    get_move_inner(piece, board, moves, 1, 1, attacks);
    get_move_inner(piece, board, moves, -1, -1, attacks);
  }
  if (pin == PinType::None || pin == PinType::RDiagonal) {
    get_move_inner(piece, board, moves, -1, 1, attacks);
    get_move_inner(piece, board, moves, 1, -1, attacks);
  }
}

void get_rook_moves(Piece &piece, Board &board, std::vector<Move> &moves,
                    Attacks &attacks) {
  PinType pin = attacks.pins[piece.id];
  if (pin == PinType::None || pin == PinType::Horizontal) {
    get_move_inner(piece, board, moves, 1, 0, attacks);
    get_move_inner(piece, board, moves, -1, 0, attacks);
  }
  if (pin == PinType::None || pin == PinType::Vertical) {
    get_move_inner(piece, board, moves, 0, 1, attacks);
    get_move_inner(piece, board, moves, 0, -1, attacks);
  }
}

void get_queen_moves(Piece &piece, Board &board, std::vector<Move> &moves,
                     Attacks &attacks) {
  PinType pin = attacks.pins[piece.id];
  if (pin == PinType::None || pin == PinType::LDiagonal) {
    get_move_inner(piece, board, moves, 1, 1, attacks);
    get_move_inner(piece, board, moves, -1, -1, attacks);
  }
  if (pin == PinType::None || pin == PinType::RDiagonal) {
    get_move_inner(piece, board, moves, -1, 1, attacks);
    get_move_inner(piece, board, moves, 1, -1, attacks);
  }
  if (pin == PinType::None || pin == PinType::Horizontal) {
    get_move_inner(piece, board, moves, 1, 0, attacks);
    get_move_inner(piece, board, moves, -1, 0, attacks);
  }
  if (pin == PinType::None || pin == PinType::Vertical) {
    get_move_inner(piece, board, moves, 0, 1, attacks);
    get_move_inner(piece, board, moves, 0, -1, attacks);
  }
}

void get_king_moves(Piece &piece, Board &board, std::vector<Move> &moves,
                    Attacks &attacks) {
  get_move_inner_single(piece, board, moves, 1, 1, attacks);
  get_move_inner_single(piece, board, moves, -1, 1, attacks);
  get_move_inner_single(piece, board, moves, 1, -1, attacks);
  get_move_inner_single(piece, board, moves, -1, -1, attacks);
  get_move_inner_single(piece, board, moves, 1, 0, attacks);
  get_move_inner_single(piece, board, moves, -1, 0, attacks);
  get_move_inner_single(piece, board, moves, 0, 1, attacks);
  get_move_inner_single(piece, board, moves, 0, -1, attacks);

  // Castling
  if (!attacks.check) {
    if (piece.colour == Colour::White) {
      if (board.castling.white_queenside) {
        Square square1 = {4, 0}, square2 = {5, 0}, square3 = {6, 0};
        if (!board.is_occupied(square1) && !board.is_occupied(square2) &&
            !board.is_occupied(square3) && attacks.is_safe(square1) &&
            attacks.is_safe(square2)) {
          board.get_move(moves, piece.square, square2, MoveType::Castle,
                         attacks);
        }
      }
      if (board.castling.white_kingside) {
        Square square1 = {2, 0}, square2 = {1, 0};
        if (!board.is_occupied(square1) && !board.is_occupied(square2) &&
            attacks.is_safe(square1) && attacks.is_safe(square2)) {
          board.get_move(moves, piece.square, square2, MoveType::Castle,
                         attacks);
        }
      }
    } else {
      if (board.castling.black_queenside) {
        Square square1 = {4, 7}, square2 = {5, 7}, square3 = {6, 7};
        if (!board.is_occupied(square1) && !board.is_occupied(square2) &&
            !board.is_occupied(square3) && attacks.is_safe(square1) &&
            attacks.is_safe(square2)) {
          board.get_move(moves, piece.square, square2, MoveType::Castle,
                         attacks);
        }
      }
      if (board.castling.black_kingside) {
        Square square1 = {2, 7}, square2 = {1, 7};
        if (!board.is_occupied(square1) && !board.is_occupied(square2) &&
            attacks.is_safe(square1) && attacks.is_safe(square2)) {
          board.get_move(moves, piece.square, square2, MoveType::Castle,
                         attacks);
        }
      }
    }
  }
}

void Piece::get_moves(Board &board, std::vector<Move> &moves,
                      Attacks &attacks) {
  switch (this->type) {
  case PieceType::Pawn:
    return get_pawn_moves(*this, board, moves, attacks);
  case PieceType::Knight:
    return get_knight_moves(*this, board, moves, attacks);
  case PieceType::Bishop:
    return get_bishop_moves(*this, board, moves, attacks);
  case PieceType::Rook:
    return get_rook_moves(*this, board, moves, attacks);
  case PieceType::Queen:
    return get_queen_moves(*this, board, moves, attacks);
  case PieceType::King:
    return get_king_moves(*this, board, moves, attacks);
  }
}