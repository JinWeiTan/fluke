#pragma once

#include "engine/board.hpp"
#include <cstdint>
#include <sstream>
#include <variant>
#include <vector>

enum MessageType: uint8_t {
  ClientConnectType,
  ClientMoveType,
  ServerMoveType,
  ServerCloseType,
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
  std::string encode();
};

struct ServerClose {
  std::string encode();
};

struct Message {
  using MessageData = std::variant<ClientConnect, ClientMove, ServerMove, ServerClose>;
  MessageData data;
  std::string encode();
  static Message decode(std::string_view buffer);
};

