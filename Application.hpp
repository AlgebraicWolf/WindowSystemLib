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

    Color defBkg = {0, 0, 0, 0};
    Color hoverBkg = {100, 100, 100, 255};
    Color pressBkg = {255, 255, 255, 255};

    AbstractButton *but1 = new AbstractButton;
    but1->setBackgroundColor(defBkg);
    but1->setHoverColor(hoverBkg);
    but1->setPressColor(pressBkg);
    but1->setOutlineColor(pressBkg);
    but1->setThickness(1);

    but1->setPosition(50, 50);
    but1->setSize(20, 20);

    AbstractButton *but2 = new AbstractButton;
    but2->setBackgroundColor(defBkg);
    but2->setHoverColor(hoverBkg);
    but2->setPressColor(pressBkg);
    but2->setOutlineColor(pressBkg);
    but2->setThickness(1);

    but2->setPosition(50, 350);
    but2->setSize(20, 20);

    AbstractButton *sliderBkg = new AbstractButton;
    
    sliderBkg->setBackgroundColor(defBkg);
    sliderBkg->setHoverColor(defBkg);
    sliderBkg->setPressColor(defBkg);
    sliderBkg->setOutlineColor(pressBkg);
    sliderBkg->setThickness(1);

    sliderBkg->setPosition(50, 70);
    sliderBkg->setSize(20, 280);

    Slider *slider = new Slider;

    slider->setBackgroundColor(defBkg);
    slider->setHoverColor(hoverBkg);
    slider->setPressColor(pressBkg);
    slider->setOutlineColor(pressBkg);
    slider->setThickness(1);

    slider->setPosition(50, 220);
    slider->setSize(20, 60);

    rootWindow->attachChild(but1);
    rootWindow->attachChild(but2);
    rootWindow->attachChild(sliderBkg);
    rootWindow->attachChild(slider);
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