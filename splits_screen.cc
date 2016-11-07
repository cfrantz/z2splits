#include "splits_screen.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "util/logging.h"
#include "util/os.h"
#include "util/strutil.h"

static const SDL_Scancode KEY_NEXT = SDL_SCANCODE_SPACE;
static const SDL_Scancode KEY_ERASE = SDL_SCANCODE_ESCAPE;

void SplitsScreen::init() {
  index_ = time_ = 0;
  running_ = false;

  text_.reset(new Text("text.png"));
  maps_.reset(new SpriteMap("maps.png", 1, 256, 64));
}

void SplitsScreen::load_resources(const z2splits::GameConfig& gcfg) {
  for(const auto& ir : gcfg.resource()) {
    if (ir.type() == z2splits::ImageType::SpriteSheet) {
        const auto& ss = ir.sprite();

        //std::string filename = os::path::Join({ir.path(), ir.filename()});
        const std::string& filename = ir.filename();
        LOG(INFO, "resource: ", filename);
        images_[ir.id()].reset(new SpriteMap(filename, ss.columns(),
                                             ss.width(), ss.height()));
    } else {
      LOG(ERROR, "ImageResource.type ", ir.type(), " not suppoted.");
    }
  }
}

bool SplitsScreen::load_splits(const z2splits::Config& config) {
  config_ = config;

  game_ = 0;
  category_ = 0;

  const z2splits::GameConfig& gcfg = config_.game(game_);
  const z2splits::CategoryConfig& ccfg = gcfg.category(category_);


  title_ = gcfg.name() + ": " + ccfg.name();

  load_resources(gcfg);
  for(const z2splits::Split& split : ccfg.splits()) {
      splits_.push_back(split);
  }

  return true;
}

bool SplitsScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  z2splits::ControlAction action;
  if (running_) {
    splits_[index_].set_time_ms(splits_[index_].time_ms() + elapsed);
    time_ += elapsed;

    for(const auto& c : config_.controls()) {
        action = input.key_pressed(SDL_Scancode(c.scancode())) ?
                 c.action() : z2splits::ControlAction::UNKNOWN;
        switch(action) {
          case z2splits::ControlAction::SPLIT: next(); break;
          case z2splits::ControlAction::BACK: back(); break;
          case z2splits::ControlAction::STOP: stop(); break;
          default:
            ; // do nothing
        }
    }
  } else {
    for(const auto& c : config_.controls()) {
        action = input.key_pressed(SDL_Scancode(c.scancode())) ?
                 c.action() : z2splits::ControlAction::UNKNOWN;
        switch(action) {
          case z2splits::ControlAction::SPLIT: go(); break;
          case z2splits::ControlAction::RESET: reset(); break;
          default:
            ; // do nothing
        }
    }
  }

  return true;
}

void SplitsScreen::draw(Graphics& graphics) const {
  text_->draw(
      graphics, title_, graphics.width() / 2, 8, Text::Alignment::CENTER);

  const int right = graphics.width() - 16;

  unsigned int total = 0;
  int offset = 0;
  const int max_shown = 16;

  if (splits_.size() > max_shown) {
    offset = index_ - max_shown + 1;
    if (offset < 0) offset = 0;
  }

  for (size_t i = 0; i < splits_.size(); ++i) {
    const z2splits::Split& s = splits_[i];
    const int y = 16 * (i - offset) + 40;

    total += s.time_ms();

    if (i >= offset && i < offset + max_shown) {
      text_->draw(graphics, s.name(), 16, y);
      if (i <= index_) draw_time(graphics, total, right, y);
    }
  }

  if (index_ < splits_.size()) {
    const z2splits::Split& s = splits_[index_];
    int n = -1;
    int x, y;

    std::vector<std::string> hint = google::protobuf::Split(s.image().id(), ":");
    switch(s.image().origin()) {
        case z2splits::WindowGravity::UpperLeft:
          x = 0; y = 0; break;
        case z2splits::WindowGravity::UpperRight:
          x = graphics.width(); y = 0; break;
        case z2splits::WindowGravity::LowerRight:
          x = graphics.width(); y = graphics.height(); break;
        case z2splits::WindowGravity::LowerLeft:
        default:
          x = 0;                y = graphics.height(); break;
    }
    x += s.image().x();
    y += s.image().y();

    if (hint.size() == 2) {
      safe_strto32(hint[1], &n);
      const auto& im = images_.find(hint[0]);
      if (im != images_.end()) {
          im->second->draw(graphics, n, x, y);
      }
    }
  }

  draw_corner(graphics, 1, 1);
  draw_corner(graphics, graphics.width() - 7, 1);
  draw_hline(graphics, 8, 2, graphics.width() - 17);

  draw_corner(graphics, 1, 25);
  draw_corner(graphics, graphics.width() - 7, 25);
  draw_hline(graphics, 8, 26, graphics.width() - 17);

  draw_corner(graphics, 1, graphics.height() - 7);
  draw_corner(graphics, graphics.width() - 7, graphics.height() - 7);
  draw_hline(graphics, 8, graphics.height() - 6, graphics.width() - 17);

  draw_vline(graphics, 2, 8, 15);
  draw_vline(graphics, graphics.width() - 6, 8, 15);

  draw_vline(graphics, 2, 32, graphics.height() - 41);
  draw_vline(graphics, graphics.width() - 6, 32, graphics.height() - 41);

  // TODO draw hint
}

void SplitsScreen::stop() {
  running_ = false;
}

void SplitsScreen::reset() {
  running_ = false;
  index_ = time_ = 0;
  for (size_t i = 0; i < splits_.size(); ++i) {
      splits_[i].set_time_ms(0);
  }
}

void SplitsScreen::go() {
  if (index_ < splits_.size()) running_ = true;
}

void SplitsScreen::next() {
  ++index_;

  if (index_ >= splits_.size()) stop();
}

void SplitsScreen::back() {
  if (index_ > 0) {
    splits_[index_ - 1].set_time_ms(splits_[index_ - 1].time_ms() +
                                    splits_[index_].time_ms());
    splits_[index_].set_time_ms(0);
    --index_;
  } else {
    stop();
  }
}

void SplitsScreen::draw_time(
    Graphics& graphics, int ms, int x, int y) const {
  std::ostringstream out;

  if (ms < 0) {
    out << '-';
    ms = -ms;
  }

  const int hours = ms / 3600000;
  const int minutes = (ms / 60000) % 60;
  const int seconds = (ms / 1000) % 60;
  const int tenths = (ms / 100) % 10;

  if (minutes > 0 || hours > 0) {
    if (hours > 0) {
      out << hours << ':' << std::setw(2) << std::setfill('0');
    }
    out << minutes << ':';
    out << std::setw(2) << std::setfill('0') << seconds;
  } else {
    out << seconds << '.' << tenths;
  }

  text_->draw(graphics, out.str(), x, y, Text::Alignment::RIGHT);
}

void SplitsScreen::draw_corner(Graphics& graphics, int x, int y) const {
  const SDL_Rect r = { x, y, 6, 6 };

  graphics.draw_rect(&r, 0xdb2b00ff, true);
  graphics.draw_rect(&r, 0xffffffff, false);
}

void SplitsScreen::draw_hline(
    Graphics& graphics, int x, int y, int width) const {
  for (int i = 0; i < 4; ++i) {
    const int color = i % 3 == 0 ? 0xffffffff : 0x233befff;
    graphics.draw_line(x, y + i, x + width, y + i, color);
  }
}

void SplitsScreen::draw_vline(
    Graphics& graphics, int x, int y, int height) const {
  for (int i = 0; i < 4; ++i) {
    const int color = i % 3 == 0 ? 0xffffffff : 0x233befff;
    graphics.draw_line(x + i, y, x + i, y + height, color);
  }
}
