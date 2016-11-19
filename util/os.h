#ifndef Z2SPLITS_UTIL_OS_H
#define Z2SPLITS_UTIL_OS_H
#include <vector>
#include <time.h>

#include "util/string.h"

namespace os {
string GetCWD();
void Yield();
std::string StrFTime(const char* format, struct tm* tm=nullptr);

namespace path {
string Join(const std::vector<string>& components);
}
}

#endif // Z2SPLITS_UTIL_OS_H
