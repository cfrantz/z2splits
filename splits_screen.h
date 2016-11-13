#pragma once

#include <memory>
#include <map>
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
    void save_splits();
    bool update(const Input& input, Audio& audio, unsigned int elapsed);

    void draw(Graphics& graphics) const;

    void stop();
    void reset();
    void go();
    void next();
    void next(const std::string& nickname);
    void skip();
    void back();

  private:
    std::string title_, file_;
    int game_;
    int category_;
    z2splits::Run run_;
    z2splits::SavedRuns saved_runs_;
    int index_;
    int offset_;
    int time_;
    int delay_;
    int visible_;
    int number_;
    bool running_;
    std::unique_ptr<Text> text_;
    std::unique_ptr<SpriteMap> maps_;
    std::unique_ptr<SpriteMap> fairy_;
    std::unique_ptr<SpriteMap> triforce_;
    std::map<std::string, std::unique_ptr<SpriteMap>> images_;

    void scroll_offset();
    void scroll_up();
    void scroll_down();
    const z2splits::Split* find_same_split(const std::string& name,
                                           const z2splits::Run& run) const;
    bool is_gold_split(const z2splits::Split& split) const;

    void load_resources(const z2splits::GameConfig& gcfg);
    void draw_time(Graphics& graphics, int ms, int x, int y) const;

    void draw_corner(Graphics& graphics, int x, int y) const;
    void draw_hline(Graphics& graphics, int x, int y, int width) const;
    void draw_vline(Graphics& graphics, int x, int y, int height) const;

    z2splits::Config config_;
};
