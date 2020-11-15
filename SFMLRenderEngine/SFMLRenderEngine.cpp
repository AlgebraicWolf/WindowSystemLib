#include <SFML/Graphics.hpp>

#include "RenderEngine.hpp"

sf::RenderWindow RenderEngine::mainWindow;
sf::RenderTarget* RenderEngine::currentTarget;
sf::RenderTexture RenderEngine::offScreenTarget;
sf::Font RenderEngine::defaultFont;

void RenderEngine::Init(unsigned int width, unsigned int height) {
    mainWindow.create(sf::VideoMode(width, height), "My window system", sf::Style::None);
    if(!defaultFont.loadFromFile("default.ttf")) {
        printf("Unable to load font\n");
        exit(-1);
    }
    currentTarget = &mainWindow;
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

void RenderEngine::DrawRect(int x, int y,
                            unsigned int width, unsigned int height,
                            Color bkgColor, Color frgColor, float thickness) {
    sf::RectangleShape rect(sf::Vector2f(width, height));
    rect.setPosition(sf::Vector2f(x, y));
    rect.setFillColor(sf::Color(bkgColor.red, bkgColor.green, bkgColor.blue, bkgColor.alpha));
    rect.setOutlineColor(sf::Color(frgColor.red, frgColor.green, frgColor.blue, frgColor.alpha));
    rect.setOutlineThickness(thickness);
    mainWindow.draw(rect);
}

void RenderEngine::DrawText(int x, int y, const wchar_t* text) {
    sf::Text txt(text, defaultFont);
    txt.setPosition(x, y);
    txt.setFillColor(sf::Color::White);
    txt.setFont(defaultFont);
    currentTarget->draw(txt);
}

void RenderEngine::InitOffScreen(unsigned int width, unsigned int height) {
    // printf("Initializing off-screen buffer of size(%u, %u)\n", width, height);
    offScreenTarget.create(width,  height);
    offScreenTarget.clear();

    // sf::VertexArray lines(sf::Triangles, 3);
    // lines[0].position = sf::Vector2f(100, 0);
    // lines[1].position = sf::Vector2f(200, 0);
    // lines[2].position = sf::Vector2f(150, 100);
    // offScreenTarget.draw(lines);

    currentTarget = &offScreenTarget;
}

void RenderEngine::FlushOffScreen(int x, int y) {
    offScreenTarget.display();
    // printf("Flushin off-screen buffer at coordinates (%d, %d)\n", x, y);
    sf::Sprite offScreenTargetSprite(offScreenTarget.getTexture());
    offScreenTargetSprite.setPosition(x, y);
    mainWindow.draw(offScreenTargetSprite);

    currentTarget = &mainWindow;
}
