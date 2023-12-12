#pragma once

#include "Global.h"
#include "RefCounted.h"

namespace rad
{

class MemoryDebug : public RefCounted<MemoryDebug>
{
public:
    MemoryDebug();
    ~MemoryDebug();

    static void EnableLeakCheckingAtExit();
    // Windows: _CrtDumpMemoryLeaks can give false indications of memory leaks
    // if a library marks internal allocations as normal blocks instead of CRT blocks or client blocks.
    static void DumpLeaks();

    void DumpObjects();
    void DumpStatistics();

private:
#ifdef _WIN32
    _CrtMemState m_memStateBeg = {};
    _CrtMemState m_memStateEnd = {};
#endif

}; // class MemoryDebug

} // namespace rad
