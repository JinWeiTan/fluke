#include "server.hpp"
#include <thread>
#include <variant>

void Server::handle_message(Message &message, PerSocketData *socket_data) {
  if (holds_alternative<ClientConnect>(message.data)) {
    this->handle_client_connect(message, socket_data);
  } else if (holds_alternative<ClientMove>(message.data)) {
    this->handle_client_move(message, socket_data);
  }
}

void Server::handle_client_connect(Message &message,
                                   PerSocketData *socket_data) {
  ClientConnect data = std::get<ClientConnect>(message.data);
  Player player(socket_data->client_id, data.colour);
  Game game = Game{this->game_id, player, Engine::init()};
  this->games[this->game_id] = game;
  socket_data->game_id = this->game_id;
  this->game_id++;
  if (player.colour == Colour::White) {
    game.engine.move->get_moves();
    Message message = Message(ServerMove(game.engine.move));
    this->send_message(message, socket_data);
  }
}

void Server::handle_client_move(Message &message, PerSocketData *socket_data) {
  ClientMove data = std::get<ClientMove>(message.data);
  Game &game = this->games[socket_data->game_id];
  game.engine.make_move(data.move);
  // game.engine.board->display();

  int move = game.engine.search_moves(4);
  if (move == -1) {
    Message server_message = Message(ServerClose());
    return this->send_message(server_message, socket_data);
  }
  game.engine.clean_moves(move);
  game.engine.make_move(move);
  Message server_message = Message(ServerMove(game.engine.move));
  this->send_message(server_message, socket_data);
}

void Server::handle_client_close(PerSocketData socket_data) {
  this->games.erase(socket_data.game_id);
  this->sockets.erase(socket_data.client_id);
}

void Server::send_message(Message &message, PerSocketData *socket_data) {
  auto ws = this->sockets[socket_data->client_id];
  ws->send(message.encode(), uWS::OpCode::BINARY);
}