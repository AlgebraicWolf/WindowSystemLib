#ifndef EVENT_HPP_
#define EVENT_HPP_
#include <cstdint>

#include "EventTypes.hpp"

struct Event {
    // Useful enumerations
    enum MOUSE_BUTTON {
        NONE = 0,
        LEFT,
        RIGHT,
        MIDDLE
    };

    enum SCROLL {
        UP,
        DOWN,
        PG_UP,
        PG_DOWN,
        RELOCATION
    };

    // Structs for different types of events
    struct Mouse {
        unsigned int x;
        unsigned int y;
        MOUSE_BUTTON button;
    };

    struct Keyboard {
        unsigned char keyCode;
    };

    struct Scroll {
        SCROLL scrollType;
        float position;
    };

    // Member data
    uint64_t eventType;
    union {
        Mouse mouse;
        Keyboard keyboard;
        Scroll scroll;
    };
};
#endif  // EVENT_HPP_