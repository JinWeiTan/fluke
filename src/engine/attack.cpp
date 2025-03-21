#include "attack.hpp"
#include "board.hpp"

bool Attacks::is_safe(Square &square) {
  return !this->attacks[square.x][square.y];
}

void get_attack_inner(Piece &piece, Board &board, Attacks &attacks, uint8_t a,
                      uint8_t b, PinType pin) {
  Square square{piece.square.x + a, piece.square.y + b};
  while (board.in_bounds(square)) {
    attacks.attacks[square.x][square.y] = true;
    if (board.is_occupied(square)) {
      Piece &current = board.pieces[board.board[square.x][square.y]];
      if (current.colour != piece.colour) {
        if (current.type == PieceType::King) {
          Square origin = piece.square;
          attacks.check = true;
          while (!(origin == square)) {
            attacks.checks[origin.x][origin.y] = true;
            origin.x += a, origin.y += b;
          }
          while (board.in_bounds(square)) {
            attacks.attacks[square.x][square.y] = true;
            square.x += a, square.y += b;
            if (board.is_occupied(square)) {
              break;
            }
          }
        } else {
          PinType &pinned = attacks.pins[board.board[square.x][square.y]];
          while (board.in_bounds(square)) {
            square.x += a, square.y += b;
            if (board.is_occupied(square)) {
              Piece &target = board.pieces[board.board[square.x][square.y]];
              if (target.type == PieceType::King &&
                  target.colour != piece.colour) {
                pinned = pin;
              }
              break;
            }
          }
        }
      }
      break;
    }
    square.x += a, square.y += b;
  }
};

void get_attack_inner_single(Piece &piece, Board &board, Attacks &attacks,
                             uint8_t a, uint8_t b) {
  Square square{piece.square.x + a, piece.square.y + b};
  if (board.in_bounds(square)) {
    attacks.attacks[square.x][square.y] = true;
    if (board.board[square.x][square.y] != EMPTY) {
      Piece &current = board.pieces[board.board[square.x][square.y]];
      if (current.type == PieceType::King && current.colour != piece.colour) {
        attacks.checks[piece.square.x][piece.square.y] = true;
        attacks.check = true;
      }
    }
  }
};

void get_pawn_attacks(Piece &piece, Board &board, Attacks &attacks) {
  uint8_t step = piece.colour == Colour::White ? 1 : -1;
  // Diagonal pawn capture
  Square square{piece.square.x + 1, piece.square.y + step};
  if (board.in_bounds(square)) {
    attacks.attacks[square.x][square.y] = true;
    if (board.board[square.x][square.y] != EMPTY) {
      Piece &target = board.pieces[board.board[square.x][square.y]];
      if (target.type == PieceType::King && target.colour != piece.colour) {
        attacks.checks[piece.square.x][piece.square.y] = true;
        attacks.check = true;
      }
    }
  }
  Square square2{piece.square.x - 1, piece.square.y + step};
  if (board.in_bounds(square2)) {
    attacks.attacks[square2.x][square2.y] = true;
    if (board.board[square2.x][square2.y] != EMPTY) {
      Piece &target = board.pieces[board.board[square2.x][square2.y]];
      if (target.type == PieceType::King && target.colour != piece.colour) {
        attacks.checks[piece.square.x][piece.square.y] = true;
        attacks.check = true;
      }
    }
  }
}

void get_knight_attacks(Piece &piece, Board &board, Attacks &attacks) {
  get_attack_inner_single(piece, board, attacks, 1, 2);
  get_attack_inner_single(piece, board, attacks, 1, -2);
  get_attack_inner_single(piece, board, attacks, -1, 2);
  get_attack_inner_single(piece, board, attacks, -1, -2);
  get_attack_inner_single(piece, board, attacks, 2, 1);
  get_attack_inner_single(piece, board, attacks, 2, -1);
  get_attack_inner_single(piece, board, attacks, -2, 1);
  get_attack_inner_single(piece, board, attacks, -2, -1);
}

void get_bishop_attacks(Piece &piece, Board &board, Attacks &attacks) {
  get_attack_inner(piece, board, attacks, 1, 1, PinType::LDiagonal);
  get_attack_inner(piece, board, attacks, -1, -1, PinType::LDiagonal);
  get_attack_inner(piece, board, attacks, -1, 1, PinType::RDiagonal);
  get_attack_inner(piece, board, attacks, 1, -1, PinType::RDiagonal);
}

void get_rook_attacks(Piece &piece, Board &board, Attacks &attacks) {
  get_attack_inner(piece, board, attacks, 1, 0, PinType::Horizontal);
  get_attack_inner(piece, board, attacks, -1, 0, PinType::Horizontal);
  get_attack_inner(piece, board, attacks, 0, 1, PinType::Vertical);
  get_attack_inner(piece, board, attacks, 0, -1, PinType::Vertical);
}

void get_queen_attacks(Piece &piece, Board &board, Attacks &attacks) {
  get_attack_inner(piece, board, attacks, 1, 1, PinType::LDiagonal);
  get_attack_inner(piece, board, attacks, -1, -1, PinType::LDiagonal);
  get_attack_inner(piece, board, attacks, -1, 1, PinType::RDiagonal);
  get_attack_inner(piece, board, attacks, 1, -1, PinType::RDiagonal);
  get_attack_inner(piece, board, attacks, 1, 0, PinType::Horizontal);
  get_attack_inner(piece, board, attacks, -1, 0, PinType::Horizontal);
  get_attack_inner(piece, board, attacks, 0, 1, PinType::Vertical);
  get_attack_inner(piece, board, attacks, 0, -1, PinType::Vertical);
}

void get_king_attacks(Piece &piece, Board &board, Attacks &attacks) {
  get_attack_inner_single(piece, board, attacks, 1, 1);
  get_attack_inner_single(piece, board, attacks, -1, 1);
  get_attack_inner_single(piece, board, attacks, 1, -1);
  get_attack_inner_single(piece, board, attacks, -1, -1);
  get_attack_inner_single(piece, board, attacks, 1, 0);
  get_attack_inner_single(piece, board, attacks, -1, 0);
  get_attack_inner_single(piece, board, attacks, 0, 1);
  get_attack_inner_single(piece, board, attacks, 0, -1);
}

void Piece::get_attacks(Board &board, Attacks &attacks) {
  switch (this->type) {
  case PieceType::Pawn:
    return get_pawn_attacks(*this, board, attacks);
  case PieceType::Knight:
    return get_knight_attacks(*this, board, attacks);
  case PieceType::Bishop:
    return get_bishop_attacks(*this, board, attacks);
  case PieceType::Rook:
    return get_rook_attacks(*this, board, attacks);
  case PieceType::Queen:
    return get_queen_attacks(*this, board, attacks);
  case PieceType::King:
    return get_king_attacks(*this, board, attacks);
  }
}