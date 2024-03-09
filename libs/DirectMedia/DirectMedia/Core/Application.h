#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/String.h"
#include "EventHandler.h"
#include "SDL2/SDL.h"
#include <atomic>

namespace sdl
{

struct DisplayInfo
{
    std::string name;
    SDL_Rect bounds;
    SDL_Rect usableBounds;
    float ddpi;
    float hdpi;
    float vdpi;
    std::vector<SDL_DisplayMode> modes;
}; // struct DisplayInfo

class Application : public rad::RefCounted<Application>
{
public:
    Application();
    virtual ~Application();

    const char* GetError();

    bool Init(int argc, char** argv);
    void Destroy();

    int GetArgc() const { return m_argc; }
    const char* const* GetArgv() const { return m_argv; }
    const char* GetPlatformName() const;
    std::string GetBasePath() const;

    bool HasClipboardText() const;
    std::string GetClipboardText();
    bool SetClipboardText(std::string_view text);

    bool LoadVulkanLibrary(const char* path = nullptr);
    std::vector<const char*> GetVulkanInstanceExtensions();

    void RegisterEventHandler(EventHandler* handler);
    void UnregisterEventHandler(EventHandler* handler);
    int Run();
    void SetExitEventLoop(bool exit = true) { m_exitEventLoop = exit; }
    void SetExitCode(int exitCode) { m_exitCode = exitCode; }

    const std::vector<DisplayInfo>& GetDisplayInfos() const { return m_displayInfos; }
    SDL_DisplayMode GetDisplayCurrentMode(int displayIndex) const;

    // returns the min of horizontal and vertical DPI; default 96.0f if failed.
    float GetDisplayDPI(int displayIndex);

protected:
    bool OnEvent(const SDL_Event& event);
    void UpdateDisplayInfos();

    int m_argc = 0;
    const char* const* m_argv = nullptr;

    // Handlers registered for the event loop
    std::vector<EventHandler*> m_eventHandlers;
    std::atomic_bool m_exitEventLoop = false;
    int m_exitCode = 0;
    std::vector<DisplayInfo> m_displayInfos;

    bool m_isVulkanLibraryLoaded = false;

}; // class Application

Application* GetApp();

} // namespace sdl
