#ifndef Z2SPLITS_UTIL_OS_H
#define Z2SPLITS_UTIL_OS_H
#include <vector>

#include "util/string.h"

namespace os {
string GetCWD();
void Yield();

namespace path {
string Join(const std::vector<string>& components);
}
}

#endif // Z2SPLITS_UTIL_OS_H
