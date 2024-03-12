#include "GuiContext.h"
#include "DirectMedia/Core/Renderer.h"

namespace sdl
{

GuiContext::GuiContext(Window* window, Renderer* renderer) :
    m_window(window),
    m_renderer(renderer)
{
}

GuiContext::~GuiContext()
{
    Destroy();
}

bool GuiContext::Init()
{
    IMGUI_CHECKVERSION();
    m_context = ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    ImGui_ImplSDL2_InitForSDLRenderer(
        m_window->GetHandle(), m_renderer->GetHandle());
    ImGui_ImplSDLRenderer2_Init(m_renderer->GetHandle());

#ifdef _WIN32
    float fontSize = m_window->GetDisplayDPI() / 72.0f * 12.0f;
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", fontSize);
#endif
    return true;
}

void GuiContext::Destroy()
{
    if (m_context)
    {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        m_context = nullptr;
    }
}

ImFontAtlas* GuiContext::GetFontAtlas()
{
    return ImGui::GetIO().Fonts;
}

bool GuiContext::ProcessEvent(const SDL_Event& e)
{
    return ImGui_ImplSDL2_ProcessEvent(&e);
}

void GuiContext::NewFrame()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void GuiContext::Render()
{
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();
    m_renderer->SetScale(io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    m_renderer->SetDrawColor(Uint8(255), Uint8(255), Uint8(255), Uint8(255));
    m_renderer->Clear();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    m_renderer->Present();
}

} // namespace sdl
