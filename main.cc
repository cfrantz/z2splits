#include <gflags/gflags.h>
#include "game.h"
#include "config.h"

#include "remote.h"
#include "splits_screen.h"

DEFINE_string(config, "config.textpb", "Config file");
DEFINE_bool(enable_remote, false, "Enable remote splits");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  Game::Config config;
  z2splits::ConfigLoader loader;

  loader.Load(FLAGS_config);

  config.graphics.title = "splits";
  config.graphics.width = 360;
  config.graphics.height = 384;
  config.graphics.fullscreen = false;

  SplitsScreen *s = new SplitsScreen();
  Remote remote(s);

  // TODO load splits from command line
  s->load_splits(loader.config());


  if (FLAGS_enable_remote) {
      remote.Start();
  }

  Game game(config);
  game.loop(s);

  return 0;
}
