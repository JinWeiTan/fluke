#include "message.hpp"
#include "engine/piece.hpp"
#include <cstdint>
#include <sstream>
#include <variant>
#include <vector>

std::string Message::encode() {
  if (holds_alternative<ServerMove>(this->data)) {
    return std::get<ServerMove>(this->data).encode();
  }
}

Message Message::decode(std::string_view buffer) {
  switch (buffer[0]) {
  case MessageType::ClientConnectType:
    return ClientConnect::decode(buffer);
  case MessageType::ClientMoveType:
    return ClientMove::decode(buffer);
  }
}

Message ClientConnect::decode(std::string_view buffer) {
  return Message(ClientConnect((Colour)buffer[1]));
}

Message ClientMove::decode(std::string_view buffer) {
  Square from((uint8_t)buffer[1], (uint8_t)buffer[2]);
  Square to((uint8_t)buffer[3], (uint8_t)buffer[4]);
  return Message(ClientMove(from, to, (MoveType)buffer[5], (PieceType)buffer[6]));
}

std::string ServerMove::encode() {
  std::string buffer = "";
  buffer.push_back(MessageType::ServerMoveType);
  buffer.push_back(this->is_check_white);
  buffer.push_back(this->is_check_black);
  for (auto &&move : this->moves) {
    buffer.push_back(move.piece.id);
    buffer.push_back(move.type);
    buffer.push_back(move.to.x);
    buffer.push_back(move.to.y);
  }
  return buffer;
}