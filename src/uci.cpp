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

std::string format_board(Board &board) {
  std::string output;
  for (int i = 7; i >= 0; i--) {
    for (int j = 7; j >= 0; j--) {
      output += "+---";
    }
    output += "+\n";
    for (int j = 7; j >= 0; j--) {
      if (board.board[j][i] == EMPTY) {
        output += "|   ";
      } else {
        Piece &piece = board.pieces[board.board[j][i]];
        int offset = piece.colour == Colour::White ? 6 : 0;
        output += "| " + BoardName[piece.type + offset] + " ";
      }
    }
    output += "|\n";
  }
  for (int j = 7; j >= 0; j--) {
    output += "+---";
  }
  output += "+";
  return output;
}

std::string format_square(Square &square) {
  return HorizontalName[square.x] + VerticalName[square.y];
}

std::string format_move(Move &move) {
  std::string promote = "";
  if (move.type >= MoveType::PromoteKnight) {
    int offset = move.colour == Colour::White ? 4 : 0;
    promote = PromoteName[move.type - MoveType::PromoteKnight + offset];
  }
  return format_square(move.from) + format_square(move.to) + promote;
}

Square parse_square(std::string &square) {
  char x = square[0], y = square[1];
  return Square{uint8_t(7 - x + 'a'), uint8_t(y - '1')};
}

void parse_move(Engine &engine, std::string &command) {
  char x1 = command[0], y1 = command[1], x2 = command[2], y2 = command[3];
  Square from = Square{uint8_t(7 - x1 + 'a'), uint8_t(y1 - '1')};
  Square to = Square{uint8_t(7 - x2 + 'a'), uint8_t(y2 - '1')};
  MoveType move_type = MoveType(MoveType::PromoteKnight + command[4]);
  if (!engine.move->generated) {
    engine.board.get_moves(engine.move->next,
                           opposite(engine.move->move.colour));
    engine.move->generated = true;
  }

  int move_id = -1;
  for (int i = 0; i < engine.move->next.size(); i++) {
    Move &move = engine.move->next[i]->move;
    bool is_move = (command.size() == 5) ? (move.type == move_type) : true;
    if (move.from == from && move.to == to && is_move) {
      move_id = i;
    }
  }

  engine.clean_moves(move_id);
  engine.make_move(move_id);
}

void parse_fen(Engine &engine, Commands &commands) {
  for (size_t i = 0; i < 8; i++) {
    for (size_t j = 0; j < 8; j++) {
      engine.board.board[i][j] = EMPTY;
    }
  }

  FEN placement = FEN{commands.next(), 0};
  std::map<char, int> PieceMap = FENPieceMap;
  int x = 0, y = 0;
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
      PieceMap[ch] += 1;
      x += 1;
    }
  }

  if (commands.next() == "w") {
    engine.move->move.colour = Colour::Black;
  } else {
    engine.move->move.colour = Colour::White;
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

void parse_go(Engine &engine) {
  int move_id = engine.search_moves(5);
  if (move_id != -1) {
    engine.clean_moves(move_id);
    engine.make_move(move_id);
    Move &move = engine.move->move;
    std::cout << "bestmove " << format_move(move) << std::endl;
    // std::cout << format_board(engine.board) << std::endl;
  }
}

void parse_position(Engine &engine, Commands &commands) {
  if (commands.next() == "fen") {
    parse_fen(engine, commands);
  }
  if (commands.next() == "moves") {
    std::string command;
    while (!commands.end()) {
      command = commands.next();
    }
    parse_move(engine, command);
  }
  // std::cout << format_board(engine.board) << std::endl;
}

void UCI::init() {
  UCI uci = UCI{Engine::init()};

  std::cout << "id name Anchovy\n";
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
      parse_go(uci.engine);
    } else if (command == "position") {
      parse_position(uci.engine, commands);
    } else if (command == "isready") {
      std::cout << "readyok\n";
    } else if (command == "ucinewgame") {
      uci.engine.board = Board::init();
      uci.engine.clean_moves(-1);
    } else if (command == "quit") {
      exit(0);
    }
  }
}