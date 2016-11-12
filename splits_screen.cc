#include "splits_screen.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

void SplitsScreen::init() {
  text_.reset(new Text("text.png"));
  maps_.reset(new SpriteMap("maps.png", 1, 256, 64));
  fairy_.reset(new SpriteMap("fairy.png", 2, 8, 16));
  triforce_.reset(new SpriteMap("triforce.png", 3, 8, 16));

  visible_ = 15;

  reset();
}

bool SplitsScreen::load(const std::string& file) {
  file_ = file;
  splits_.clear();

  std::ifstream in(file);

  in >> delay_;
  std::getline(in, title_);

  std::string name;
  unsigned int hint, best;

  while (true) {
    if (in >> hint >> best) {
      std::getline(in, name);
      splits_.emplace_back(name, hint, best);
    } else {
      break;
    }
  }

  reset();

  return true;
}

void SplitsScreen::save() {
  std::ofstream out(file_);

  if (out.is_open()) {
    out << delay_ << title_ << "\n";
    for (size_t i = 0; i < splits_.size(); ++i) {
      Split s = splits_[i];
      if (s.current < s.best || s.best == 0) s.best = s.current;
      out << s.hint << " " << s.best << s.name << "\n";
    }

    out.close();
  }
}

bool SplitsScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  if (running_) {
    time_ += elapsed;
    splits_[index_].current = time_;

    if (input.key_pressed(SDL_SCANCODE_SPACE)) next();
    if (input.key_pressed(SDL_SCANCODE_BACKSPACE)) back();
    if (input.key_pressed(SDL_SCANCODE_RETURN)) stop();
    if (input.key_pressed(SDL_SCANCODE_TAB)) skip();

  } else {
    if (input.key_pressed(SDL_SCANCODE_SPACE)) go();
  }

  if (input.key_pressed(SDL_SCANCODE_ESCAPE)) reset();

  if (input.key_pressed(SDL_SCANCODE_UP)) scroll_up();
  if (input.key_pressed(SDL_SCANCODE_DOWN)) scroll_down();

  return true;
}

void SplitsScreen::draw(Graphics& graphics) const {
  text_->draw(graphics, title_, graphics.width() / 2, 8, Text::Alignment::CENTER);

  const int right = graphics.width() - 16;

  for (size_t i = 0; i < visible_; ++i) {
    if (i + offset_ >= splits_.size()) break;

    const Split s = splits_[i + offset_];
    const int y = 16 * i + 40;

    if (i + offset_ == index_) fairy_->draw(graphics, (time_ / 64) % 2, 16, y);

    text_->draw(graphics, s.name, 24, y);

    if (s.best > 0) {
      draw_time(graphics, s.best, right, y);

      if (i + offset_ <= index_) {
        draw_time(graphics, s.current - s.best, right - 80, y);
        if (is_gold_split(i + offset_)) triforce_->draw(graphics, (time_ / 64) % 3, right - 80, y);
      }

    } else if (i + offset_ <= index_) {
      if (s.current > 0) {
        draw_time(graphics, s.current, right, y);
      } else {
        text_->draw(graphics, "-", right, y, Text::Alignment::RIGHT);
      }
    }
  }

  maps_->draw(graphics, splits_[index_].hint, 8, graphics.height() - 72);

  draw_time(graphics, time_, right, graphics.height() - 96);

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

SplitsScreen::Split::Split(const std::string& name, int hint, unsigned int best) :
  name(name), current(0), best(best), hint(hint) {}

bool SplitsScreen::is_gold_split(int split) const {
  if (split >= index_) return false;
  if (split == 0) return splits_[0].current < splits_[0].best;

  const int current = splits_[split].current - splits_[split - 1].current;
  const int best = splits_[split].best - splits_[split - 1].best;

  return current < best;
}

void SplitsScreen::stop() {
  running_ = false;
}

void SplitsScreen::reset() {
  running_ = false;
  offset_ = index_ = 0;
  for (size_t i = 0; i < splits_.size(); ++i) splits_[i].current = 0;
  time_ = splits_[0].current = -delay_;
}

void SplitsScreen::go() {
  if (index_ < splits_.size()) running_ = true;
}

void SplitsScreen::next() {
  ++index_;
  scroll_offset();

  if (index_ >= splits_.size()) {
    save();
    stop();
  }
}

void SplitsScreen::skip() {
  if (index_ < splits_.size()) {
    splits_[index_].current = 0;
    ++index_;
    scroll_offset();
  }
}

void SplitsScreen::back() {
  if (index_ > 0) {
    splits_[index_].current = 0;
    --index_;
    scroll_offset();
  } else {
    stop();
  }
}

void SplitsScreen::scroll_offset() {
  if (offset_ + visible_ <= index_) offset_ = index_ - visible_ + 1;
  if (offset_ > index_) offset_ = index_;
}

void SplitsScreen::scroll_up() {
  if (offset_ > 0) --offset_;
}

void SplitsScreen::scroll_down() {
  if (offset_ + visible_ < splits_.size()) ++offset_;
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
