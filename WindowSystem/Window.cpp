#include "Window.hpp"

#include <cstdio>
#include <cstdlib>

const char* uint64_to_bin(uint64_t val) {
    char* bin = new char[65]();
    for (uint64_t i = 0; i < 64; i++) {
        bin[63 - i] = (val & (uint64_t(1) << i)) ? '1' : '0';
    }

    return bin;
}

// TODO: Should probably split this into even more files, as it becomes increasingly difficult to
// navigate and maintain

// AbstractWindow methods
constexpr unsigned int scrollbarElemSize = 25;

void AbstractWindow::dump(FILE* f) {
    fprintf(f,
            "node%p [style=\"record\", label=\"{AbstractWindow | {Event mask | %s} | {Propagation "
            "mask | }}\"]\n",
            static_cast<void*>(this), uint64_to_bin(eventMask));

    if (parent) {
        fprintf(f, "node%p -> node%p\n", static_cast<void*>(this), static_cast<void*>(parent));
    }
}

void AbstractWindow::invokeModalWindow(ModalWindow* modal) {
    if (parent) parent->invokeModalWindow(modal);
}

#define DUMP_CONT(window)                                                                          \
    void window::dump(FILE* f) {                                                                   \
        fprintf(f,                                                                                 \
                "node%p [shape=\"record\", label=\"{ " #window                                     \
                " | {Event mask | %s } | {Propagation mask | %s }}\"]\n",                          \
                static_cast<void*>(this), uint64_to_bin(eventMask),                                \
                uint64_to_bin(propagationMask));                                                   \
        if (parent) {                                                                              \
            fprintf(f, "node%p -> node%p\n", static_cast<void*>(this),                             \
                    static_cast<void*>(parent));                                                   \
        }                                                                                          \
        for (auto child : children) {                                                              \
            child->dump(f);                                                                        \
            fprintf(f, "node%p -> node%p\n", static_cast<void*>(this), static_cast<void*>(child)); \
        }                                                                                          \
    }

#define DUMP(window)                                                    \
    void window::dump(FILE* f) {                                        \
        fprintf(f,                                                      \
                "node%p [shape=\"record\", label=\"{ " #window          \
                " | {Event mask | %s} | {Propagation mask | %s}}\"]\n", \
                static_cast<void*>(this), uint64_to_bin(eventMask),     \
                uint64_to_bin(propagationMask));                        \
        if (parent) {                                                   \
            fprintf(f, "node%p -> node%p\n", static_cast<void*>(this),  \
                    static_cast<void*>(parent));                        \
        }                                                               \
    }

void AbstractWindow::draw() {}

DUMP_CONT(ContainerWindow);
DUMP_CONT(RectangleWindow);
DUMP(AbstractButton);
DUMP(RectangleButton);
DUMP_CONT(Scrollbar);
DUMP(ScrollbarButton);
DUMP(Slider);
DUMP_CONT(ScrollbarBackground);
DUMP_CONT(Viewport);
DUMP_CONT(TextWindow);

void ScrollbarManager::dump(FILE* f) {
    fprintf(f,
            "node%p [shape=\"record\", label=\"{ ScrollbarManager | {Event mask | %s} | "
            "{Propagation mask | %s}}\"]\n",
            static_cast<void*>(this), uint64_to_bin(eventMask), uint64_to_bin(propagationMask));
    if (parent) {
        fprintf(f, "node%p -> node%p\n", static_cast<void*>(this), static_cast<void*>(parent));
    }
    for (auto child : children) {
        child->dump(f);
        fprintf(f, "node%p -> node%p\n", static_cast<void*>(this), static_cast<void*>(child));
    }

    if (horizontal) {
        horizontal->dump(f);
        fprintf(f, "node%p -> node%p\n", static_cast<void*>(this), static_cast<void*>(horizontal));
    }

    if (vertical) {
        vertical->dump(f);
        fprintf(f, "node%p -> node%p\n", static_cast<void*>(this), static_cast<void*>(vertical));
    }
}

// void ContainerWindow::dump(FILE *f) {
//     fprintf(f, "node%p [style=\"record\", label=\"{ ContainerWindow | {Event mask | } |
//     {Propagation mask | }}\"]\n", this);

//     if(parent) {
//         fprintf("node%p -> node%p\n", this, parent);
//     }

//     for(auto child : children) {
//         child->dump(f);
//         fprintf("node%p -> node%p\n", this, child);
//     }
// }

AbstractWindow::AbstractWindow() {
    parent = nullptr;
    eventMask = 0;
    propagationMask = 0;
}

AbstractWindow::~AbstractWindow() {
    if (parent) {
        // parent->detach(this);
        printf("%p is dying and detaching from parent %p\n", static_cast<void*>(this),
               static_cast<void*>(parent));
    } else {
        printf("Orphan window %p is dying\n", static_cast<void*>(this));
    }
}

void AbstractWindow::processEvent(Event ev) {
    if (!(ev.eventType & eventMask)) return;

    handleEvent(ev);
}

void AbstractWindow::attachToParent(AbstractWindow* parent) {
    this->parent = parent;
    parent->updatePropagationMask(eventMask | propagationMask);
}

void AbstractWindow::updatePropagationMask(uint64_t update) {
    propagationMask |= update;
    // eventMask |= update;

    if (parent) {
        parent->updatePropagationMask(update);
    }
}

void AbstractWindow::updateEventMask(uint64_t update) {
    eventMask |= update;
    // propagationMask |= update;

    if (parent) {
        parent->updatePropagationMask(update);
    }
}

void AbstractWindow::handleEvent(Event ev) {
    printf("Event type: %lx\nEvent type mask: %lx\n\n", ev.eventType, eventMask);
}

void AbstractWindow::detach() {
    if (!parent) return;

    static_cast<ContainerWindow*>(parent)->detachChild(this);
    parent = nullptr;
}

// ContainerWindow methods

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

void ContainerWindow::processEvent(Event ev) {
    if (ev.eventType & propagationMask) {
        for (auto child : children) {
            child->processEvent(ev);
        }
    }

    if (ev.eventType & eventMask) handleEvent(ev);
}

void ContainerWindow::attachChild(AbstractWindow* win) {
    children.push_back(win);
    win->attachToParent(this);
}

void ContainerWindow::detachChild(AbstractWindow* child) { children.remove(child); }

// AbstractButton methods
// void AbstractButton::attachToParent(AbstractWindow* parent) {
//     this->parent = parent;
//     // updateEventMask(EV_MOUSE_KEY_PRESS | EV_MOUSE_KEY_RELEASE | EV_MOUSE_MOVE);  // Need to
//     track key presses, releases & movements in order to properly update state
//     parent->updatePropagationMask(eventMask);
// }

AbstractButton::AbstractButton() {
    pressed = false;
    hovered = false;
    updateEventMask(EV_MOUSE_KEY_PRESS | EV_MOUSE_KEY_RELEASE | EV_MOUSE_MOVE);
}

void AbstractButton::handleEvent(Event ev) {
    if (IS_MOUSE_EV(ev)) {
        bool inside = isInside(ev.mouse.x, ev.mouse.y);

        if (ev.eventType == EV_MOUSE_KEY_PRESS && inside) {
            hovered = true;
            if (!pressed) {
                pressed = true;
                onButtonPress(ev);
            }
        } else if (ev.eventType == EV_MOUSE_KEY_RELEASE && pressed) {
            onButtonRelease(ev);
            if (inside) click(ev);
            pressed = false;
        } else if (ev.eventType == EV_MOUSE_MOVE) {
            if (inside) {
                if (!hovered) {
                    hovered = true;
                    onHoverEnter(ev);
                }
            } else {
                if (hovered) {
                    hovered = false;
                    onHoverExit(ev);
                }
            }
            onMouseMove(ev);
        }
    }
}

// Rectangle methods
void Rectangle::setPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

void Rectangle::setSize(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
}

void Rectangle::setThickness(float thickness) { this->thickness = thickness; }

void Rectangle::setBackgroundColor(const Color& color) { this->bkg = color; }

void Rectangle::setOutlineColor(const Color& color) { this->frg = color; }

bool Rectangle::isInsideRect(int x, int y) {
    int xstart = this->x;
    int xend = this->x + this->width;
    int ystart = this->y;
    int yend = this->y + this->height;

    // if (thickness > 0) {
    //     xstart -= thickness;
    //     ystart -= thickness;
    //     xend += thickness;
    //     yend += thickness;
    // }

    return (xstart <= x) && (ystart <= y) && (xend >= x) && (yend >= y);
}

int Rectangle::getWidth() { return width; }

int Rectangle::getHeight() { return height; }

// RectangleButton methods
void RectangleButton::draw() { RenderEngine::DrawRect(x, y, width, height, bkg, frg, thickness); }

void RectangleWindow::draw() {
    RenderEngine::DrawRect(x, y, width, height, bkg, frg, thickness);
    ContainerWindow::draw();
}

void RectangleButton::setHoverColor(const Color& color) { hoverBkg = color; }

void RectangleButton::setPressColor(const Color& color) { pressBkg = color; }

void RectangleButton::click(const Event&) {
    printf("Abstract button %p just got clicked\n", static_cast<void*>(this));
}

void RectangleButton::onHoverEnter(const Event&) { bkg = hoverBkg; }

void RectangleButton::onHoverExit(const Event&) { bkg = defaultBkg; }

void RectangleButton::onButtonPress(const Event&) { bkg = pressBkg; }

void RectangleButton::setBackgroundColor(const Color& color) {
    defaultBkg = color;
    bkg = color;
}

void RectangleButton::onButtonRelease(const Event&) {
    if(hovered) {
        bkg = hoverBkg;
    } else {
        bkg = defaultBkg;
    }
}

void RectangleButton::onMouseMove(const Event&) {}

bool RectangleButton::isInside(int x, int y) { return Rectangle::isInsideRect(x, y); }

// TexturedButton methods
TexturedButton::TexturedButton() : textureDescriptor(std::nullopt) {}

void TexturedButton::attachTexture(uint64_t descriptor) { textureDescriptor = descriptor; }

void TexturedButton::draw() {
    RectangleButton::draw();

    if (textureDescriptor.has_value()) {
        unsigned int shift = 3;
        if (thickness < 0) {
            shift += -thickness;
        }
        RenderEngine::DrawTexture(x + shift, y + shift, width - 2 * shift, height - 2 * shift,
                                  *textureDescriptor);
    }
}

// Slider methods
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

void Slider::setLimit(int limit) { this->limit = limit; }

void Slider::onButtonPress(const Event& ev) {
    if (isHorizontal) {
        strokeStart = ev.mouse.x;
        movementStart = x;
    } else {
        strokeStart = ev.mouse.y;
        movementStart = y;
    }
}

void Slider::onButtonRelease(const Event&) {}

void Slider::onMouseMove(const Event& ev) {
    if (pressed) {
        if (isHorizontal) {
            x = movementStart + ev.mouse.x - strokeStart;
        } else {
            y = movementStart + ev.mouse.y - strokeStart;
        }
    }
}

void Slider::handleEvent(Event ev) {
    if (IS_MOUSE_EV(ev)) {
        AbstractButton::handleEvent(ev);
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
                    x -= width;
                } else {
                    y -= height;
                }
                break;

            case Event::PG_DOWN:
                if (isHorizontal) {
                    x += width;
                } else {
                    y += height;
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

    // Event scrollEv;
    // scrollEv.eventType = EV_SCROLL;

    // Scrollbar* sParent = dynamic_cast<Scrollbar*>(parent);
    // sParent->handleEvent(scrollEv);
    // fprintf(stderr, "%p %p\n", static_cast<void*>(this), static_cast<void*>(sParent));
}

int Slider::getPositionAlongAxis() {
    if (isHorizontal) {
        return x;
    } else {
        return y;
    }
}

// Scrollbar methods
Scrollbar::Scrollbar(int length, bool isHorizontal) : isHorizontal(isHorizontal), length(length) {
    up = new ScrollbarButton(true);
    down = new ScrollbarButton(false);
    slider = new Slider(isHorizontal);
    bkg = new ScrollbarBackground(isHorizontal);

    attachChild(bkg);
    attachChild(up);
    attachChild(down);
    attachChild(slider);

    up->setSize(scrollbarElemSize, scrollbarElemSize);
    down->setSize(scrollbarElemSize, scrollbarElemSize);

    if (isHorizontal) {
        bkg->setSize(length, scrollbarElemSize);
    } else {
        bkg->setSize(scrollbarElemSize, length);
    }

    up->setThickness(-1);
    down->setThickness(-1);
    slider->setThickness(-1);
    bkg->setThickness(-1);

    eventMask |= EV_SCROLL;  // Don't really want to propagate subscription to scroll event
}

void Scrollbar::handleEvent(Event ev) {
    if (!parent) return;
    fprintf(stderr, "Scrollbar %p processing event of type %lu\n", static_cast<void*>(this),
            ev.eventType);
    if (ev.eventType == EV_SCROLL) {
        fprintf(stderr, "THAT IS A GODDAMN SCROLL EVENT!\n");
        if (isHorizontal) {
            ev.scroll.isHorizontal = true;
            ev.scroll.position =
                ((float)slider->x - slider->pivot) / (slider->limit + slider->width);

        } else {
            ev.scroll.isHorizontal = false;
            ev.scroll.position =
                ((float)slider->y - slider->pivot) / (slider->limit + slider->height);
            fprintf(stderr, "Scroll pos: %f\n", ev.scroll.position);
        }

        parent->processEvent(ev);
    }
}

void Scrollbar::setLength(int length) {
    this->length = length;
    if (isHorizontal) {
        bkg->setSize(length, scrollbarElemSize);
    } else {
        bkg->setSize(scrollbarElemSize, length);
    }
    setPosition(x, y);
}

void Scrollbar::setSliderSize(int size) {
    if (isHorizontal) {
        slider->setSize(size, scrollbarElemSize);

    } else {
        slider->setSize(scrollbarElemSize, size);
    }

    slider->setLimit(length - size);
}

void Scrollbar::setPosition(int x, int y) {
    this->x = x;
    this->y = y;

    up->setPosition(x, y);
    if (isHorizontal) {
        bkg->setPosition(x + scrollbarElemSize, y);
        slider->setPosition(x + scrollbarElemSize, y);
        down->setPosition(x + scrollbarElemSize + length, y);
    } else {
        bkg->setPosition(x, y + scrollbarElemSize);
        slider->setPosition(x, y + scrollbarElemSize);
        down->setPosition(x, y + scrollbarElemSize + length);
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

bool Scrollbar::isInsideSlider(int x, int y) { return slider->isInside(x, y); }

int Scrollbar::getSliderPositionAlongAxis() { return slider->getPositionAlongAxis(); }

int Scrollbar::getBkgLength() {
    if (isHorizontal) {
        return bkg->getWidth();
    } else {
        return bkg->getHeight();
    }
}

// Scrollbar button methods
ScrollbarButton::ScrollbarButton(bool isUp) : isUp(isUp) {
    updateEventMask(EV_MOUSE_KEY_PRESS | EV_MOUSE_KEY_RELEASE | EV_MOUSE_MOVE);
}

void ScrollbarButton::click(const Event&) {
    if (!parent) return;

    Event scrollEvent;
    scrollEvent.eventType = EV_SCROLL;

    if (isUp) {
        scrollEvent.scroll.scrollType = Event::UP;
    } else {
        scrollEvent.scroll.scrollType = Event::DOWN;
    }

    parent->processEvent(scrollEvent);
}

// ScrollbarBackground methods
ScrollbarBackground::ScrollbarBackground(bool isHorizontal) : isHorizontal(isHorizontal) {
    updateEventMask(EV_MOUSE_KEY_RELEASE);
}

void ScrollbarBackground::handleEvent(Event ev) {
    Scrollbar* p = static_cast<Scrollbar*>(parent);  // Believe me
    if (!parent) return;

    if (ev.eventType == EV_MOUSE_KEY_RELEASE && isInsideRect(ev.mouse.x, ev.mouse.y) &&
        !p->isInsideSlider(ev.mouse.x, ev.mouse.y)) {
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

// ScrollbarManager methods
ScrollbarManager::ScrollbarManager(bool horizontalScrollable, bool verticalScrollable) {
    eventMask |= EV_SCROLL;  // Want to process event; don't really want to propagate subscription
                             // since scroll events are going to be issued by a child
    propagationMask |= EV_SCROLL;
    if (horizontalScrollable) {
        horizontal = new Scrollbar(100, true);
        horizontal->attachToParent(this);
    }

    if (verticalScrollable) {
        vertical = new Scrollbar(100, false);
        vertical->attachToParent(this);
    }
}

void ScrollbarManager::processEvent(Event ev) {
    fprintf(stderr, "Issuing event of type %lu\n", ev.eventType);
    if (ev.eventType != EV_SCROLL && ev.eventType & propagationMask) {
        if (horizontal) {
            horizontal->processEvent(ev);
        }

        if (vertical) {
            vertical->processEvent(ev);
        }
    }

    if (ev.eventType & propagationMask) {
        for (auto child : children) {
            child->processEvent(ev);
        }
    }
}

// TODO Turn this goddamn scrollbar width into a parameter, not a fucking magic number

void ScrollbarManager::adjustScrollbarSize(int x, int y, int width, int height) {
    adjWidth = width;
    adjHeight = height;

    if (horizontal) {
        horizontal->setPosition(x, y + height);
        horizontal->setLength(width - 2 * scrollbarElemSize);
    }

    if (vertical) {
        vertical->setPosition(x + width, y);
        vertical->setLength(height - 2 * scrollbarElemSize);
    }
}

void ScrollbarManager::adjustScrollableAreaSize(int width, int height) {
    if (horizontal) {
        horizontal->setSliderSize(static_cast<double>(adjWidth) /
                                  (width)*horizontal->getBkgLength());
    }

    if (vertical) {
        vertical->setSliderSize(static_cast<double>(adjHeight) / (height)*vertical->getBkgLength());
    }
}

void ScrollbarManager::draw() {
    if (horizontal) horizontal->draw();

    if (vertical) vertical->draw();

    for (auto child : children) {
        child->draw();
    }
}

// TextWindow methods
TextWindow::TextWindow() : characterSize(30) { content = nullptr; }

void TextWindow::setCharSize(int size) { characterSize = size; }

void TextWindow::setText(const wchar_t* newContent) { content = newContent; }

void TextWindow::draw() { RenderEngine::DrawText(x, y, content, characterSize); }

// Vector2:
template <typename T>
Vector2<T>::Vector2() : x(0), y(0) {}

template <typename T>
Vector2<T>::Vector2(const T& x, const T& y) : x(x), y(y) {}

template struct Vector2<int>;
// Viewport container

void Viewport::setPosition(const Vector2<int>& pos) { this->position = pos; }

void Viewport::setSpan(const Vector2<int>& span) { this->span = span; }

void Viewport::setSize(const Vector2<int>& size) { this->size = size; }

void Viewport::handleEvent(Event ev) {
    if (ev.eventType == EV_SCROLL) {
        if (ev.scroll.isHorizontal) {
            viewPosition.x = ev.scroll.position * span.x;
        } else {
            viewPosition.y = ev.scroll.position * span.y;
        }
    }
}

void Viewport::draw() {
    RenderEngine::InitOffScreen(size.x, size.y);
    RenderEngine::pushGlobalOffset(viewPosition.x, viewPosition.y);
    ContainerWindow::draw();
    RenderEngine::popGlobalOffset();
    RenderEngine::FlushOffScreen(position.x, position.y);
}

ModalWindowManager::ModalWindowManager() : currentModal(nullptr), invoked(false) {}

void ModalWindowManager::processEvent(Event ev) {
    if (invoked) {
        currentModal->processEvent(ev);
    } else {
        ContainerWindow::processEvent(ev);
    }
}

void ModalWindowManager::draw() {
    ContainerWindow::draw();

    if (invoked) {
        currentModal->draw();
    }
}

void ModalWindowManager::invokeModalWindow(ModalWindow* modal) {
    currentModal = modal;
    invoked = true;
}

void ModalWindowManager::deinvoke() { invoked = false; }

void ModalWindow::finish() {
    if (parent) {
        static_cast<ModalWindowManager*>(parent)->deinvoke();
    }
}
// P. S.: it's a mess, destroy this abomination