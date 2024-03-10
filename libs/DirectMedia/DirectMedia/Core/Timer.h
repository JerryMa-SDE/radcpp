#pragma once

#include "rad/Core/RefCounted.h"
#include "SDL2/SDL.h"

namespace sdl
{

class Timer : public rad::RefCounted<Timer>
{
public:
    Timer();
    ~Timer();

    // @param interval in milliseconds, passed to callback.
    // @param callback will be passed the current timer interval and returns the next timer interval.
    //          If the returned value is the same as the one passed in, the periodic alarm continues,
    //          otherwise a new alarm is scheduled.
    //          If the callback returns 0, the periodic alarm is cancelled.
    // @param param a pointer that is passed to callback.
    // @note Timing may be inexact due to OS scheduling. Be sure to note the current time with
    // SDL_GetTicks() or SDL_GetPerformanceCounter() in case your callback
    // needs to adjust for variances.
    SDL_TimerID Start(Uint32 interval, SDL_TimerCallback callback, void* param);
    void Stop();

    SDL_TimerID GetID() const { return m_id; }

private:
    SDL_TimerID m_id = 0;

}; // class Timer

} // namespace sdl
