#include "WindowTest.h"
#include "rad/IO/Logging.h"

int main(int argc, char* argv[])
{
    rad::Ref<sdl::Application> app = new sdl::Application();
    if (!app->Init(argc, argv))
    {
        LogGlobal(Error, "Application::Init failed!");
        return -1;
    }

    const std::vector<sdl::DisplayInfo>& displayInfos = app->GetDisplayInfos();
    for (uint32_t i = 0; i < displayInfos.size(); ++i)
    {
        const sdl::DisplayInfo& info = displayInfos[i];
        const SDL_DisplayMode mode = app->GetDisplayCurrentMode(i);
        LogGlobal(Info, "Display#{}: {}: {:4} x {:4}, {} Hz, Format={}",
            i, info.name, mode.w, mode.h, mode.refresh_rate, SDL_GetPixelFormatName(mode.format));
    }

    rad::Ref<WindowTest> window = new WindowTest();
    window->Init();

    return app->Run();
}
