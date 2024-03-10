#include "Timer.h"
#include "rad/IO/Logging.h"

namespace sdl
{

Timer::Timer()
{
}

Timer::~Timer()
{
    if (m_id != 0)
    {
        Stop();
    }
}

SDL_TimerID Timer::Start(Uint32 interval, SDL_TimerCallback callback, void* param)
{
    m_id = SDL_AddTimer(interval, callback, param);
    if (m_id == 0)
    {
        LogGlobal(Error, "SDL_AddTimer failed: {}", SDL_GetError());
    }
    return m_id;
}

void Timer::Stop()
{
    assert(m_id != 0);
    SDL_RemoveTimer(m_id);
    m_id = 0;
}

} // namespace sdl
