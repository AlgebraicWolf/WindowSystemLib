#define EV_CLOSED            0b1
#define EV_MOUSE_KEY_PRESS   0b10
#define EV_MOUSE_KEY_RELEASE 0b100
#define EV_MOUSE_MOVE        0b1000
#define EV_KEYBOARD_PRESS    0b10000
#define EV_KEYBOARD_RELEASE  0b100000
#define EV_SCROLL            0b1000000
#define EV_TEXT              0b10000000

#define IS_MOUSE_EV(X) ((X).eventType & (EV_MOUSE_KEY_PRESS | EV_MOUSE_KEY_RELEASE | EV_MOUSE_MOVE))