#ifndef WINDOW_HPP_
#define WINDOW_HPP_
#include <list>

#include "../Event.hpp"
#include "../SFMLRenderEngine/RenderEngine.hpp"

// Abstract window that can handle an event
class AbstractWindow {
   public:
    virtual void draw();                          // Draw the window
    virtual void processEvent(Event ev);          // Event processing. Just dummy function that calls handleEvent
    void attachToParent(AbstractWindow *parent);  // Function that attaches this window to some other window
    void updateEventMask(uint64_t update);        // Function that updates event mask of the window as well as of its paternal node
    virtual ~AbstractWindow();                    // Virtual destructor

   protected:
    uint64_t eventMask;                  // Mask for filtering out unnecessary events
    AbstractWindow *parent;              // Parent window
    virtual void handleEvent(Event ev);  // Handle certain (function that should be overloaded in order to implement event handling)
};

// Abstract container window that can have child windows and pass on events.
// Here implementation of processEvent is different and actually passes on event to children
class ContainerWindow : public AbstractWindow {
   public:
    virtual void processEvent(Event ev) override;
    virtual void draw() override;
    void attachChild(AbstractWindow *win);
    virtual ~ContainerWindow();

   private:
    std::list<AbstractWindow *> children;
};

// Container window that can be represented as a rectangle
class RectangleWindow : public ContainerWindow {
   public:
    void setPosition(unsigned int x, unsigned int y);       // Sets position of the rectangle window
    void setSize(unsigned int width, unsigned int height);  // Sets dimensions of the window
    void setBackgroundColor(const Color &color);            // Color of the rectangle itself
    void setOutlineColor(const Color &color);               // Color of the rectangle outline
    void setThickness(float thickness);                     // Outline thickness

    virtual void draw();  // Function that draws the rectangle window

   private:
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    float thickness;
    Color bkg;  // Color of the rectangle
    Color frg;  // Color of the outline
};
#endif  // WINDOW_HPP_