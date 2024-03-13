#pragma once

#include "rad/Core/RefCounted.h"
#include "DirectMedia/Core/Application.h"
#include "DirectMedia/Core/Window.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

#include "implot/implot.h"

namespace sdl
{

class Window;
class Renderer;

// A simple wrapper for ImGui.
class GuiContext : public rad::RefCounted<GuiContext>
{
public:
    GuiContext(Window* window, Renderer* renderer);
    ~GuiContext();

    ImGuiContext* GetHandle() { return m_context; }

    bool Init();
    void Destroy();

    ImFontAtlas* GetFontAtlas();

    bool ProcessEvent(const SDL_Event& e);
    void NewFrame();
    void Render();

protected:
    Window* m_window;
    Renderer* m_renderer;
    ImGuiContext* m_context = nullptr;
    ImPlotContext* m_plotContext = nullptr;

}; // class GuiContext

} // namespace sdl
