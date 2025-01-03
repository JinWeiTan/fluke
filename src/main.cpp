#include "message.hpp"
#include "server.hpp"
#include "util/AsyncFileReader.h"
#include "util/AsyncFileStreamer.h"
#include "util/Middleware.h"
#include "uwebsockets/App.h"
#include <map>
#include <queue>

Server server;

auto on_open = [](auto *ws) {
  server.sockets[server.socket_id] = ws;
  ws->getUserData()->client_id = server.socket_id;
  server.socket_id++;
};

auto on_message = [](auto *ws, std::string_view buffer, uWS::OpCode opCode) {
  Message message = Message::decode(buffer);
  server.handle_message(message, ws->getUserData());
};

auto on_close = [](auto *ws, int code, std::string_view message) {};

int main() {
  const int port = 3000;

  uWS::App app = uWS::App();
  server.loop = uWS::Loop::get();
  std::cout << "Listening on port " << port << std::endl;

  uWS::App()
      .ws<PerSocketData>(
          "/*", {.open = on_open, .message = on_message, .close = on_close})
      .get("/",
           [](auto res, auto req) {
             res->writeHeader("Content-Type", "text/html; charset=utf8");
             AsyncFileReader page_contents("./public/index.html");
             res->end(page_contents.peek(0));
           })
      .get("/js/*",
           [](auto res, auto req) {
             res->writeHeader("Content-Type", "text/javascript");
             AsyncFileReader page_contents("./public/js/script.js");
             res->end(page_contents.peek(0));
           })
      .get("/css/*",
           [](auto res, auto req) {
             res->writeHeader("Content-Type", "text/css");
             AsyncFileReader page_contents("./public/css/style.css");
             res->end(page_contents.peek(0));
           })
      .get("/assets/*",
           [](auto res, auto req) {
             std::string path = "./public" + std::string(req->getUrl());
             res->writeHeader("Content-Type", "image/png");
             AsyncFileReader page_contents(path);
             res->end(page_contents.peek(0));
           })
      .listen(port, [](auto *listen_socket) {})
      .run();

  return 0;
}