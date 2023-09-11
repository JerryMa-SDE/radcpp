#include "Memory.h"

namespace rad
{
// Windows CRT debug heap details:
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-debug-heap-details

MemoryDebug::MemoryDebug()
{
#ifdef _WIN32
    _CrtMemCheckpoint(&m_memStateBeg);
#endif
}

MemoryDebug::~MemoryDebug()
{
}

void MemoryDebug::EnableLeakCheckingAtExit()
{
#ifdef _WIN32
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

    int dbgFlag = 0;
    dbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    dbgFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(dbgFlag);
#endif
}

void MemoryDebug::DumpLeaks()
{
#ifdef _WIN32
    _CrtDumpMemoryLeaks();
#endif
}

void MemoryDebug::DumpObjects()
{
#ifdef _WIN32
    _CrtMemDumpAllObjectsSince(&m_memStateBeg);
#endif
}

void MemoryDebug::DumpStatistics()
{
#ifdef _WIN32
    _CrtMemCheckpoint(&m_memStateEnd);
    _CrtMemState memStateDiff = {};
    if (_CrtMemDifference(&memStateDiff, &m_memStateBeg, &m_memStateEnd))
    {
        _CrtMemDumpStatistics(&memStateDiff);
    }
#endif
}

} // namespace rad
