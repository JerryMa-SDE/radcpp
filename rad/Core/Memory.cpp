#include "Memory.h"
#include <cassert>
#include <cstdlib>

namespace rad
{
void* AlignedAlloc(std::size_t size, std::size_t alignment)
{
    assert(size % alignment == 0);
#ifdef _WIN32
    return _aligned_malloc(size, alignment);
#else
    return std::aligned_alloc(alignment, size);
#endif
}

void AlignedFree(void* p)
{
#ifdef _WIN32
    _aligned_free(p);
#else
    std::free(p);
#endif
}

} // namespace rad
