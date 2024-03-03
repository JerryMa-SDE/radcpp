#pragma once

#include "Global.h"
#include <memory>

namespace rad
{
void* AlignedAlloc(std::size_t size, std::size_t alignment);
void AlignedFree(void* p);

} // namespace rad
