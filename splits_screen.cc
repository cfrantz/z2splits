#include "splits_screen.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

static const SDL_Scancode KEY_START = SDL_SCANCODE_RETURN;
static const SDL_Scancode KEY_QUIT = SDL_SCANCODE_ESCAPE;
static const SDL_Scancode KEY_NEXT = SDL_SCANCODE_SPACE;
static const SDL_Scancode KEY_ERASE = SDL_SCANCODE_BACKSPACE;
static const SDL_Scancode KEY_SAVE = SDL_SCANCODE_F2;

void SplitsScreen::init() {
  index_ = time_ = 0;
  running_ = false;

  text_.reset(new Text("text"));
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

  mode_ = title_.find("Rando") == std::string::npos ?
    ComparisonMode::BEST : ComparisonMode::AVERAGE;

  return true;
}

bool SplitsScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  if (running_) {
    splits_[index_].current += elapsed;
    time_ += elapsed;

    if (input.key_pressed(KEY_START)) stop();
    if (input.key_pressed(KEY_QUIT)) reset();
    if (input.key_pressed(KEY_NEXT)) next();
    if (input.key_pressed(KEY_ERASE)) back();
  } else {
    if (input.key_pressed(KEY_START)) go();
    if (input.key_pressed(KEY_ERASE)) reset();
    if (input.key_pressed(KEY_SAVE)) save();
    if (input.key_pressed(KEY_QUIT)) return false;
  }

  return true;
}

void SplitsScreen::draw(Graphics& graphics) const {
  text_->draw(
      graphics, title_, graphics.width() / 2, 8, Text::Alignment::CENTER);

  const int right = graphics.width() - 16;
  const int bottom = graphics.height() - 32;

  for (size_t i = 0; i < splits_.size(); ++i) {
    const Split s = splits_[i];
    const int y = 16 * i + 40;

    text_->draw(graphics, s.name, 16, y);

    if (i <= index_) {
      draw_time(graphics, s.current, right, y);

      if (s.count > 0) {
        int compare = 0;

        switch (mode_) {
          case ComparisonMode::AVERAGE:
            compare = s.current - s.average;
            break;

          case ComparisonMode::BEST:
            compare = s.current - s.best;
            break;
        }

        draw_time(graphics, compare, right - 80, y);
      }
    }
  }

  draw_time(graphics, time_, right, bottom);

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
}

SplitsScreen::Split::Split(const std::string& name) :
  name(name), count(0), best(0), average(0), current(0) {}

SplitsScreen::Split::Split(
    const std::string& name, int count, int best, int average) :
  name(name), count(count), best(best), average(average), current(0) {}

void SplitsScreen::stop() {
  running_ = false;
}

void SplitsScreen::reset() {
  running_ = false;
  index_ = time_ = 0;
  for (size_t i = 0; i < splits_.size(); ++i) splits_[i].current = 0;
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
  }
}

void SplitsScreen::save() {
  for (size_t i = 0; i < splits_.size(); ++i) {
    Split* s = &splits_[i];
    const int t = s->count * s->average + s->current;

    s->count++;
    s->average = t / s->count;
    s->best = s->current;

    // if (s->current < s->best || s->best == 0) s->best = s->current;
  }

  reset();
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

  if (minutes > 0) {
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
