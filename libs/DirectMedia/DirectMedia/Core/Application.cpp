#include "Application.h"
#include "Window.h"
#include "rad/IO/Logging.h"

#include "SDL2/SDL_vulkan.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace sdl
{

static Application* g_app = nullptr;

Application* GetApp()
{
    return g_app;
}

Application::Application()
{
    assert(g_app == nullptr);
    g_app = this;
}

Application::~Application()
{
    Destroy();
    g_app = nullptr;
}

const char* Application::GetError()
{
    return SDL_GetError();
}

bool Application::Init(int argc, char** argv)
{
    m_argc = argc;
    m_argv = argv;
#if defined(_WIN32) || defined(_WIN64)
    ::SetConsoleOutputCP(65001);
#endif

    std::string commandLine;
    for (int i = 0; i < argc; ++i)
    {
        commandLine += std::string(argv[i]) + " ";
    }
    commandLine.pop_back();
    LogGlobal(Info, "Command line: {}", commandLine);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        LogGlobal(Error, "SDL_Init failed: {}", SDL_GetError());
    }

    SDL_version linkedVersion = {};
    SDL_GetVersion(&linkedVersion);
    LogGlobal(Info, "SDL {}.{}.{} initialized.",
        linkedVersion.major, linkedVersion.minor, linkedVersion.patch);

    LogGlobal(Info, "Platform: {}", SDL_GetPlatform());
    char* basePath = SDL_GetBasePath();
    LogGlobal(Info, "BasePath: {}", basePath);
    SDL_free(basePath);

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    UpdateDisplayInfos();

    return true;
}

void Application::Destroy()
{
    if (m_isVulkanLibraryLoaded)
    {
        SDL_Vulkan_UnloadLibrary();
    }

    SDL_Quit();
    LogGlobal(Info, "SDL Quited.");
}

const char* Application::GetPlatformName() const
{
    return SDL_GetPlatform();
}

std::string Application::GetBasePath() const
{
    return SDL_GetBasePath();
}

bool Application::HasClipboardText() const
{
    return SDL_HasClipboardText();
}

std::string Application::GetClipboardText()
{
    std::string text;
    if (char* pText = SDL_GetClipboardText())
    {
        text = pText;
    }
    else
    {
        LogGlobal(Warn, "GetClipboardText() failed: {}", SDL_GetError());
    }
    return text;
}

bool Application::SetClipboardText(std::string_view text)
{
    if (SDL_SetClipboardText(text.data()) == 0)
    {
        return true;
    }
    else
    {
        LogGlobal(Warn, "SDL_SetClipboardText() failed: {}: {}", SDL_GetError());
        return false;
    }
}

bool Application::LoadVulkanLibrary(const char* path)
{
    if (SDL_Vulkan_LoadLibrary(path) == 0)
    {
        m_isVulkanLibraryLoaded = true;
        return true;
    }
    else
    {
        LogGlobal(Error, "SDL_Vulkan_LoadLibrary(path={}): {}",
            path ? path : "nullptr", SDL_GetError());
        return false;
    }
}

std::vector<const char*> Application::GetVulkanInstanceExtensions()
{
    std::vector<const char*> extensionNames;
    unsigned int extensionCount = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(NULL, &extensionCount, NULL))
    {
        LogGlobal(Error, "SDL_Vulkan_GetInstanceExtensions failed");
    }
    extensionNames.resize(extensionCount);
    if (!SDL_Vulkan_GetInstanceExtensions(NULL, &extensionCount, extensionNames.data()))
    {
        LogGlobal(Error, "SDL_Vulkan_GetInstanceExtensions failed");
    }
    return extensionNames;
}

void Application::RegisterEventHandler(EventHandler* handler)
{
    m_eventHandlers.push_back(handler);
}

void Application::UnregisterEventHandler(EventHandler* handler)
{
    std::erase(m_eventHandlers, handler);
}

int Application::Run()
{
    while (!m_exitEventLoop)
    {
        SDL_Event event = {};
        while (SDL_PollEvent(&event))
        {
            OnEvent(event);
        }

        for (EventHandler* handler : m_eventHandlers)
        {
            handler->OnIdle();
        }

        if (m_eventHandlers.empty())
        {
            m_exitEventLoop = true;
        }
    }
    return m_exitCode;
}

bool Application::OnEvent(const SDL_Event& event)
{
    for (EventHandler* handler : m_eventHandlers)
    {
        if (handler->OnEvent(event))
        {
            return true;
        }
    }

    switch (event.type)
    {
    case SDL_QUIT:
        m_exitEventLoop = true;
        break;
    case SDL_LOCALECHANGED:
        LogGlobal(Info, "SDL_LOCALECHANGED: The user's locale preferences have changed.");
        break;
    case SDL_DISPLAYEVENT:
        LogGlobal(Info, "SDL_DISPLAYEVENT: Display state change.");
        UpdateDisplayInfos();
        break;
    }

    // These application events have special meaning on iOS, see README-ios.md for details.
    switch (event.type)
    {
    case SDL_APP_TERMINATING:
        LogGlobal(Info, "SDL_APP_TERMINATING: "
            "The application is being terminated by the OS.");
        break;
    case SDL_APP_LOWMEMORY:
        LogGlobal(Info, "SDL_APP_LOWMEMORY: "
            "The application is low on memory, free memory if possible.");
        break;
    case SDL_APP_WILLENTERBACKGROUND:
        LogGlobal(Info, "SDL_APP_WILLENTERBACKGROUND: "
            "The application is about to enter the background.");
        break;
    case SDL_APP_DIDENTERBACKGROUND:
        LogGlobal(Info, "SDL_APP_DIDENTERBACKGROUND: "
            "The application did enter the background and may not get CPU for some time.");
        break;
    case SDL_APP_WILLENTERFOREGROUND:
        LogGlobal(Info, "SDL_APP_WILLENTERFOREGROUND: "
            "The application is about to enter the foreground.");
        break;
    case SDL_APP_DIDENTERFOREGROUND:
        LogGlobal(Info, "SDL_APP_DIDENTERFOREGROUND: "
            "The application is now interactive.");
        break;
    }

    return true;
}

void Application::UpdateDisplayInfos()
{
    m_displayInfos.resize(SDL_GetNumVideoDisplays());
    for (int displayIndex = 0; displayIndex < static_cast<int>(m_displayInfos.size()); displayIndex++)
    {
        DisplayInfo& displayInfo = m_displayInfos[displayIndex];
        displayInfo.name = SDL_GetDisplayName(displayIndex);
        SDL_GetDisplayBounds(displayIndex, &displayInfo.bounds);
        SDL_GetDisplayUsableBounds(displayIndex, &displayInfo.usableBounds);
        SDL_GetDisplayDPI(displayIndex, &displayInfo.ddpi, &displayInfo.hdpi, &displayInfo.vdpi);
        displayInfo.modes.resize(SDL_GetNumDisplayModes(displayIndex));
        for (int modeIndex = 0; modeIndex < displayInfo.modes.size(); modeIndex++)
        {
            SDL_GetDisplayMode(displayIndex, modeIndex, &displayInfo.modes[modeIndex]);
        }
    }
}

SDL_DisplayMode Application::GetDisplayCurrentMode(int displayIndex) const
{
    SDL_DisplayMode mode = {};
    SDL_GetCurrentDisplayMode(displayIndex, &mode);
    return mode;
}

float Application::GetDisplayDPI(int displayIndex)
{
    float ddpi = 96.0f;
    float hdpi = 96.0f;
    float vdpi = 96.0f;
    if (SDL_GetDisplayDPI(displayIndex, &ddpi, &hdpi, &vdpi) != 0)
    {
        LogGlobal(Error, "SDL_GetDisplayDPI: {}", SDL_GetError());
    }
    return std::min(hdpi, vdpi);
}

} // namespace sdl
