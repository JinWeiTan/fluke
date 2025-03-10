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
  static Message decode(std::string_view buffer);
};

struct ClientMove {
  uint8_t move;
  static Message decode(std::string_view buffer);
};

struct ServerMove {
  Position *move;
  std::vector<Position *> moves;
  std::string encode();
};

struct Message {
  using MessageData = std::variant<ClientConnect, ClientMove, ServerMove>;
  MessageData data;
  std::string encode();
  static Message decode(std::string_view buffer);
};