#include "game.h"

#include "splits_screen.h"

int main(int, char**) {
  Game::Config config;

  config.graphics.title = "splits";
  config.graphics.width = 256;
  config.graphics.height = 256;
  config.graphics.fullscreen = false;

  SplitsScreen *s = new SplitsScreen();

  // TODO load splits from command line
  s->load_splits("tlozhacks.txt");

  Game game(config);
  game.loop(s);

  return 0;
}
