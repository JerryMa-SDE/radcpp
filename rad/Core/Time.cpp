#include "Time.h"

namespace rad {

const char* GetTimeString(const char* format)
{
    thread_local char buffer[64];
    const time_t t = time(nullptr);
    std::tm timeInfo = {};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&timeInfo, &t);
#else
    localtime_r(&t, &timeInfo);
#endif
    strftime(buffer, sizeof(buffer), format, &timeInfo);
    return buffer;
}

} // namespace rad
