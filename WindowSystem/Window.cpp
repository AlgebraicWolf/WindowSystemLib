#include "Window.hpp"

#include "stdlib.h"

void AbstractWindow::draw() {
}

AbstractWindow::AbstractWindow() {
    parent = nullptr;
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
    parent->updateEventMask(eventMask);
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

void RectangleWindow::setPosition(int x, int y) {
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

bool RectangleWindow::isInside(unsigned int x, unsigned int y) {
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

void Slider::setPosition(int x, int y) {
    this->x = x;
    this->y = y;

    if (isHorizontal) {
        pivot = x;
    } else {
        pivot = y;
    }
}

Slider::Slider(bool isHorizontal) : isHorizontal(isHorizontal) {
    eventMask |= EV_SCROLL;  // Don't want to propagate subscription
}

void Slider::setLimit(int limit) {
    this->limit = limit;
}

void Slider::handleEvent(Event ev) {
    if (IS_MOUSE_EV(ev)) {
        bool inside = isInside(ev.mouse.x, ev.mouse.y);

        if (pressed) {
            if (isHorizontal) {
                x = movementStart + ev.mouse.x - strokeStart;
            } else {
                y = movementStart + ev.mouse.y - strokeStart;
            }
        }

        if (ev.eventType == EV_MOUSE_KEY_PRESS && inside) {
            hovered = true;
            pressed = true;
            if (isHorizontal) {
                strokeStart = ev.mouse.x;
                movementStart = x;
            } else {
                strokeStart = ev.mouse.y;
                movementStart = y;
            }
        } else if (ev.eventType == EV_MOUSE_KEY_RELEASE && pressed) {
            pressed = false;
        } else if (ev.eventType == EV_MOUSE_MOVE) {
            if (inside) {
                hovered = true;
            } else {
                hovered = false;
            }
        }
    } else if (ev.eventType == EV_SCROLL) {
        switch (ev.scroll.scrollType) {
            case Event::UP:
                if (isHorizontal) {
                    x -= 10;
                } else {
                    y -= 10;
                }
                break;

            case Event::DOWN:
                if (isHorizontal) {
                    x += 10;
                } else {
                    y += 10;
                }
                break;

            case Event::PG_UP:
                if (isHorizontal) {
                    x -= 30;
                } else {
                    y -= 30;
                }
                break;

            case Event::PG_DOWN:
                if (isHorizontal) {
                    x += 30;
                } else {
                    y += 30;
                }
                break;

            default:
                break;
        }
    }

    if (isHorizontal) {
        if (x < pivot)
            x = pivot;
        else if (x > pivot + limit)
            x = pivot + limit;
    } else {
        if (y < pivot)
            y = pivot;
        else if (y > pivot + limit)
            y = pivot + limit;
    }

    if (pressed) {
        bkg = pressBkg;
    } else if (hovered) {
        bkg = hoverBkg;
    } else {
        bkg = defaultBkg;
    }
}

int Slider::getPositionAlongAxis() {
    if (isHorizontal) {
        return x;
    } else {
        return y;
    }
}

Scrollbar::Scrollbar(int length, bool isHorizontal) : isHorizontal(isHorizontal), length(length) {
    up = new ScrollbarButton(true);
    down = new ScrollbarButton(false);
    slider = new Slider(isHorizontal);
    bkg = new ScrollbarBackground(isHorizontal);

    attachChild(bkg);
    attachChild(up);
    attachChild(down);
    attachChild(slider);

    up->setSize(20, 20);
    down->setSize(20, 20);

    if (isHorizontal) {
        bkg->setSize(length, 20);
    } else {
        bkg->setSize(20, length);
    }

    up->setThickness(-1);
    down->setThickness(-1);
    slider->setThickness(-1);
    bkg->setThickness(-1);

    eventMask |= EV_SCROLL;  // Don't really want to propagate subscription to scroll event since we want
}

void Scrollbar::setSliderSize(int size) {
    if (isHorizontal) {
        slider->setSize(size, 20);
    } else {
        slider->setSize(20, size);
    }

    slider->setLimit(length - size);
}

void Scrollbar::setPosition(int x, int y) {
    up->setPosition(x, y);
    if (isHorizontal) {
        bkg->setPosition(x + 20, y);
        slider->setPosition(x + 20, y);
        down->setPosition(x + 20 + length, y);
    } else {
        bkg->setPosition(x, y + 20);
        slider->setPosition(x, y + 20);
        down->setPosition(x, y + 20 + length);
    }
}

void Scrollbar::setOutlineColor(const Color& color) {
    up->setOutlineColor(color);
    down->setOutlineColor(color);
    slider->setOutlineColor(color);
    bkg->setOutlineColor(color);
}

void Scrollbar::setBackgroundColor(const Color& color) {
    up->setBackgroundColor(color);
    down->setBackgroundColor(color);
    slider->setBackgroundColor(color);
    bkg->setBackgroundColor(color);
}

void Scrollbar::setHoverColor(const Color& color) {
    up->setHoverColor(color);
    down->setHoverColor(color);
    slider->setHoverColor(color);
}

void Scrollbar::setPressColor(const Color& color) {
    up->setPressColor(color);
    down->setPressColor(color);
    slider->setPressColor(color);
}

bool Scrollbar::isInsideSlider(int x, int y) {
    return slider->isInside(x, y);
}

int Scrollbar::getSliderPositionAlongAxis() {
    return slider->getPositionAlongAxis();
}

ScrollbarButton::ScrollbarButton(bool isUp) : isUp(isUp) {}

void ScrollbarButton::click() {
    if (!parent)
        return;

    Event scrollEvent;
    scrollEvent.eventType = EV_SCROLL;

    if (isUp) {
        scrollEvent.scroll.scrollType = Event::UP;
    } else {
        scrollEvent.scroll.scrollType = Event::DOWN;
    }

    parent->processEvent(scrollEvent);
}

TextWindow::TextWindow() {
    content = nullptr;
    viewX = 0;
    viewY = 0;
}

void TextWindow::setText(const char* newContent) {
    content = newContent;
}

void TextWindow::setViewportPosition(int x, int y) {
    viewX = x;
    viewY = y;
}

void TextWindow::draw() {
    viewport.create(width, height);
    RenderEngine::SetRenderTarget(&viewport);
    RenderEngine::DrawText(-viewX, -viewY, content);
    RenderEngine::RenderToMain();
    RenderEngine::DrawRenderTarget(x, y, viewport);
}

ScrollbarBackground::ScrollbarBackground(bool isHorizontal) : isHorizontal(isHorizontal) {
    eventMask = EV_MOUSE_KEY_RELEASE;
}

void ScrollbarBackground::handleEvent(Event ev) {
    Scrollbar* p = static_cast<Scrollbar*>(parent);  // Believe me
    if (!parent) return;

    if (ev.eventType == EV_MOUSE_KEY_RELEASE && isInside(ev.mouse.x, ev.mouse.y) && !p->isInsideSlider(ev.mouse.x, ev.mouse.y)) {
        printf("Scrollbar background had received an interesting event!\n");
        int coord = 0;
        if (isHorizontal) {
            coord = ev.mouse.x;
        } else {
            coord = ev.mouse.y;
        }

        Event newEv;
        newEv.eventType = EV_SCROLL;

        if (coord < p->getSliderPositionAlongAxis()) {
            newEv.scroll.scrollType = Event::PG_UP;
        } else {
            newEv.scroll.scrollType = Event::PG_DOWN;
        }

        p->processEvent(newEv);
    }
}