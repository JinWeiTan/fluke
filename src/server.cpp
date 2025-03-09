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
    game.engine.move->board.get_moves(game.engine.move->next, player.colour);
    Message message = Message(ServerMove(false, false, game.engine.move->next));
    this->send_message(message, socket_data);
  }
}

void Server::handle_client_move(Message &message, PerSocketData *socket_data) {
  ClientMove data = std::get<ClientMove>(message.data);
  Game &game = this->games[socket_data->game_id];
  game.engine.move = game.engine.move->next[data.move];
  game.engine.move->board.display();

  game.player.colour = opposite(game.player.colour);
  bool is_check_white = game.engine.move->board.is_check(Colour::White);
  bool is_check_black = game.engine.move->board.is_check(Colour::Black);
  game.engine.move->board.get_moves(game.engine.move->next, game.player.colour);
  Message server_message =
      Message(ServerMove(is_check_white, is_check_black, game.engine.move->next));
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

// void Server::send_message(Message &message) {
//   this->loop->defer([this, &message]() {
//     auto ws = this->sockets[message.client_id];
//     ws->send(message.encode(), uWS::OpCode::BINARY);
//   });
// }