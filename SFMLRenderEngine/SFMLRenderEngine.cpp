#include <SFML/Graphics.hpp>

#include "RenderEngine.hpp"

sf::RenderWindow RenderEngine::mainWindow;

void RenderEngine::Init(unsigned int width, unsigned int height) {
    mainWindow.create(sf::VideoMode(width, height), "My window system", sf::Style::None);
}

void RenderEngine::Finalize() {
    if (!mainWindow.isOpen()) {
        mainWindow.close();
    }
}

void RenderEngine::Clear() {
    mainWindow.clear();
}

void RenderEngine::Display() {
    mainWindow.display();
}

bool RenderEngine::Run() {
    return 0;
}

bool RenderEngine::PollEvent(Event& ev) {
    sf::Event sfmlEv;
    while (mainWindow.pollEvent(sfmlEv)) {
        if (TranslateEvent(sfmlEv, ev))
            return true;
    }

    return false;
}

Event::MOUSE_BUTTON RenderEngine::TranslateMouseButton(sf::Mouse::Button button) {
    switch (button) {
        case sf::Mouse::Left:
            return Event::LEFT;

        case sf::Mouse::Right:
            return Event::RIGHT;

        case sf::Mouse::Middle:
            return Event::MIDDLE;

        default:
            return Event::NONE;
    }
}

bool RenderEngine::TranslateEvent(sf::Event sfmlEv, Event& ev) {
    switch (sfmlEv.type) {
        case sf::Event::Closed:
            ev.eventType = EV_CLOSED;
            break;

        case sf::Event::MouseButtonPressed:
            ev.eventType = EV_MOUSE_KEY_PRESS;
            ev.mouse.x = sfmlEv.mouseButton.x;
            ev.mouse.y = sfmlEv.mouseButton.y;
            ev.mouse.button = TranslateMouseButton(sfmlEv.mouseButton.button);
            break;

        case sf::Event::MouseButtonReleased:
            ev.eventType = EV_MOUSE_KEY_RELEASE;
            ev.mouse.x = sfmlEv.mouseButton.x;
            ev.mouse.y = sfmlEv.mouseButton.y;
            ev.mouse.button = TranslateMouseButton(sfmlEv.mouseButton.button);
            break;

        case sf::Event::MouseMoved:
            ev.eventType = EV_MOUSE_MOVE;
            ev.mouse.x = sfmlEv.mouseMove.x;
            ev.mouse.y = sfmlEv.mouseMove.y;
            ev.mouse.button = Event::NONE;
            break;

        case sf::Event::KeyPressed:
            ev.eventType = EV_KEYBOARD_PRESS;
            ev.keyboard.keyCode = sfmlEv.key.code;
            break;

        case sf::Event::KeyReleased:
            ev.eventType = EV_KEYBOARD_RELEASE;
            ev.keyboard.keyCode = sfmlEv.key.code;
            break;

        default:
            return false;
    }

    return true;
}

void RenderEngine::DrawRect(unsigned int x, unsigned int y,
                            unsigned int width, unsigned int height,
                            Color bkgColor, Color frgColor, float thickness) {
    sf::RectangleShape rect(sf::Vector2f(width, height));
    rect.setPosition(sf::Vector2f(x, y));
    rect.setFillColor(sf::Color(bkgColor.red, bkgColor.green, bkgColor.blue, bkgColor.alpha));
    rect.setOutlineColor(sf::Color(frgColor.red, frgColor.green, frgColor.blue, frgColor.alpha));
    rect.setOutlineThickness(thickness);
    mainWindow.draw(rect);
}