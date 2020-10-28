#ifndef WINDOW_HPP_
#define WINDOW_HPP_
#include <list>

#include "../Event.hpp"
#include "../RenderEngine.hpp"

// Abstract window that can handle an event
class AbstractWindow {
   public:
    virtual void draw();                  // Draw the window
    virtual void processEvent(Event ev);  // Event processing. Just dummy function that calls handleEvent

   private:
    uint64_t eventMask;                  // Mask for filtering out unnecessary events
    virtual void handleEvent(Event ev);  // Handle certain (function that should be overloaded in order to implement event handling)
    AbstractWindow *parent;              // Parent window
};

// Abstract container window that can have child windows and pass on events.
// Here implementation of processEvent is different and actually passes on event to children
class ContainerWindow : public AbstractWindow {
   public:
   private:
    std::list<AbstractWindow *> children;
};
#endif  // WINDOW_HPP_