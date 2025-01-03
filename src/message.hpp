#pragma once

#include "engine/piece.hpp"
#include <cstdint>
#include <sstream>
#include <variant>
#include <vector>

enum MessageType {
  ClientConnectType,
  ClientMoveType,
  ServerMoveType,
};

struct Message;

struct PerSocketData {
  uint8_t client_id;
  uint8_t game_id;
};

struct ClientConnect {
  Colour colour;
  ClientConnect(Colour colour) : colour(colour){};
  static Message decode(std::string_view buffer);
};

struct ClientMove {
  Square from;
  Square to;
  MoveType move;
  PieceType piece;
  ClientMove(Square from, Square to, MoveType move, PieceType piece)
      : from(from), to(to), move(move), piece(piece){};
  static Message decode(std::string_view buffer);
};

struct ServerMove {
  bool is_check_white;
  bool is_check_black;
  std::vector<Move> moves;
  ServerMove(bool is_check_white, bool is_check_black, std::vector<Move> moves)
      : is_check_white(is_check_white), is_check_black(is_check_black),
        moves(moves){};
  std::string encode();
};

struct Message {
  using MessageData = std::variant<ClientConnect, ClientMove, ServerMove>;
  MessageData data;
  std::string encode();
  Message(MessageData data) : data(data){};
  static Message decode(std::string_view buffer);
};