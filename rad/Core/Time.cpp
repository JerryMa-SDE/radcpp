#include "Time.h"
#include <cassert>

namespace rad
{

tm* LocalTime(const time_t* timer, tm* buffer)
{
    assert((timer != nullptr) && (buffer != nullptr));
#ifdef _WIN32
    errno_t err = localtime_s(buffer, timer);
    if (err == 0)
    {
        return buffer;
    }
    else
    {
        return nullptr;
    }
#else
    return localtime_r(timer, buffer);
#endif
}

} // namespace rad
