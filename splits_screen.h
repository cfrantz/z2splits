#pragma once

#include <memory>
#include <string>
#include <vector>

#include "audio.h"
#include "input.h"
#include "graphics.h"
#include "screen.h"
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
      Split(const std::string& name, int count, int best, int average);
      std::string name;
      unsigned int count, best, average, current;
    };

    enum class ComparisonMode { BEST, AVERAGE };

    std::string title_, file_;
    std::vector<Split> splits_;
    unsigned int index_, time_;
    bool running_;
    std::unique_ptr<Text> text_;
    ComparisonMode mode_;

    void stop();
    void reset();
    void go();
    void next();
    void back();
    void save();

    void draw_time(Graphics& graphics, int ms, int x, int y) const;

    void draw_corner(Graphics& graphics, int x, int y) const;
    void draw_hline(Graphics& graphics, int x, int y, int width) const;
    void draw_vline(Graphics& graphics, int x, int y, int height) const;
};
