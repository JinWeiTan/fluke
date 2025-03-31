#include "uci.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> result;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    result.push_back(item);
  }
  return result;
}

template <typename T, typename U> U Iterator<T, U>::next() {
  U item;
  if (!this->end()) {
    item = this->list[this->count];
    this->count++;
  }
  return item;
}

template <typename T, typename U> bool Iterator<T, U>::end() {
  return this->count >= this->list.size();
}

Square parse_square(std::string &square) {
  char x = square[0], y = square[1];
  return Square{uint8_t(7 - x + 'a'), uint8_t(y - '1')};
}

void parse_move(Engine &engine, std::string &command) {
  char x1 = command[0], y1 = command[1], x2 = command[2], y2 = command[3];
  Square from = Square{uint8_t(7 - x1 + 'a'), uint8_t(y1 - '1')};
  Square to = Square{uint8_t(7 - x2 + 'a'), uint8_t(y2 - '1')};
  std::vector<Move> moves;
  engine.board.get_moves(moves, opposite(engine.move.colour));

  Move best_move;
  for (int i = 0; i < moves.size(); i++) {
    bool is_move = true;
    if (command.size() == 5) {
      is_move = moves[i].type ==
                MoveType(MoveType::PromoteKnight + PromoteMap.at(command[4]));
    }
    if (moves[i].from == from && moves[i].to == to && is_move) {
      best_move = moves[i];
      break;
    }
  }

  engine.board = engine.board.make_move(best_move);
  engine.move = best_move;
}

void parse_fen(Engine &engine, Commands &commands) {
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      engine.board.board[i][j] = EMPTY;
    }
  }
  for (auto &&piece : engine.board.pieces) {
    piece.taken = true;
  }

  FEN placement = FEN{commands.next(), 0};
  std::map<char, int> PieceMap = FENPieceMap;
  int x = 0, y = 0;
  engine.board.piece_count = 0;
  while (!placement.end()) {
    char ch = placement.next();
    if (ch == '/') {
      x = 0;
      y++;
    } else if (isdigit(ch)) {
      x += ch - '0';
    } else {
      Piece &piece = engine.board.pieces[PieceMap[ch]];
      piece.square.x = 7 - x, piece.square.y = 7 - y;
      engine.board.board[7 - x][7 - y] = piece.id;
      piece.taken = false;
      if (piece.type > PieceType::Pawn && piece.type < PieceType::King) {
        engine.board.piece_count += 1;
      }
      PieceMap[ch] += 1;
      x += 1;
    }
  }

  if (commands.next() == "w") {
    engine.move.colour = Colour::Black;
  } else {
    engine.move.colour = Colour::White;
  }

  FEN castling = FEN{commands.next(), 0};
  engine.board.castling = Castling{};
  while (!castling.end()) {
    char ch = castling.next();
    if (ch == 'Q') {
      engine.board.castling.white_queenside = true;
    } else if (ch == 'q') {
      engine.board.castling.black_queenside = true;
    } else if (ch == 'K') {
      engine.board.castling.white_kingside = true;
    } else if (ch == 'k') {
      engine.board.castling.black_kingside = true;
    }
  }

  std::string command = commands.next();
  if (command != "-") {
    engine.board.double_step = parse_square(command).x;
  }

  commands.count += 2;
}

void parse_go(Engine &engine, Commands &commands) {
  int btime = 600000, wtime = 600000;
  while (!commands.end()) {
    std::string command = commands.next();
    if (command == "wtime") {
      std::string command = commands.next();
      wtime = std::stoi(command);
    } else if (command == "btime") {
      std::string command = commands.next();
      btime = std::stoi(command);
    } else if (command == "perft") {
      std::string ch = commands.next();
      int depth = ch == "" ? 5 : (ch[0] - '0');
      engine.perft(depth);
      return;
    }
  }
  int time = engine.move.colour == Colour::White ? btime : wtime;
  Mode mode = Mode{6, -3};
  if (time <= 10000) {
    mode = Mode{4, -2};
    std::cout << "info mode fast" << std::endl;
  } else if (time <= 180000) {
    mode = Mode{5, -4};
    std::cout << "info mode medium" << std::endl;
  } else {
    std::cout << "info mode slow" << std::endl;
  }
  BestMove best_move = engine.search_moves(mode);
  if (!best_move.end) {
    engine.board = engine.board.make_move(best_move.move);
    engine.move = best_move.move;
    std::cout << "bestmove " << engine.move.format() << std::endl;
  }
}

void UCI::bench() {
  NodeCount = 0;
  uint64_t start = Engine::get_timestamp();
  for (auto &&position : FENPositions) {
    Commands fen = Commands{split(position, ' '), 0};
    uint64_t before = NodeCount;
    parse_fen(this->engine, fen);
    this->engine.search_moves(Mode{4, -2});
    // std::cout << position << " fen " << NodeCount - before << " nodes\n";
  }
  uint64_t end = Engine::get_timestamp();
  uint64_t nps = NodeCount / (end == start ? 1 : end - start) * 1000;
  std::cout << NodeCount << " nodes " << nps << " nps\n";
}

void parse_position(UCI &uci, Commands &commands) {
  if (commands.next() == "fen") {
    if (!uci.started) {
      parse_fen(uci.engine, commands);
      uci.started = true;
    } else {
      commands.count += 6;
    }
  }
  if (commands.next() == "moves") {
    std::string command;
    while (!commands.end()) {
      command = commands.next();
    }
    parse_move(uci.engine, command);
  }
  // std::cout << uci.engine.board.format() << std::endl;
}

void UCI::run_loop() {
  std::cout << "id name Fluke\n";
  std::cout << "id author JinWeiTan\n";

  std::cout << "option name Threads type spin default 1 min 1 max 1\n";
  std::cout << "option name Hash type spin default 1 min 1 max 1\n";

  std::cout << "uciok\n";

  while (true) {
    std::string input;
    std::getline(std::cin, input);
    Commands commands = Commands{split(input, ' '), 0};

    std::string command = commands.next();
    if (command == "go") {
      parse_go(this->engine, commands);
    } else if (command == "position") {
      parse_position((*this), commands);
    } else if (command == "isready") {
      std::cout << "readyok\n";
    } else if (command == "ucinewgame") {
      this->engine.board = Board::init();
      this->started = false;
    }  else if (command == "display") {
      std::cout << this->engine.board.format() << "\n";
    } else if (command == "quit") {
      exit(0);
    }
  }
}