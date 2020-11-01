#ifndef WINDOW_HPP_
#define WINDOW_HPP_
#include <list>

#include "../Event.hpp"
#include "../SFMLRenderEngine/RenderEngine.hpp"

// Abstract window that can handle an event
class AbstractWindow {
   public:
    virtual void draw();                                  // Draw the window
    virtual void processEvent(Event ev);                  // Event processing. Just dummy function that calls handleEvent
    virtual void attachToParent(AbstractWindow *parent);  // Function that attaches this window to some other window
    void updateEventMask(uint64_t update);                // Function that updates event mask of the window as well as of its paternal node
    virtual ~AbstractWindow();                            // Virtual destructor

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

   protected:
    int x;
    int y;
    int width;
    int height;
    float thickness;
    Color bkg;  // Color of the rectangle
    Color frg;  // Color of the outline
};

class AbstractButton : public RectangleWindow {
   public:
    AbstractButton();                             // Button constructor that shall initialize fields of integral types and subscribe to events
    virtual void click();                         // Click payload
    void setHoverColor(const Color &color);       // Set button hover color
    void setPressColor(const Color &color);       // Set button press color
    void setBackgroundColor(const Color &color);  // Set color for chillin' button

    virtual void attachToParent(AbstractWindow *parent) override;  // Function that attaches this window to some other window
    bool isInside(unsigned int x, unsigned int y);                 // Checks whether the given point is inside the button

   protected:
    virtual void handleEvent(Event ev) override;  // New event handler
    Color hoverBkg;                               // Color of the button when it is being hovered over
    Color pressBkg;                               // Color of the button when it is being pressed on
    Color defaultBkg;                             // Color of button when it's chillin'

    bool hovered;  // Button is currently hovered over
    bool pressed;  // Button is currently being pressed down
};

// Slider -- control that can be moved either horizontally of vertically
class Slider : public AbstractButton {
   public:
    void setLimits(unsigned int min, unsigned int max);

   private:
    virtual void handleEvent(Event ev) override;  // handleEvent should be overriden in order to allow for proper movement
    int pivotX;                                   // Coordinates of the beginning
    int pivotY;                                   // Coordinates of the beginning
    int limit;                                    // Amount of movement to allow for
    bool isHorizontal;                            // Is slider horizontal or vertical
};

// Scrollbar -- control with two buttons and a slider
class Scrollbar : public ContainerWindow {
   public:
    Scrollbar(unsigned int height);  // Constructor of slider element
   private:
    unsigned int height;  // Height of the active part of the slider
};

#endif  // WINDOW_HPP_