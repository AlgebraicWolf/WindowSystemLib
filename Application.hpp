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

    // Scrollbar *scrollbar = new Scrollbar(250, false);
    // scrollbar->setSliderSize(50);
    // scrollbar->setPosition(50, 50);

    // scrollbar->setBackgroundColor(defBkg);
    // scrollbar->setHoverColor(hoverBkg);
    // scrollbar->setPressColor(pressBkg);

    // scrollbar->setOutlineColor(pressBkg);

    ScrollbarManager *scr = new ScrollbarManager(true, true);
    scr->adjustScrollbarSize(50, 50, 700, 500);
    scr->adjustScrollableAreaSize(720, 5500);
    
    // TODO setProperty functions overload 
    scr->horizontal->setBackgroundColor(defBkg);
    scr->horizontal->setHoverColor(hoverBkg);
    scr->horizontal->setPressColor(pressBkg);
    scr->horizontal->setOutlineColor(pressBkg);

    scr->vertical->setBackgroundColor(defBkg);
    scr->vertical->setHoverColor(hoverBkg);
    scr->vertical->setPressColor(pressBkg);
    scr->vertical->setOutlineColor(pressBkg);

    // scr->horizontal->setSliderSize(50);
    // scr->vertical->setSliderSize(50);

    TextWindow *text = new TextWindow;
    text->updateEventMask(EV_SCROLL);
    text->setText(poem);
    text->setSize(700, 500);
    text->setPosition(50, 50);
    text->setViewportSpan(720, 5500);
    text->setViewportPosition(0, 0);
    text->setBackgroundColor(defBkg);
    text->setOutlineColor(pressBkg);
    text->setThickness(1);

    scr->attachChild(text);
    rootWindow->attachChild(scr);
    

    // AbstractButton *but1 = new AbstractButton;
    // but1->setBackgroundColor(defBkg);
    // but1->setHoverColor(hoverBkg);
    // but1->setPressColor(pressBkg);
    // but1->setOutlineColor(pressBkg);
    // but1->setThickness(1);

    // but1->setPosition(50, 50);
    // but1->setSize(20, 20);

    // AbstractButton *but2 = new AbstractButton;
    // but2->setBackgroundColor(defBkg);
    // but2->setHoverColor(hoverBkg);
    // but2->setPressColor(pressBkg);
    // but2->setOutlineColor(pressBkg);
    // but2->setThickness(1);

    // but2->setPosition(50, 350);
    // but2->setSize(20, 20);

    // AbstractButton *sliderBkg = new AbstractButton;
    
    // sliderBkg->setBackgroundColor(defBkg);
    // sliderBkg->setHoverColor(defBkg);
    // sliderBkg->setPressColor(defBkg);
    // sliderBkg->setOutlineColor(pressBkg);
    // sliderBkg->setThickness(1);

    // sliderBkg->setPosition(50, 70);
    // sliderBkg->setSize(20, 280);

    // Slider *slider = new Slider(true);

    // slider->setBackgroundColor(defBkg);
    // slider->setHoverColor(hoverBkg);
    // slider->setPressColor(pressBkg);
    // slider->setOutlineColor(pressBkg);
    // slider->setThickness(1);

    // slider->setPosition(50, 70);
    // slider->setSize(20, 60);
    // slider->setLimit(220);

    // rootWindow->attachChild(but1);
    // rootWindow->attachChild(but2);
    // rootWindow->attachChild(sliderBkg);
    // rootWindow->attachChild(slider);
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