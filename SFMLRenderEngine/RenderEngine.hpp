#ifndef RENDERENGINE_HPP_
#define RENDERENGINE_HPP_
#include <SFML/Graphics.hpp>

#include "../Color.hpp"
#include "../Event.hpp"

using OffScreenRenderTarget = sf::RenderTexture;
using RenderTarget = sf::RenderTarget;

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
    static void DrawText(int x, int y, const char *text);
    static void DrawRenderTarget(int x, int y, const OffScreenRenderTarget& from);

    static void RenderToMain(); // Set current target to mainWindow
    static void SetRenderTarget(RenderTarget* target); // Set current render target

   private:
    static bool TranslateEvent(sf::Event sfmlEv, Event& ev);                    // Translate SFML event into own event type
    static Event::MOUSE_BUTTON TranslateMouseButton(sf::Mouse::Button button);  // Translate SFML mouse key identifier to own event system
    static sf::RenderTarget *currentTarget;                                     // Current render target (for off-screen rendering)
    static sf::RenderWindow mainWindow;                                         // System window for displaying anything
    static sf::Font defaultFont;                                                // Default text font
    RenderEngine();                                                             // Private constructor ensures that class is a singletone indeed
};
#endif  // RENDERENGINE_HPP_