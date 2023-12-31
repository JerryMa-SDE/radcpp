#pragma once

#include "rad/DirectMedia/Core.h"
#include "rad/DirectMedia/Gui/GuiContext.h"

class Painter : public DirectMedia::Window
{
public:
    Painter();
    ~Painter();

    bool Init();

    virtual bool OnEvent(const SDL_Event& event);

    virtual void OnShown() override;
    virtual void OnHidden() override;
    virtual void OnExposed() override;
    virtual void OnMoved(int x, int y) override;
    virtual void OnResized(int width, int height) override;
    virtual void OnMinimized() override;
    virtual void OnMaximized() override;
    virtual void OnRestored() override;
    virtual void OnEnter() override;
    virtual void OnLeave() override;
    virtual void OnKeyDown(const SDL_KeyboardEvent& keyDown) override;
    virtual void OnKeyUp(const SDL_KeyboardEvent& keyUp) override;
    virtual void OnMouseMove(const SDL_MouseMotionEvent& mouseMotion) override;
    virtual void OnMouseButtonDown(const SDL_MouseButtonEvent& mouseButton) override;
    virtual void OnMouseButtonUp(const SDL_MouseButtonEvent& mouseButton) override;
    virtual void OnMouseWheel(const SDL_MouseWheelEvent& mouseWheel) override;

    virtual void OnRender() override;

private:
    const char* GetMouseButtonName(Uint8 button);

    rad::Ref<DirectMedia::Renderer> m_renderer;
    rad::Ref<DirectMedia::GuiContext> m_guiContext;

    ImVec4 m_clearColor = ImVec4(1, 1, 1, 1);

}; // class Painter
