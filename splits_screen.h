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
    bool load(const std::string& file);
    void save();
    bool update(const Input& input, Audio& audio, unsigned int elapsed);

    void draw(Graphics& graphics) const;

  private:

    struct Split {
      Split(const std::string& name, int hint, unsigned int best);
      std::string name;
      unsigned int current, best;
      int hint;
    };

    std::string title_, file_;
    std::vector<Split> splits_;
    unsigned int index_, offset_, time_, delay_, visible_;
    bool running_;
    std::unique_ptr<Text> text_;
    std::unique_ptr<SpriteMap> maps_, fairy_, triforce_;

    bool is_gold_split(int split) const;

    void stop();
    void reset();
    void go();
    void next();
    void skip();
    void back();

    void scroll_offset();
    void scroll_up();
    void scroll_down();

    void draw_time(Graphics& graphics, int ms, int x, int y) const;

    void draw_corner(Graphics& graphics, int x, int y) const;
    void draw_hline(Graphics& graphics, int x, int y, int width) const;
    void draw_vline(Graphics& graphics, int x, int y, int height) const;
};
