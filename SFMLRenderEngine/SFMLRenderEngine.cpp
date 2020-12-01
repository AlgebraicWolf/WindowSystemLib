#include <SFML/Graphics.hpp>

#include "RenderEngine.hpp"

sf::RenderWindow RenderEngine::mainWindow;
sf::Font RenderEngine::defaultFont;
std::stack<sf::Vector2i> RenderEngine::globalOffsets;
std::stack<sf::RenderTarget *> RenderEngine::targets;

void RenderEngine::Init(unsigned int width, unsigned int height) {
    mainWindow.create(sf::VideoMode(width, height), "My window system", sf::Style::None);
    if (!defaultFont.loadFromFile("default.ttf")) {
        printf("Unable to load font\n");
        exit(-1);
    }

    targets.push(&mainWindow);
    pushGlobalOffset(0, 0);
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

bool RenderEngine::PollEvent(Event &ev) {
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

bool RenderEngine::TranslateEvent(sf::Event sfmlEv, Event &ev) {
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
    rect.setPosition(x - globalOffsets.top().x, y - globalOffsets.top().y);
    rect.setFillColor(sf::Color(bkgColor.red, bkgColor.green, bkgColor.blue, bkgColor.alpha));
    rect.setOutlineColor(sf::Color(frgColor.red, frgColor.green, frgColor.blue, frgColor.alpha));
    rect.setOutlineThickness(thickness);
    targets.top()->draw(rect);
}

void RenderEngine::DrawText(int x, int y, const wchar_t *text) {
    sf::Text txt(text, defaultFont);
    txt.setPosition(x - globalOffsets.top().x, y - globalOffsets.top().y);
    txt.setFillColor(sf::Color::White);
    txt.setFont(defaultFont);
    targets.top()->draw(txt);
}

void RenderEngine::InitOffScreen(unsigned int width, unsigned int height) {
    sf::RenderTexture *offScreen = new sf::RenderTexture();
    offScreen->create(width, height);
    offScreen->clear();
    targets.push(offScreen);
}

void RenderEngine::FlushOffScreen(int x, int y) {
    // offScreenTarget.display();
    sf::RenderTexture *current = static_cast<sf::RenderTexture *>(targets.top());
    targets.pop();
    current->display();
    // printf("Flushin off-screen buffer at coordinates (%d, %d)\n", x, y);
    sf::Sprite offScreenTargetSprite(current->getTexture());
    offScreenTargetSprite.setPosition(x - globalOffsets.top().x, y - globalOffsets.top().y);
    targets.top()->draw(offScreenTargetSprite);
    delete current;
    // currentTarget = &mainWindow;
}

void RenderEngine::popGlobalOffset() {
    globalOffsets.pop();
}

void RenderEngine::pushGlobalOffset(int x, int y) {
    globalOffsets.emplace(x, y);
}

void RenderEngine::DrawBitmap(int x, int y, uint32_t width, uint32_t height, uint32_t* data) {
    sf::Texture texture;
    texture.create(width, height);
    texture.update(reinterpret_cast<uint8_t *>(data));
    sf::Sprite bitmap_sprite(texture);
    bitmap_sprite.setPosition(x, y);
    mainWindow.draw(bitmap_sprite);
}
