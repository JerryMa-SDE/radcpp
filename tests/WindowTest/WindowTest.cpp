#include "WindowTest.h"
#include "rad/IO/Logging.h"

WindowTest::WindowTest()
{
    LogGlobal(Info, "WindowTest::WindowTest()");
}

WindowTest::~WindowTest()
{
    LogGlobal(Info, "WindowTest::~WindowTest()");
}

bool WindowTest::Init()
{
    float windowScale = sdl::GetApp()->GetDisplayDPI(0) / 96.0f;
    SDL_WindowFlags windowFlags = (SDL_WindowFlags)
        (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    Create("WindowTest",
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), SDL_WINDOWPOS_CENTERED_DISPLAY(0),
        int(800 * windowScale), int(600 * windowScale),
        windowFlags);

    m_renderer = new sdl::Renderer(this);
    if (!m_renderer->Init())
    {
        LogGlobal(Error, "WindowTest::Init: failed to create renderer!");
        return false;
    }
    return true;
}

bool WindowTest::OnEvent(const SDL_Event& event)
{
    return Window::OnEvent(event);
}

void WindowTest::OnShown()
{
    LogGlobal(Info, "OnShown");
}

void WindowTest::OnHidden()
{
    LogGlobal(Info, "OnHidden");
}

void WindowTest::OnExposed()
{
    LogGlobal(Info, "OnExposed");
}

void WindowTest::OnMoved(int x, int y)
{
    LogGlobal(Info, "OnMoved: {:4}, {:4}", x, y);
}

void WindowTest::OnResized(int width, int height)
{
    LogGlobal(Info, "OnResized: {:4}, {:4}", width, height);
}

void WindowTest::OnMinimized()
{
    LogGlobal(Info, "OnMinimized");
}

void WindowTest::OnMaximized()
{
    LogGlobal(Info, "OnMaximized");
}

void WindowTest::OnRestored()
{
    LogGlobal(Info, "OnRestored");
}

void WindowTest::OnEnter()
{
    LogGlobal(Info, "OnEnter");
}

void WindowTest::OnLeave()
{
    LogGlobal(Info, "OnLeave");
}

void WindowTest::OnKeyDown(const SDL_KeyboardEvent& keyDown)
{
    LogGlobal(Info, "OnKeyDown: {}", SDL_GetKeyName(keyDown.keysym.sym));
}

void WindowTest::OnKeyUp(const SDL_KeyboardEvent& keyUp)
{
    LogGlobal(Info, "OnKeyUp: {}", SDL_GetKeyName(keyUp.keysym.sym));
}

void WindowTest::OnMouseMove(const SDL_MouseMotionEvent& mouseMotion)
{
    LogGlobal(Info, "OnMouseMove: {:4}, {:4}", mouseMotion.x, mouseMotion.y);
}

void WindowTest::OnMouseButtonDown(const SDL_MouseButtonEvent& mouseButton)
{
    LogGlobal(Info, "OnMouseButtonDown: {}", GetMouseButtonName(mouseButton.button));
}

void WindowTest::OnMouseButtonUp(const SDL_MouseButtonEvent& mouseButton)
{
    LogGlobal(Info, "OnMouseButtonUp: {}", GetMouseButtonName(mouseButton.button));
}

void WindowTest::OnMouseWheel(const SDL_MouseWheelEvent& mouseWheel)
{
    LogGlobal(Info, "OnMouseWheel: {:+}", mouseWheel.y);
}

void WindowTest::OnRender()
{
}

const char* WindowTest::GetMouseButtonName(Uint8 button)
{
    switch (button)
    {
    case SDL_BUTTON_LEFT: return "SDL_BUTTON_LEFT";
    case SDL_BUTTON_MIDDLE: return "SDL_BUTTON_MIDDLE";
    case SDL_BUTTON_RIGHT: return "SDL_BUTTON_RIGHT";
    case SDL_BUTTON_X1: return "SDL_BUTTON_X1";
    case SDL_BUTTON_X2: return "SDL_BUTTON_X2";
    }
    return "SDL_BUTTON_UNKNOWN";
}
