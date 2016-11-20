#include "splits_screen.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <gflags/gflags.h>

#include "google/protobuf/text_format.h"
#include "util/file.h"
#include "util/logging.h"
#include "util/os.h"
#include "util/strutil.h"

// Change this when we're happy with loading/saving.
DEFINE_bool(text_format, true, "Save splits as textpb");

using google::protobuf::TextFormat;
static const SDL_Scancode KEY_NEXT = SDL_SCANCODE_SPACE;
static const SDL_Scancode KEY_ERASE = SDL_SCANCODE_ESCAPE;

void SplitsScreen::init() {
  index_ = time_ = 0;
  running_ = false;

  text_.reset(new Text("text.png"));
  maps_.reset(new SpriteMap("maps.png", 1, 256, 64));
  fairy_.reset(new SpriteMap("fairy.png", 2, 8, 16));
  triforce_.reset(new SpriteMap("triforce.png", 3, 8, 16));

  visible_ = 15;
  delta_ = true;
  reset();
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
  delay_ = ccfg.start_delay();
  time_ = -delay_;

  load_resources(gcfg);
  *run_.mutable_splits() = ccfg.splits();

  std::string saved_data;
  saved_runs_.Clear();
  if (ccfg.save_filename().empty()) {
      std::string fn;
      for(auto ch : title_) {
          fn.append(1, isalnum(ch) ? ch : '_');
      }
      config_.mutable_game(game_)->
          mutable_category(category_)->set_save_filename(fn);
  }


  if (File::GetContents(ccfg.save_filename(), &saved_data)) {
    if (saved_runs_.ParseFromString(saved_data)) {
        number_ = saved_runs_.run(saved_runs_.run_size() - 1).number();
    } else {
      LOG(INFO, "Couldn't parse binary protobuf.  Trying TextFormat");
      if (TextFormat::ParseFromString(saved_data, &saved_runs_)) {
        number_ = saved_runs_.run(saved_runs_.run_size() - 1).number();
      } else {
        LOG(WARN, "Couldn't parse text protobuf either.");
      }
    }
  } else {
    LOG(WARN, "Couldn't load saved splits from '", ccfg.save_filename(), "'");
  }

  view_number_ = number_;
  return true;
}

void SplitsScreen::save_splits() {

  *saved_runs_.add_run() = run_;

  int n = run_.splits_size();
  int total = run_.splits(n-1).time_ms();

  auto* best = saved_runs_.mutable_best();
  int bestn = best->splits_size();
  if (bestn == 0) {
      *best = run_;
  } else if (total < best->splits(bestn - 1).time_ms()) {
      *best = run_;
  }

  if (saved_runs_.gold().splits_size() == 0) {
    *saved_runs_.mutable_gold() = run_;
  } else {
    for(auto& gold : *saved_runs_.mutable_gold()->mutable_splits()) {
      for(const auto& split : run_.splits()) {
        if (gold.name() == split.name()) {
          if (split.time_ms() < gold.time_ms()) {
            gold = split;
          }
        }
      }
    }
  }

  const auto& fn = config_.game(game_).category(category_).save_filename();
  std::string data;
  if (FLAGS_text_format) {
    TextFormat::PrintToString(saved_runs_, &data);
  } else {
    saved_runs_.SerializeToString(&data);
  }
  if (!File::SetContents(fn, data)) {
    LOG(ERROR, "Couldn't load save splits to '", fn, "'");
  }
}

bool SplitsScreen::update(const Input& input, Audio&, unsigned int elapsed) {
  z2splits::ControlAction action;
  if (running_) {
    time_ += elapsed;
    auto* split = run_.mutable_splits(index_);
    split->set_time_ms(time_);
  }

  // FIXME: this is dumb.  Rewrite to use a map.
  for(const auto& c : config_.controls()) {
    action = input.key_pressed(SDL_Scancode(c.scancode())) ?
             c.action() : z2splits::ControlAction::UNKNOWN;
    if (running_) {
      if (action == z2splits::ControlAction::SPLIT) next();
      if (action == z2splits::ControlAction::BACK) back();
      if (action == z2splits::ControlAction::STOP) stop();
      if (action == z2splits::ControlAction::SKIP) skip();
    } else {
      if (action == z2splits::ControlAction::SPLIT) go();
      if (action == z2splits::ControlAction::VIEW_DOWN) view(-1);
      if (action == z2splits::ControlAction::VIEW_UP) view(1);
    }
    if (action == z2splits::ControlAction::RESET) reset();
    if (action == z2splits::ControlAction::UP) scroll_up();
    if (action == z2splits::ControlAction::DOWN) scroll_down();
    if (action == z2splits::ControlAction::TOGGLE_DELTA) delta_ = !delta_;
  }
  return true;
}

const z2splits::Split* SplitsScreen::find_same_split(const std::string& name,
                                             const z2splits::Run& run) const {
  for(const auto& s : run.splits()) {
    if (name == s.name())
      return &s;
  }
  return nullptr;
}

bool SplitsScreen::is_gold_split(const z2splits::Split& split) const {
  const auto* gold = find_same_split(split.name(), saved_runs_.gold());
  if (gold && gold->time_ms() > 0) {
    if (split.time_ms() < gold->time_ms()) {
      return true;
    }
  }
  return false;
}

void SplitsScreen::draw(Graphics& graphics) const {
  const int right = graphics.width() - 16;
  std::string title = StrCat(title_, " #", run_.number());
  text_->draw(
      graphics, title, graphics.width() / 2, 8, Text::Alignment::CENTER);

  // split name, current, gold, p.b.
  text_->draw(graphics, "Split", 24, 32);
  text_->draw(graphics, "Cur", right-184, 32);
  text_->draw(graphics, "Gold",right-112, 32);
  text_->draw(graphics, "P.B.",right-32, 32);

  int last_time = 0, last_best = 0, last_gold = 0;
  int tm;
  for (int n = 0; n < run_.splits_size(); ++n) {
    int i = n - offset_;
    if (n > 0)
        last_time = run_.splits(n-1).time_ms();
    if (i < 0 || i >= visible_)
        continue;

    const z2splits::Split& s = run_.splits(n);
    const int y = 16 * (n - offset_) + 60;

    if (n == index_) fairy_->draw(graphics, unsigned(time_ / 64) %2, 16, y);
    text_->draw(graphics, s.name(), 24, y);

    const auto* best = find_same_split(s.name(), saved_runs_.best());
    if (best) {
      tm = best->time_ms();
      if (tm > 0)
          draw_time(graphics, delta_ ? tm - last_best: tm, right, y);
      last_best = tm;
    }

    const auto* gold = find_same_split(s.name(), saved_runs_.gold());
    if (gold) {
      tm = gold->time_ms();
      if (tm > 0)
          draw_time(graphics, delta_ ? tm - last_gold: tm, right - 80, y);
      last_gold = tm;
    }

    if (n <= index_) {
      tm = s.time_ms();
      if (tm == 0) {
        text_->draw(graphics, "-", right - 160, y, Text::Alignment::RIGHT);
      } else {
        draw_time(graphics, delta_ ? tm - last_time: tm, right - 160, y);
        if (n < index_ && is_gold_split(s)) {
            triforce_->draw(graphics, unsigned(time_ / 64) % 3, right-160, y);
        }
      }
    }
  }

  if (index_ < run_.splits_size()) {
    const z2splits::Split& s = run_.splits(index_);
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

  draw_vline(graphics, 2, 32, 16);
  draw_vline(graphics, graphics.width() - 6, 32, 16);

  draw_corner(graphics, 1, 50);
  draw_corner(graphics, graphics.width() - 7, 50);
  draw_hline(graphics, 8, 51, graphics.width() - 17);

  draw_corner(graphics, 1, graphics.height() - 7);
  draw_corner(graphics, graphics.width() - 7, graphics.height() - 7);
  draw_hline(graphics, 8, graphics.height() - 6, graphics.width() - 17);

  draw_vline(graphics, 2, 8, 15);
  draw_vline(graphics, graphics.width() - 6, 8, 15);

  draw_vline(graphics, 2, 57, graphics.height() - 66);
  draw_vline(graphics, graphics.width() - 6, 57, graphics.height() - 66);
}

void SplitsScreen::stop() {
  running_ = false;
  save_splits();
}

void SplitsScreen::reset() {
  running_ = false;
  offset_ = index_ = 0;
  number_++;
  for (int i = 0; i < run_.splits_size(); ++i) {
    run_.mutable_splits(i)->set_time_ms(0);
  }
  time_ = -delay_;
  run_.mutable_splits(0)->set_time_ms(time_);
}

void SplitsScreen::go() {
  if (index_ < run_.splits_size()) running_ = true;
  view_number_ = number_;
  run_.set_number(number_);
  run_.set_datetime(os::StrFTime("%Y-%m-%d %H:%M:%S %Z"));
}

void SplitsScreen::next() {
  ++index_;
  scroll_offset();

  if (index_ >= run_.splits_size()) stop();
}

void SplitsScreen::next(const std::string& nickname) {
  for(int i=0; i<run_.splits_size(); i++) {
    if (run_.splits(i).nickname() == nickname) {
        // FIXME: what to do if player goes out of order?
        index_ = i;
        next();
        break;
    }
  }
}

void SplitsScreen::skip() {
  if (index_ < run_.splits_size()) {
    run_.mutable_splits(index_)->set_time_ms(0);
    ++index_;
    scroll_offset();
  }
}

void SplitsScreen::back() {
  if (index_ > 0) {
    run_.mutable_splits(index_)->set_time_ms(0);;
    --index_;
    scroll_offset();
  } else {
    stop();
  }
}

void SplitsScreen::view(int dir) {
    view_number_ = unsigned(view_number_ + dir) % saved_runs_.run_size();
    run_ = saved_runs_.run(view_number_);
}

void SplitsScreen::scroll_offset() {
  if (offset_ + visible_ <= index_) offset_ = index_ - visible_ + 1;
  if (offset_ > index_) offset_ = index_;
}

void SplitsScreen::scroll_up() {
  if (offset_ > 0) --offset_;
}

void SplitsScreen::scroll_down() {
  if (offset_ + visible_ < run_.splits_size()) ++offset_;
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
