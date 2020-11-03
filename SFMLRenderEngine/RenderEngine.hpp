#ifndef RENDERENGINE_HPP_
#define RENDERENGINE_HPP_
#include <SFML/Graphics.hpp>

#include "../Color.hpp"
#include "../Event.hpp"

class RenderEngine {
   public:
    static void Init(unsigned int width, unsigned int height);  // Initialization
    static void Finalize();                                     // Finalization
    static bool Run();                                          // Do one loop iteration
    static void Clear();                                        // Function that clears window
    static void Display();                                      // Function that displays new content on window
    static bool PollEvent(Event& ev);                           // Event polling
    static void DrawRect(int x, int y,
                         unsigned int width, unsigned int height,
                         Color bkgColor, Color frgColor, float thickness);  // Draw rectangle

   private:
    static bool TranslateEvent(sf::Event sfmlEv, Event& ev);                    // Translate SFML event into own event type
    static Event::MOUSE_BUTTON TranslateMouseButton(sf::Mouse::Button button);  // Translate SFML mouse key identifier to own event system
    static sf::RenderWindow mainWindow;                                         // System window for displaying anything
    RenderEngine();                                                             // Private constructor ensures that class is a singletone indeed
};

using OffScreenRenderTarget = sf::RenderTexture;
#endif  // RENDERENGINE_HPP_