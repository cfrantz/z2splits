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
  maps_.reset(new SpriteMap("maps.png", 1, 256, 64));
}

bool SplitsScreen::load_splits(const std::string& file) {
  file_ = file;

  title_ = "Zelda 2 All Keys";

  splits_.emplace_back("Magic", 0);
  splits_.emplace_back("Rauru", 0);
  splits_.emplace_back("Desert Heart", 0);
  splits_.emplace_back("Palace 1", 1);
  splits_.emplace_back("Trophy", 0);
  splits_.emplace_back("Ruto", 0);
  splits_.emplace_back("Swamp 1 Up", 0);
  splits_.emplace_back("Bagu", 0);
  splits_.emplace_back("Saria", 0);
  splits_.emplace_back("Hammer", 0);
  splits_.emplace_back("Rock Magic", 0);
  splits_.emplace_back("Desert 1 Up", 0);
  splits_.emplace_back("Cave Heart", 0);
  splits_.emplace_back("Medicine", 0);
  splits_.emplace_back("Mido", 0);
  splits_.emplace_back("Palace 2", 2);
  splits_.emplace_back("Palace 3", 3);
  splits_.emplace_back("Nabooru Fire", 0);
  splits_.emplace_back("Desert 1 Up", 0);
  splits_.emplace_back("Pit Magic", 0);
  splits_.emplace_back("Boots", 0);
  splits_.emplace_back("Child", 0);
  splits_.emplace_back("Darunia", 0);
  splits_.emplace_back("Palace 4", 4);
  splits_.emplace_back("Ocean Heart", 0);
  splits_.emplace_back("Palace 5", 5);
  splits_.emplace_back("Swamp 1 Up", 0);
  splits_.emplace_back("New Kasuto", 0);
  splits_.emplace_back("Desert Heart", 0);
  splits_.emplace_back("Palace 6", 6);
  splits_.emplace_back("Kasuto", 0);
  splits_.emplace_back("Palace 7", 0);

  return true;
}

bool SplitsScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  if (running_) {
    splits_[index_].current += elapsed;
    time_ += elapsed;

    if (input.key_pressed(SDL_SCANCODE_SPACE)) next();
    if (input.key_pressed(SDL_SCANCODE_BACKSPACE)) back();
    if (input.key_pressed(SDL_SCANCODE_RETURN)) stop();

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
  int offset = 0;
  const int max_shown = 16;

  if (splits_.size() > max_shown) {
    offset = index_ - max_shown + 1;
    if (offset < 0) offset = 0;
  }

  for (size_t i = 0; i < splits_.size(); ++i) {
    const Split s = splits_[i];
    const int y = 16 * (i - offset) + 40;

    total += s.current;

    if (i >= offset && i < offset + max_shown) {
      text_->draw(graphics, s.name, 16, y);
      if (i <= index_) draw_time(graphics, total, right, y);
    }
  }

  maps_->draw(graphics, splits_[index_].hint, 0, graphics.height() - 72);

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

SplitsScreen::Split::Split(const std::string& name, int hint=-1) :
  name(name), current(0), hint(hint) {}

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
