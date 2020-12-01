#include "GraphicEditor.hpp"

#include <cstring>

#include "../SFMLRenderEngine/RenderEngine.hpp"

Canvas::Canvas(uint32_t width, uint32_t height) {
    setSize(width, height);
    pressed = false;

    data = new uint32_t[width * height];
    memset(data, 0xFF, width * height * 4);
    updateEventMask(EV_MOUSE_MOVE | EV_MOUSE_KEY_PRESS | EV_MOUSE_KEY_RELEASE);
}

Canvas::~Canvas() {
    delete[] data;
}

uint32_t *Canvas::getData() {
    return data;
}

void Canvas::draw() {
    RenderEngine::DrawBitmap(x, y, width, height, data);
}

void Canvas::handleEvent(Event ev) {
    if (ev.eventType == EV_MOUSE_KEY_PRESS) {
        pressed = true;
        prev_x = ev.mouse.x - x;
        prev_y = ev.mouse.y - y;
    } else if (ev.eventType == EV_MOUSE_KEY_RELEASE) {
        pressed = false;
    }

    // fprintf(stderr, "X: %d, Y: %d, inside: %s\n", ev.mouse.x, ev.mouse.y, isInsideRect(ev.mouse.x, ev.mouse.y) ? "True" : "False");

    if (pressed && isInsideRect(ev.mouse.x, ev.mouse.y)) {
        uint32_t relX = ev.mouse.x - x;
        uint32_t relY = ev.mouse.y - y;

        int32_t x0 = prev_x;
        int32_t x1 = relX;
        int32_t y0 = prev_y;
        int32_t y1 = relY;

        int32_t delta_y = y1 - y0;
        int32_t delta_x = x1 - x0;

        uint32_t num_steps = (1 + std::max(std::abs(delta_x), std::abs(delta_y)));

        double x_step = static_cast<double>(delta_x) / num_steps;
        double y_step = static_cast<double>(delta_y) / num_steps;

        for (uint32_t i = 0; i < num_steps; i++) {
            data[static_cast<uint32_t>(y0 + y_step * i) * width + static_cast<uint32_t>(x0 + x_step * i)] = 0;
        }

        prev_x = relX;
        prev_y = relY;
    }
}