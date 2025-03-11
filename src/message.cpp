#include "message.hpp"
#include "engine/piece.hpp"
#include <cstdint>
#include <sstream>
#include <variant>
#include <vector>

std::string Message::encode() {
  if (holds_alternative<ServerMove>(this->data)) {
    return std::get<ServerMove>(this->data).encode();
  } else if (holds_alternative<ServerClose>(this->data)) {
    return std::get<ServerClose>(this->data).encode();
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
  return Message(ClientMove((uint8_t)buffer[1]));
}

std::string ServerClose::encode() {
  std::string buffer = "";
  buffer.push_back(MessageType::ServerCloseType);
  return buffer;
}

std::string ServerMove::encode() {
  std::string buffer = "";
  buffer.push_back(MessageType::ServerMoveType);
  if (this->move != NULL) {
    buffer.push_back(this->move->move.piece.id);
    buffer.push_back(this->move->move.type);
    buffer.push_back(this->move->move.to.x);
    buffer.push_back(this->move->move.to.y);
  } else {
    for (size_t i = 0; i < 4; i++) {
      buffer.push_back(0);
    }
  }

  for (auto &&move : this->moves) {
    buffer.push_back(move->move.piece.id);
    buffer.push_back(move->move.type);
    buffer.push_back(move->move.to.x);
    buffer.push_back(move->move.to.y);
  }
  return buffer;
}