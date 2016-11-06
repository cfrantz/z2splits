#include <gflags/gflags.h>
#include "game.h"
#include "config.h"

#include "splits_screen.h"

DEFINE_string(config, "config.textpb", "Config file");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  Game::Config config;
  z2splits::ConfigLoader loader;

  loader.Load(FLAGS_config);

  config.graphics.title = "splits";
  config.graphics.width = 256;
  config.graphics.height = 384;
  config.graphics.fullscreen = false;

  SplitsScreen *s = new SplitsScreen();

  // TODO load splits from command line
  s->load_splits(loader.config());

  Game game(config);
  game.loop(s);

  return 0;
}
