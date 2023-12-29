#pragma once

#include "rad/Core/RefCounted.h"
#include "rad/DirectMedia/Core/Window.h"
#include "rad/DirectMedia/Core/Renderer.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"

namespace DirectMedia {

class GuiContext : public rad::RefCounted<GuiContext>
{
public:
    GuiContext(Window* window, Renderer* renderer);
    ~GuiContext();

    ImGuiIO& GetIO() { return ImGui::GetIO(); }

    ImFontAtlas* GetFonts() { return ImGui::GetIO().Fonts; }

    bool ProcessEvent(const SDL_Event& event);
    void NewFrame();
    void PrepareDrawData();
    void Render();
    void Present();

    void SetScale(float scaleX, float scaleY);
    void Clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void Clear(float r, float g, float b, float a);

private:
    Window* m_window;
    Renderer* m_renderer;

}; // class GuiContext

} // namespace DirectMedia
