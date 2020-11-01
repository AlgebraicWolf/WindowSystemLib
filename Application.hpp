#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_
#include "SFMLRenderEngine/RenderEngine.hpp"
#include "WindowSystem/Window.hpp"


// Singletone application class
class Application {
   public:
    static void Init();
    static void Finalize();
    static bool Run();

   private:
    static ContainerWindow *rootWindow;
    Application();  // Ensure that class is indeed singletone by prohibiting object construction
};

ContainerWindow *Application::rootWindow;

void Application::Init() {
    RenderEngine::Init(800, 600);
    rootWindow = new ContainerWindow();

    Color bkg;
    Color frg;

    bkg.red = 0;
    bkg.green = 0;
    bkg.blue = 0;
    bkg.alpha = 0;

    frg.red = 255;
    frg.green = 255;
    frg.blue = 255;
    frg.alpha = 255;

    RectangleWindow *rect1 = new RectangleWindow();
    rect1->setBackgroundColor(bkg);
    rect1->setOutlineColor(frg);
    rect1->setPosition(50, 50);
    rect1->setSize(200, 200);
    rect1->setThickness(-30);

    RectangleWindow *rect2 = new RectangleWindow();
    rect2->setBackgroundColor(bkg);
    rect2->setOutlineColor(frg);
    rect2->setPosition(300, 50);
    rect2->setSize(200, 200);
    rect2->setThickness(30);

    rootWindow->attachChild(rect1);
    rootWindow->attachChild(rect2);

    rect1->updateEventMask(EV_KEYBOARD_PRESS | EV_KEYBOARD_RELEASE);
    rect2->updateEventMask(EV_MOUSE_KEY_PRESS | EV_MOUSE_KEY_RELEASE);
}

void Application::Finalize() {
    delete rootWindow;
    RenderEngine::Finalize();
}

bool Application::Run() {
    Event ev;
    while (RenderEngine::PollEvent(ev)) {
        rootWindow->processEvent(ev);

        switch (ev.eventType)
        {
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