#pragma once

#include "engine/board.hpp"
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
  uint8_t move;
  ClientMove(uint16_t move) : move(move){};
  static Message decode(std::string_view buffer);
};

struct ServerMove {
  bool is_check_white;
  bool is_check_black;
  std::vector<Position*> moves;
  ServerMove(bool is_check_white, bool is_check_black, std::vector<Position*> moves)
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