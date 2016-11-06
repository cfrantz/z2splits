#include "config.h"

#include "google/protobuf/text_format.h"
#include "util/file.h"
#include "util/logging.h"
#include "util/os.h"

namespace z2splits {
using google::protobuf::TextFormat;

ConfigLoader::ConfigLoader() {}

void ConfigLoader::Load(const std::string& filename) {
    Load(filename, &config_);
}

void ConfigLoader::Load(const std::string& filename, Config* config) {
    std::string pb;
    std::string path = File::Dirname(filename);
    Config local_config;

    if (!File::GetContents(filename, &pb)) {
        LOG(FATAL, "Could not read '", filename, "'.");
    }
    if (!TextFormat::ParseFromString(pb, &local_config)) {
        LOG(FATAL, "Could not parse '", filename, "'.");
    }

    for(const auto& file : local_config.load()) {
        Load(os::path::Join({path, file}), &local_config);
    }
    config->MergeFrom(local_config);
}

}  // namespace z2splits
