#include "GuiContext.h"
#include "rad/IO/Logging.h"

namespace DirectMedia {

GuiContext::GuiContext(Window* window, Renderer* renderer) :
    m_window(window),
    m_renderer(renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(m_window->GetHandle(), m_renderer->GetHandle());
    ImGui_ImplSDLRenderer2_Init(m_renderer->GetHandle());
}

GuiContext::~GuiContext()
{
    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

bool GuiContext::ProcessEvent(const SDL_Event& event)
{
    return ImGui_ImplSDL2_ProcessEvent(&event);
}

void GuiContext::NewFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void GuiContext::PrepareDrawData()
{
    ImGui::Render();
}

void GuiContext::Render()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

void GuiContext::Present()
{
    m_renderer->Present();
}

void GuiContext::SetScale(float scaleX, float scaleY)
{
    m_renderer->SetScale(scaleX, scaleY);
}

void GuiContext::Clear(float r, float g, float b, float a)
{
    m_renderer->SetDrawColor(
        Uint8(r * 255),
        Uint8(g * 255),
        Uint8(b * 255),
        Uint8(a * 255)
    );
    m_renderer->Clear();
}

void GuiContext::Clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    m_renderer->SetDrawColor(r, g, b, a);
    m_renderer->Clear();
}

} // namespace DirectMedia
