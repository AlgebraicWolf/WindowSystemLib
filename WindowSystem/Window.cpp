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