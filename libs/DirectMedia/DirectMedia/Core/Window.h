#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/String.h"
#include "EventHandler.h"

namespace sdl
{

class Window : public EventHandler
{
    friend class Application;
public:
    Window();
    virtual ~Window();

    bool Create(const char* title, int x, int y, int width, int height, Uint32 flags);
    void Destroy();

    Uint32 GetID() const { return m_id; }

    void SetShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }
    bool ShouldClose() const { return m_shouldClose; }

    SDL_Window* GetHandle() const { return m_handle; }
    void GetPosition(int* x, int* y);
    void GetSize(int* w, int* h);
    void GetMaximumSize(int* w, int* h);
    void GetMinimumSize(int* w, int* h);
    void GetBorderSize(int* top, int* left, int* bottom, int* right);
    // Get the brightness (gamma multiplier) for a given window's display,
    // where 0.0 is completely dark and 1.0 is normal brightness.
    float GetBrightness();
    // Get the index of the display associated with a window.
    int GetDisplayIndex();
    int GetDisplayMode(SDL_DisplayMode* displayMode);
    float GetDisplayDPI();
    int GetFlags();
    bool IsMinimized();
    bool IsMaximized();
    int GetGammaRamp(Uint16* r, Uint16* g, Uint16* b);
    bool IsGrabbed();
    float GetOpacity();
    // SDL_PIXELFORMAT
    Uint32 GetPixelFormat();
    const char* GetTitle();

    void Resize(int w, int h);
    void Hide();
    void Show();
    void Maximize();
    void Minimize();
    void Raise();
    void Restore();
    void SetBordered(bool bordered = true);
    int SetBrightness(float brightness);
    int SetDisplayMode(const SDL_DisplayMode& mode);
    int SetGammaRamp(const Uint16* r, const Uint16* g, const Uint16* b);
    void Grab(bool grabbed);
    void SetIcon(SDL_Surface* icon);
    void SetInputFocus();
    void SetMaximumSize(int w, int h);
    void SetMinimumSize(int w, int h);
    void SetTitle(std::string_view title);

protected:
    virtual bool OnEvent(const SDL_Event& event);
    virtual void OnWindowEvent(const SDL_WindowEvent& windowEvent);
    virtual void OnShown() {}
    virtual void OnHidden() {}
    virtual void OnExposed() {}
    virtual void OnMoved(int x, int y) {}
    virtual void OnResized(int width, int height) {}
    virtual void OnMinimized() {}
    virtual void OnMaximized() {}
    virtual void OnRestored() {}
    virtual void OnEnter() {}
    virtual void OnLeave() {}
    virtual void OnFocusGained() {}
    virtual void OnFocusLost() {}
    virtual void OnClose();

    // Key
    virtual void OnKeyDown(const SDL_KeyboardEvent& keyDown) {}
    virtual void OnKeyUp(const SDL_KeyboardEvent& keyUp) {}
    virtual void OnTextEditing(const SDL_TextEditingEvent& textEditing) {}
    virtual void OnTextInput(const SDL_TextInputEvent& textInput) {}

    // Mouse
    virtual void OnMouseMove(const SDL_MouseMotionEvent& mouseMotion) {}
    virtual void OnMouseButtonDown(const SDL_MouseButtonEvent& mouseButton) {}
    virtual void OnMouseButtonUp(const SDL_MouseButtonEvent& mouseButton) {}
    virtual void OnMouseWheel(const SDL_MouseWheelEvent& mouseWheel) {}

    virtual void OnRender() {}
    virtual void OnIdle() { OnRender(); }

    SDL_Window* m_handle = nullptr;
    Uint32 m_id = 0;
    bool m_shouldClose = false;

}; // class Window

} // namespace sdl
