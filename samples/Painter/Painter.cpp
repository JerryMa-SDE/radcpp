#include "Painter.h"
#include "rad/IO/Logging.h"
#include "rad/DirectMedia/Media/Codec.h"

Painter::Painter()
{
    LogGlobal(Info, "Painter::Painter()");
}

Painter::~Painter()
{
    LogGlobal(Info, "Painter::~Painter()");
}

bool Painter::Init()
{
    float windowScale = DirectMedia::Application::GetInstance()->GetDisplayDPI(0) / 96.0f;
    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    Create("Painter",
        SDL_WINDOWPOS_CENTERED_DISPLAY(0), SDL_WINDOWPOS_CENTERED_DISPLAY(0),
        int(800 * windowScale), int(600 * windowScale),
        windowFlags);

    m_renderer = new DirectMedia::Renderer(this);
    m_renderer->Init();

    m_guiContext = new DirectMedia::GuiContext(this, m_renderer.get());

    // Load custom fonts
    float fontSize = GetDisplayDPI() / 72.0f * 12.0f;
#ifdef _WIN32
    // https://github.com/ocornut/imgui/blob/master/docs/FONTS.md
    auto fonts = m_guiContext->GetFonts();
    fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", fontSize);
    fonts->Build();
#endif

    auto codecs = DirectMedia::Codec::EnumerateCodecs();
    for (const auto& codec : codecs)
    {
        LogGlobal(Info, "Found Codec: %s", codec->long_name);
    }

    return true;
}

bool Painter::OnEvent(const SDL_Event& event)
{
    if (m_guiContext)
    {
        m_guiContext->ProcessEvent(event);
    }
    return Window::OnEvent(event);
}

void Painter::OnShown()
{
    LogGlobal(Info, "OnShown");
}

void Painter::OnHidden()
{
    LogGlobal(Info, "OnHidden");
}

void Painter::OnExposed()
{
    LogGlobal(Info, "OnExposed");
}

void Painter::OnMoved(int x, int y)
{
    LogGlobal(Info, "OnMoved: %4d, %4d", x, y);
}

void Painter::OnResized(int width, int height)
{
    LogGlobal(Info, "OnResized: %4d, %4d", width, height);
}

void Painter::OnMinimized()
{
    LogGlobal(Info, "OnMinimized");
}

void Painter::OnMaximized()
{
    LogGlobal(Info, "OnMaximized");
}

void Painter::OnRestored()
{
    LogGlobal(Info, "OnRestored");
}

void Painter::OnEnter()
{
    LogGlobal(Info, "OnEnter");
}

void Painter::OnLeave()
{
    LogGlobal(Info, "OnLeave");
}

void Painter::OnKeyDown(const SDL_KeyboardEvent& keyDown)
{
    LogGlobal(Info, "OnKeyDown: %s", SDL_GetKeyName(keyDown.keysym.sym));
}

void Painter::OnKeyUp(const SDL_KeyboardEvent& keyUp)
{
    LogGlobal(Info, "OnKeyUp: %s", SDL_GetKeyName(keyUp.keysym.sym));
}

void Painter::OnMouseMove(const SDL_MouseMotionEvent& mouseMotion)
{
}

void Painter::OnMouseButtonDown(const SDL_MouseButtonEvent& mouseButton)
{
    LogGlobal(Info, "OnMouseButtonDown: %s", GetMouseButtonName(mouseButton.button));
}

void Painter::OnMouseButtonUp(const SDL_MouseButtonEvent& mouseButton)
{
    LogGlobal(Info, "OnMouseButtonUp: %s", GetMouseButtonName(mouseButton.button));
}

void Painter::OnMouseWheel(const SDL_MouseWheelEvent& mouseWheel)
{
    LogGlobal(Info, "OnMouseWheel: %+d", mouseWheel.y);
}

void Painter::OnRender()
{
    m_guiContext->NewFrame();

    ImDrawList* bgDrawList = ImGui::GetBackgroundDrawList();

    ImGui::Begin("Background", 0, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::ColorPicker4("ClearColor", &m_clearColor.x);
    ImGui::End();

    m_guiContext->PrepareDrawData();

    ImVec2 scale = m_guiContext->GetIO().DisplayFramebufferScale;
    m_guiContext->SetScale(scale.x, scale.y);

    m_guiContext->Clear(
        Uint8(m_clearColor.x * 255),
        Uint8(m_clearColor.y * 255),
        Uint8(m_clearColor.z * 255),
        Uint8(m_clearColor.w * 255)
    );
    m_guiContext->Render();
    m_guiContext->Present();
}

const char* Painter::GetMouseButtonName(Uint8 button)
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
