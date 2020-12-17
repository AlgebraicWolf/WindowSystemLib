#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_
#include <cstdio>

#include "SFMLRenderEngine/RenderEngine.hpp"
#include "WindowSystem/Window.hpp"

// Singletone application class
class Application {
   public:
    static void Init(uint32_t width, uint32_t height);
    static void Finalize();
    static bool Run();
    static void Attach(AbstractWindow *win);
    static void DumpHierarchy(const char *filename);

   private:
    static ContainerWindow *rootWindow;
    Application();  // Ensure that class is indeed singletone by prohibiting object construction
};

ContainerWindow *Application::rootWindow;

void Application::Attach(AbstractWindow *win) {
    rootWindow->attachChild(win);
}

void Application::Init(uint32_t width, uint32_t height) {
    RenderEngine::Init(width, height);
    rootWindow = new ModalWindowManager();
}

void Application::Finalize() {
    delete rootWindow;
    RenderEngine::Finalize();
}

void Application::DumpHierarchy(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Unable to open dump file\n");
    }

    fprintf(f, "digraph {\n");
    rootWindow->dump(f);
    fprintf(f, "}\n");
    fclose(f);
}

bool Application::Run() {
    Event ev;
    while (RenderEngine::PollEvent(ev)) {
        rootWindow->processEvent(ev);

        switch (ev.eventType) {
            case EV_CLOSED:
                return 0;
                break;

            default:
                break;
        }
    }

    RenderEngine::Clear();
    rootWindow->draw();

    RenderEngine::Display();

    return 1;
}
#endif  // APPLICATION_HPP_