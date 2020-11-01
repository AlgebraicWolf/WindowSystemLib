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

    enum SCROLLS {
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

    // Member data
    uint64_t eventType;
    union {
        Mouse mouse;
        Keyboard keyboard;
    };
};
#endif  // EVENT_HPP_