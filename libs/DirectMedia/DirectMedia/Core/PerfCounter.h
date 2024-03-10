#pragma once

#include "rad/Core/RefCounted.h"
#include "SDL2/SDL_timer.h"

namespace sdl
{

class PerfCounter : rad::RefCounted<PerfCounter>
{
public:
    enum class State
    {
        Reset,
        Stopped,
        Running,
    };

    PerfCounter();
    ~PerfCounter();

    // Returns the current counter value.
    Uint64 GetValue();

    // Get the count per second of the high resolution counter.
    // Returns a platform-specific count per second.
    Uint64 GetFrequency();

    void Start();
    void Stop();
    void Reset();
    void Restart();

    Uint64 GetElapsed();
    double GetElapsedInSeconds();

private:
    State m_state = State::Reset;
    // The count per second of the high resolution counter.
    Uint64 m_frequency = 0;
    Uint64 m_duration = 0;
    Uint64 m_start = 0;

}; // class PerfCounter

} // namespace sdl
