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

class SplitsScreen : public Screen {
  public:

    void init();
    bool load_splits(const std::string& file);
    bool update(const Input& input, Audio& audio, unsigned int elapsed);

    void draw(Graphics& graphics) const;

  private:

    struct Split {
      Split(const std::string& name);
      std::string name;
      unsigned int current;
    };

    std::string title_, file_;
    std::vector<Split> splits_;
    unsigned int index_, time_;
    bool running_, killed_[7];
    std::unique_ptr<Text> text_;
    std::unique_ptr<SpriteMap> bosses_;

    void stop();
    void reset();
    void go();
    void next();
    void back();

    void toggle_boss(int boss);

    void draw_time(Graphics& graphics, int ms, int x, int y) const;

    void draw_corner(Graphics& graphics, int x, int y) const;
    void draw_hline(Graphics& graphics, int x, int y, int width) const;
    void draw_vline(Graphics& graphics, int x, int y, int height) const;
};
