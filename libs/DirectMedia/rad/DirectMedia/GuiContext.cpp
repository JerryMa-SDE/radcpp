#include "GuiContext.h"
#include "rad/IO/Logging.h"

namespace DirectMedia
{

GuiContext::GuiContext(rad::Ref<Window> window, rad::Ref<Renderer> renderer) :
    m_window(std::move(window)),
    m_renderer(std::move(renderer))
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

void GuiContext::StartFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void GuiContext::Render()
{
    ImGui::Render();
}

void GuiContext::RenderDrawData()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

} // namespace DirectMedia
