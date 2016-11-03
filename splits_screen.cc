#include "splits_screen.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

static const SDL_Scancode KEY_NEXT = SDL_SCANCODE_SPACE;
static const SDL_Scancode KEY_ERASE = SDL_SCANCODE_ESCAPE;

void SplitsScreen::init() {
  index_ = time_ = 0;
  running_ = false;

  text_.reset(new Text("text.png"));
  bosses_.reset(new SpriteMap("bosses.png", 8, 32, 48));
}

bool SplitsScreen::load_splits(const std::string& file) {
  file_ = file;

  title_ = "Zelda 2 Randomizer";

  splits_.emplace_back("Gem 1");
  splits_.emplace_back("Gem 2");
  splits_.emplace_back("Gem 3");
  splits_.emplace_back("Gem 4");
  splits_.emplace_back("Gem 5");
  splits_.emplace_back("Gem 6");
  splits_.emplace_back("Dark Link");

  return true;
}

bool SplitsScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  if (running_) {
    splits_[index_].current += elapsed;
    time_ += elapsed;

    if (input.key_pressed(SDL_SCANCODE_SPACE)) next();
    if (input.key_pressed(SDL_SCANCODE_BACKSPACE)) back();
    if (input.key_pressed(SDL_SCANCODE_RETURN)) stop();

    if (input.key_pressed(SDL_SCANCODE_1)) toggle_boss(0);
    if (input.key_pressed(SDL_SCANCODE_2)) toggle_boss(1);
    if (input.key_pressed(SDL_SCANCODE_3)) toggle_boss(2);
    if (input.key_pressed(SDL_SCANCODE_4)) toggle_boss(3);
    if (input.key_pressed(SDL_SCANCODE_5)) toggle_boss(4);
    if (input.key_pressed(SDL_SCANCODE_6)) toggle_boss(5);
    if (input.key_pressed(SDL_SCANCODE_7)) toggle_boss(6);

  } else {
    if (input.key_pressed(SDL_SCANCODE_SPACE)) go();
    if (input.key_pressed(SDL_SCANCODE_ESCAPE)) reset();
  }

  return true;
}

void SplitsScreen::draw(Graphics& graphics) const {
  text_->draw(
      graphics, title_, graphics.width() / 2, 8, Text::Alignment::CENTER);

  const int right = graphics.width() - 16;

  unsigned int total = 0;
  for (size_t i = 0; i < splits_.size(); ++i) {
    const Split s = splits_[i];
    const int y = 16 * i + 40;

    total += s.current;

    text_->draw(graphics, s.name, 16, y);

    if (i <= index_) draw_time(graphics, total, right, y);
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

  for (int i = 0; i < 7; ++i) {
    const int x = graphics.width() * (i + 1) / 8 - 16;
    const int y = graphics.height() - 64;
    if (killed_[i]) {
      bosses_->draw(graphics, i + 8, x, y);
      bosses_->draw(graphics, 7, x, y);
    } else {
      bosses_->draw(graphics, i, x, y);
    }
  }
}

SplitsScreen::Split::Split(const std::string& name) : name(name), current(0) {}

void SplitsScreen::stop() {
  running_ = false;
}

void SplitsScreen::reset() {
  running_ = false;
  index_ = time_ = 0;
  for (size_t i = 0; i < splits_.size(); ++i) splits_[i].current = 0;
  for (int i = 0; i < 7; ++i) killed_[i] = false;
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
    splits_[index_ - 1].current += splits_[index_].current;
    splits_[index_].current = 0;
    --index_;
  } else {
    stop();
  }
}

void SplitsScreen::toggle_boss(int boss) {
  killed_[boss] = !killed_[boss];
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
