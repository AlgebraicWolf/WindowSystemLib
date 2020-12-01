#include "Application.hpp"
#include "GraphicEditor/GraphicEditor.hpp"

int main() {
    Application::Init();

    // Color defBkg = {0, 0, 0, 0};
    // Color hoverBkg = {100, 100, 100, 255};
    // Color pressBkg = {255, 255, 255, 255};

    // ScrollbarManager *scr = new ScrollbarManager(true, true);
    // scr->adjustScrollbarSize(38, 38, 700, 500);
    // scr->adjustScrollableAreaSize(720, 5500);
    
    // // TODO setProperty functions overload 
    // scr->horizontal->setBackgroundColor(defBkg);
    // scr->horizontal->setHoverColor(hoverBkg);
    // scr->horizontal->setPressColor(pressBkg);
    // scr->horizontal->setOutlineColor(pressBkg);

    // scr->vertical->setBackgroundColor(defBkg);
    // scr->vertical->setHoverColor(hoverBkg);
    // scr->vertical->setPressColor(pressBkg);
    // scr->vertical->setOutlineColor(pressBkg);

    // TextWindow *text = new TextWindow;
    // Viewport *view = new Viewport;

    // view->setPosition(Vector2<int>(38, 38));
    // view->setSize(Vector2<int>(700, 500));
    // view->setSpan(Vector2<int>(720, 5500));

    // view->updateEventMask(EV_SCROLL);
    // text->setText(poem);
    // text->setSize(720, 5500);
    // text->setPosition(0, 0);
    // text->setBackgroundColor(defBkg);
    // text->setOutlineColor(pressBkg);
    // text->setThickness(-1);

    // view->attachChild(text);

    // scr->attachChild(view);
    // Application::Attach(scr);

    // Application::DumpHierarchy("dump.dot");

    Canvas *c = new Canvas(700, 500);
    c->setPosition(50, 50);
    c->setThickness(0);
    Application::Attach(c);

    while (Application::Run()) {}

    Application::Finalize();
    return 0;   
}