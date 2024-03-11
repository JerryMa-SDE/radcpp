#pragma once

#include "Global.h"
#include <ctime>

namespace rad
{

struct tm* LocalTime(const time_t* timer, struct tm* buffer);

} // namespace rad
