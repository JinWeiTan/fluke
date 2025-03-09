#pragma once

#include "engine/engine.hpp"
#include "message.hpp"
#include "uwebsockets/App.h"
#include <mutex>
#include <queue>

struct Player {
  uint8_t id;
  Colour colour;
};

struct Game {
  uint8_t id;
  Player player;
  Engine engine;
};

struct Server {
  uWS::Loop *loop;
  uint8_t socket_id;
  uint8_t game_id;
  std::map<uint8_t, uWS::WebSocket<false, true, PerSocketData> *> sockets;
  std::map<uint8_t, Game> games;

  void handle_message(Message &message, PerSocketData *socket_data);
  void handle_client_connect(Message &message, PerSocketData *socket_data);
  void handle_client_move(Message &message, PerSocketData *socket_data);
  void handle_client_close(PerSocketData socket_data);
  void send_message(Message &message, PerSocketData *socket_data);
};