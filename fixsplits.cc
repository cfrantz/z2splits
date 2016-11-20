#include <string>
#include <map>

#include <gflags/gflags.h>
#include "util/file.h"
#include "util/logging.h"
#include "proto/config.pb.h"
#include "google/protobuf/text_format.h"

DEFINE_string(out, "", "Output filename");

using google::protobuf::TextFormat;
namespace z2splits {
int number = 0;
std::map <std::string, int> gold;

void MergeSplits(SavedRuns* all, SavedRuns* x) {
    x->mutable_best()->Clear();
    x->mutable_gold()->Clear();
    for(auto& run : *x->mutable_run()) {
        int total = 0;
        bool completed = false;
        run.set_number(++number);
        for(auto& s : *run.mutable_splits()) {
            completed = true;;
            total += s.time_ms();
            if (s.time_ms() == 0) {
                completed = false;
                continue;
            }
            s.set_time_ms(total);
            int g = gold[s.name()];
            if (g==0 || total < g) {
                LOG(INFO, "Gold: ", s.name(), ": ", total);
                gold[s.name()] = total;
            }
        }
        if (!completed)
            continue;
        int b = all->best().splits_size();
        if (b) {
            if (total < all->best().splits(b-1).time_ms()) {
                *all->mutable_best() = run;
            }
        } else {
            *all->mutable_best() = run;
        }
    }
    all->MergeFrom(*x);
}

}  // namespace

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  z2splits::SavedRuns all, x;

  for(int i=1; i<argc; i++) {
      std::string content;
      x.Clear();
      if (!File::GetContents(argv[i], &content)) {
          LOG(FATAL, "Couldn't read ", argv[i]);
      }
      if (!TextFormat::ParseFromString(content, &x)) {
          LOG(FATAL, "Could not parse '", argv[i], "'.");
      }
      LOG(INFO, "Merging ", argv[i]);
      MergeSplits(&all, &x);
  }

  for(auto& s : all.best().splits()) {
      auto* g = all.mutable_gold();
      auto* gs = g->add_splits();
      *gs = s;
      gs->set_time_ms(z2splits::gold[s.name()]);
  }

  auto s = all.DebugString();
  if (FLAGS_out.empty()) {
      LOG(INFO, "Splits: ", s);
  } else {
      File::SetContents(FLAGS_out, s);
  }
  return 0;
}
