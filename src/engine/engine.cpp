#pragma once

#include "engine.hpp"

Engine Engine::init() {
  Position* move = new Position{Board::init(), Move{}};
  return Engine{move};
}