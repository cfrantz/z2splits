#ifndef Z2SPLITS_CONFIG_H
#define Z2SPLITS_CONFIG_H
#include <string>
#include "proto/config.pb.h"

namespace z2splits {

class ConfigLoader {
  public:
    ConfigLoader();
    void Load(const std::string& filename);
    void Load(const std::string& filename, Config* config);

    inline const Config& config() { return config_; }
  private:
    Config config_;
};

}  // namespace z2splits
#endif // Z2SPLITS_CONFIG_H
