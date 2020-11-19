#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_
#include "SFMLRenderEngine/RenderEngine.hpp"
#include "WindowSystem/Window.hpp"
#include "Poem.h"


// Singletone application class
class Application {
   public:
    static void Init();
    static void Finalize();
    static bool Run();
    static void Attach(AbstractWindow *win);

   private:
    static ContainerWindow *rootWindow;
    Application();  // Ensure that class is indeed singletone by prohibiting object construction
};

ContainerWindow *Application::rootWindow;

void Application::Attach(AbstractWindow *win) {
    rootWindow->attachChild(win);
}

void Application::Init() {
    RenderEngine::Init(800, 600);
    rootWindow = new ContainerWindow();
}

void Application::Finalize() {
    delete rootWindow;
    RenderEngine::Finalize();
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