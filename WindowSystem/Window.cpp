#include "Window.hpp"

#include "stdlib.h"

void AbstractWindow::draw() {
}

AbstractWindow::~AbstractWindow() {
    if (parent) {
        // parent->detach(this);
        printf("%p is dying and detaching from parent %p\n", static_cast<void*>(this), static_cast<void*>(parent));
    } else {
        printf("Orphan window %p is dying\n", static_cast<void*>(this));
    }
}

ContainerWindow::~ContainerWindow() {
    for (auto child : children) {
        delete child;
    }
}

void ContainerWindow::draw() {
    for (auto child : children) {
        child->draw();
    }
}

void AbstractWindow::processEvent(Event ev) {
    if (!(ev.eventType & eventMask))
        return;

    handleEvent(ev);
}

void AbstractWindow::attachToParent(AbstractWindow* parent) {
    this->parent = parent;
}

void AbstractWindow::updateEventMask(uint64_t update) {
    eventMask |= update;
    if (parent) {
        parent->updateEventMask(update);
    }
}

void AbstractWindow::handleEvent(Event ev) {
    printf("Event type: %lx\nEvent type mask: %lx\n\n", ev.eventType, eventMask);
}

void ContainerWindow::processEvent(Event ev) {
    if (!(ev.eventType & eventMask))
        return;

    handleEvent(ev);

    for (auto child : children) {
        child->processEvent(ev);
    }
}

void ContainerWindow::attachChild(AbstractWindow* win) {
    children.push_back(win);
    win->attachToParent(this);
}

void RectangleWindow::setPosition(unsigned int x, unsigned int y) {
    this->x = x;
    this->y = y;
}

void RectangleWindow::setSize(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
}

void RectangleWindow::setThickness(float thickness) {
    this->thickness = thickness;
}

void RectangleWindow::setBackgroundColor(const Color& color) {
    this->bkg = color;
}

void RectangleWindow::setOutlineColor(const Color& color) {
    this->frg = color;
}

void RectangleWindow::draw() {
    RenderEngine::DrawRect(x, y, width, height, bkg, frg, thickness);
}

AbstractButton::AbstractButton() {
    pressed = false;
    hovered = false;
}

void AbstractButton::attachToParent(AbstractWindow* parent) {
    this->parent = parent;
    updateEventMask(EV_MOUSE_KEY_PRESS | EV_MOUSE_KEY_RELEASE | EV_MOUSE_MOVE);  // Need to track key presses, releases & movements in order to properly update state
}

void AbstractButton::setHoverColor(const Color& color) {
    hoverBkg = color;
}

void AbstractButton::setPressColor(const Color& color) {
    pressBkg = color;
}

void AbstractButton::click() {
    printf("Abstract button %p just got clicked\n", static_cast<void*>(this));
}

void AbstractButton::handleEvent(Event ev) {
    if (IS_MOUSE_EV(ev)) {
        bool inside = isInside(ev.mouse.x, ev.mouse.y);

        if (ev.eventType == EV_MOUSE_KEY_PRESS && inside) {
            hovered = true;
            pressed = true;
        } else if (ev.eventType == EV_MOUSE_KEY_RELEASE && pressed) {
            if (inside) click();
            pressed = false;
        } else if (ev.eventType == EV_MOUSE_MOVE) {
            if (inside) {
                hovered = true;
            } else {
                hovered = false;
            }
        }
    }

    if (pressed) {
        bkg = pressBkg;
    } else if (hovered) {
        bkg = hoverBkg;
    } else {
        bkg = defaultBkg;
    }
}

void Slider::handleEvent(Event ev) {
    if (IS_MOUSE_EV(ev)) {
        bool inside = isInside(ev.mouse.x, ev.mouse.y);

        if (pressed) {
            x = scrollX + ev.mouse.x - startX;
            y = scrollY + ev.mouse.y - startY;
        }

        if (ev.eventType == EV_MOUSE_KEY_PRESS && inside) {
            hovered = true;
            pressed = true;
            startX = ev.mouse.x;
            startY = ev.mouse.y;
            scrollX = x;
            scrollY = y;
        } else if (ev.eventType == EV_MOUSE_KEY_RELEASE && pressed) {
            pressed = false;
        } else if (ev.eventType == EV_MOUSE_MOVE) {
            if (inside) {
                hovered = true;
            } else {
                hovered = false;
            }
        }
    }

    if (pressed) {
        bkg = pressBkg;
    } else if (hovered) {
        bkg = hoverBkg;
    } else {
        bkg = defaultBkg;
    }
}

bool AbstractButton::isInside(unsigned int x, unsigned int y) {
    unsigned int xstart = this->x;
    unsigned int xend = this->x + this->width;
    unsigned int ystart = this->y;
    unsigned int yend = this->y + this->height;

    if (thickness > 0) {
        xstart -= thickness;
        ystart -= thickness;
        xend += thickness;
        yend += thickness;
    }

    return (xstart <= x) && (ystart <= y) && (xend >= x) && (yend >= y);
}

void AbstractButton::setBackgroundColor(const Color& color) {
    defaultBkg = color;
    bkg = color;
}
