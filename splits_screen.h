#pragma once

#include <memory>
#include <string>
#include <vector>

#include "audio.h"
#include "input.h"
#include "graphics.h"
#include "screen.h"
#include "spritemap.h"
#include "text.h"
#include "proto/config.pb.h"

class SplitsScreen : public Screen {
  public:

    void init();
    bool load_splits(const z2splits::Config& config);
    bool update(const Input& input, Audio& audio, unsigned int elapsed);

    void draw(Graphics& graphics) const;

  private:

    std::string title_, file_;
    int game_;
    int category_;
    std::vector<z2splits::Split> splits_;
    unsigned int index_, time_;
    bool running_;
    std::unique_ptr<Text> text_;
    std::unique_ptr<SpriteMap> maps_;

    void stop();
    void reset();
    void go();
    void next();
    void back();

    void draw_time(Graphics& graphics, int ms, int x, int y) const;

    void draw_corner(Graphics& graphics, int x, int y) const;
    void draw_hline(Graphics& graphics, int x, int y, int width) const;
    void draw_vline(Graphics& graphics, int x, int y, int height) const;

    z2splits::Config config_;
};
