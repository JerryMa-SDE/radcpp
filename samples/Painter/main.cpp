#include "Painter.h"
#include "rad/IO/Logging.h"

using namespace rad;

int main(int argc, char* argv[])
{
    Ref<DirectMedia::Application> app = new DirectMedia::Application();
    if (!app->Init(argc, argv))
    {
        LogGlobal(Error, "Application::Init failed!");
        return -1;
    }

    Ref<Painter> window = new Painter();
    window->Init();

    return app->Run();
}
