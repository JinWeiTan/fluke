#include "uci.hpp"

int main(int argc, char *argv[]) {
  UCI uci = UCI{Engine::init()};
  if (argc > 1) {
    uci.bench();
  }
  uci.run_loop();
  return 0;
}