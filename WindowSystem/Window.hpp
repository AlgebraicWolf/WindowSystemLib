#ifndef WINDOW_HPP_
#define WINDOW_HPP_
#include <list>

#include "../Event.hpp"
#include "../SFMLRenderEngine/RenderEngine.hpp"

// TODO Buttons with icons

// Vector class for coordinate-like shit
// TODO Rewrite all things like (x, y), (width, height), etc. using this wonderful class
template <typename T>
struct Vector2 {
    T x;
    T y;
    Vector2();                        // Default constructor
    Vector2(const T &x, const T &y);  // Constructor with values
};

// Abstract window that can handle an event
class AbstractWindow {
   public:
    AbstractWindow();
    virtual void draw();                                  // Draw the window
    virtual void processEvent(Event ev);                  // Event processing. Just dummy function that calls handleEvent
    virtual void attachToParent(AbstractWindow *parent);  // Function that attaches this window to some other window
    void updateEventMask(uint64_t update);                // Function that updates event mask of the window as well as of its paternal node
    void updatePropagationMask(uint64_t update);          // Function that updates mask of events that are propagated further yet are not used by the window itself
    virtual ~AbstractWindow();                            // Virtual destructor
    virtual void dump(FILE *f);
    void detach(); // Detach from parent

   protected:
    uint64_t eventMask;                  // Mask for filtering out unnecessary events
    uint64_t propagationMask;            // Mask for filtering events that should be propag
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
    virtual void dump(FILE *f) override;
    void detachChild(AbstractWindow *child);

   protected:
    std::list<AbstractWindow *> children;
};

// Class of a rectangle primitive
class Rectangle {
   public:
    void setPosition(int x, int y);                         // Sets position of the rectangle window
    void setSize(unsigned int width, unsigned int height);  // Sets dimensions of the window
    void setBackgroundColor(const Color &color);            // Color of the rectangle itself
    void setOutlineColor(const Color &color);               // Color of the rectangle outline
    void setThickness(float thickness);                     // Outline thickness
    bool isInsideRect(int x, int y);
    int getHeight();
    int getWidth();

   protected:
    int x;
    int y;
    int width;
    int height;
    float thickness;
    Color bkg;  // Color of the rectangle
    Color frg;  // Color of the outline
};

// Container window that can be represented as a rectangle
class RectangleWindow : public ContainerWindow, public Rectangle {
   public:
    virtual void draw() override;  // Function that draws the rectangle window
    virtual void dump(FILE *f) override;
};

class AbstractButton : public AbstractWindow {
   public:
    AbstractButton();                                   // Button constructor that shall initialize fields of integral types and subscribe to events
    virtual void click(const Event &ev) = 0;            // Click payload
    virtual void onHoverEnter(const Event &ev) = 0;     // Hover enter payload
    virtual void onHoverExit(const Event &ev) = 0;      // Hover exit payload
    virtual void onButtonPress(const Event &ev) = 0;    // Button pressdown payload
    virtual void onMouseMove(const Event &ev) = 0;      // Arbitrary mouse move
    virtual void onButtonRelease(const Event &ev) = 0;  // Mouse button release
    void setHoverColor(const Color &color);             // Set button hover color
    void setPressColor(const Color &color);             // Set button press color
    void setBackgroundColor(const Color &color);        // Set color for chillin' button

    // virtual void attachToParent(AbstractWindow *parent) override;  // Function that attaches this window to some other window
    //  virtual bool isInside(int x, int y) ;                           // Function that checks intersection of a pixel with a button
    virtual bool isInside(int x, int y) = 0;
    virtual void dump(FILE *f) override;

   protected:
    virtual void handleEvent(Event ev) override;  // New event handler

    bool hovered;  // Button is currently hovered over
    bool pressed;  // Button is currently being pressed down
};

class RectangleButton : public AbstractButton, public Rectangle {
   public:
    void setHoverColor(const Color &color);       // Set button hover color
    void setPressColor(const Color &color);       // Set button press color
    void setBackgroundColor(const Color &color);  // Set color for chillin' button

    virtual void click(const Event &ev) override;            // Click payload
    virtual void onHoverEnter(const Event &ev) override;     // Hover enter payload
    virtual void onHoverExit(const Event &ev) override;      // Hover exit payload
    virtual void onButtonPress(const Event &ev) override;    // Button pressdown payload
    virtual void onMouseMove(const Event &ev) override;      // Arbitrary mouse move event
    virtual void onButtonRelease(const Event &ev) override;  // Mouse button release
    virtual bool isInside(int x, int y) override;
    virtual void draw() override;
    virtual void dump(FILE *f) override;

   protected:
    Color hoverBkg;
    Color pressBkg;
    Color defaultBkg;
};

// Slider -- control that can be moved either horizontally of vertically
class Slider : public RectangleButton {
   public:
    Slider(bool isHorizontal);
    void setLimit(int limit);
    void setPosition(int x, int y);  // Sets position of the rectangle window
    int getPositionAlongAxis();

    virtual void onButtonPress(const Event &ev) override;    // Button pressdown payload
    virtual void onMouseMove(const Event &ev) override;      // Arbitrary mouse move event
    virtual void onButtonRelease(const Event &ev) override;  // Mouse button release
    virtual void dump(FILE *f) override;

   private:
    virtual void handleEvent(Event ev) override;  // handleEvent should be overriden in order to allow for proper movement
    int pivot;                                    // Coordinates of the beginning
    int strokeStart;                              // Coordinate of the stroke start
    int movementStart;                            // Coordinate of the scrollbar in the beginning of the stroke
    int limit;                                    // Amount of movement to allow for
    bool isHorizontal;                            // Is slider horizontal or vertical

    friend class Scrollbar;
};

class ScrollbarButton : public RectangleButton {
   public:
    ScrollbarButton(bool isUp);
    virtual void dump(FILE *f) override;

   private:
    virtual void click(const Event &ev) override;
    bool isUp;
};

class ScrollbarBackground : public RectangleWindow {
   public:
    ScrollbarBackground(bool isHorizontal);
    virtual void dump(FILE *f) override;

   private:
    virtual void handleEvent(Event ev) override;
    bool isHorizontal;
};

// Scrollbar -- control with two buttons and a slider
class Scrollbar : public ContainerWindow {
   public:
    Scrollbar(int length, bool isHorizontal);  // Constructor of slider element
    void setSliderSize(int size);              // Set size of slider
    void setPosition(int x, int y);            // Scrollbar position
    void setOutlineColor(const Color &color);
    void setHoverColor(const Color &color);       // Set button hover color
    void setPressColor(const Color &color);       // Set button press color
    void setBackgroundColor(const Color &color);  // Set color for chillin' button
    bool isInsideSlider(int x, int y);
    int getSliderPositionAlongAxis();
    void setLength(int length);
    int getBkgLength();
    virtual void dump(FILE *f) override;

   private:
    virtual void handleEvent(Event ev) override;
    int x;
    int y;
    ScrollbarButton *up;
    ScrollbarButton *down;
    ScrollbarBackground *bkg;
    Slider *slider;

    bool isHorizontal;
    int length;  // Height of the active part of the slider

    friend class Slider;
};

// Non-owning text window
class TextWindow : public RectangleWindow {
   public:
    TextWindow();
    void setText(const wchar_t *newContent);
    void setCharSize(int size);
    virtual void draw() override;
    virtual void dump(FILE *f) override;

   private:
    // virtual void handleEvent(Event ev) override;
    const wchar_t *content;
    int characterSize;
};

class ScrollbarManager : public ContainerWindow {
   public:
    ScrollbarManager(bool horizontalScrollable, bool verticalScrollable);
    void adjustScrollbarSize(int x, int y, int width, int height);
    void adjustScrollableAreaSize(int width, int height);  // Adjusts scrollbar properties to the size of the scrollable area
    virtual void draw() override;
    Scrollbar *horizontal;
    Scrollbar *vertical;
    virtual void processEvent(Event ev) override;  // Event redirector
    virtual void dump(FILE *f) override;

   private:
    int adjWidth;
    int adjHeight;
};

// Class for an offscreen drawing of its contents. Window supports scrolling. The contents of children must be treated as relative to the Viewport position
class Viewport : public ContainerWindow {
   public:
    void setPosition(const Vector2<int> &pos);
    void setSpan(const Vector2<int> &span);
    void setSize(const Vector2<int> &size);
    virtual void draw() override;
    virtual void dump(FILE *f) override;

   protected:
    virtual void handleEvent(Event ev) override;

    Vector2<int> position;      // Position on the screen where it is rendered to
    Vector2<int> viewPosition;  // Position of the view in coordinate system of the contents
    Vector2<int> size;          // Size of the viewport
    Vector2<int> span;          // Span of the viewport to move along
};

#endif  // WINDOW_HPP_