#include "PerfCounter.h"

namespace DirectMedia {

PerfCounter::PerfCounter()
{
    m_frequency = SDL_GetPerformanceFrequency();
}

PerfCounter::~PerfCounter()
{
}

Uint64 PerfCounter::GetValue()
{
    return SDL_GetPerformanceCounter();
}

Uint64 PerfCounter::GetFrequency()
{
    return m_frequency;
}

void PerfCounter::Start()
{
    assert(m_state == State::Stopped);
    m_start = GetValue();
    m_state = State::Running;
}

void PerfCounter::Stop()
{
    assert(m_state == State::Running);
    m_duration += GetValue() - m_start;
}

void PerfCounter::Reset()
{
    m_duration = 0;
    m_state = State::Stopped;
}

void PerfCounter::Restart()
{
    Reset();
    Start();
}

Uint64 PerfCounter::GetElapsed()
{
    return m_duration;
}

double PerfCounter::GetElapsedInSeconds()
{
    return double(m_duration) / double(m_frequency);
}

} // namespace DirectMedia
