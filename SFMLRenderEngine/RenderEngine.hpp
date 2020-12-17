#ifndef RENDERENGINE_HPP_
#define RENDERENGINE_HPP_
#include <SFML/Graphics.hpp>
#include <stack>

#include "../Color.hpp"
#include "../Event.hpp"

class RenderEngine {
   public:
    static void Init(unsigned int width, unsigned int height);  // Initialization
    static void Finalize();                                     // Finalization
    static bool Run();                                          // Do one loop iteration
    static void Clear();                                        // Function that clears window
    static void Display();             // Function that displays new content on window
    static bool PollEvent(Event& ev);  // Event polling
    static void DrawRect(int x, int y, unsigned int width, unsigned int height, Color bkgColor,
                         Color frgColor, float thickness);                       // Draw rectangle
    static void DrawTexture(int x, int y, unsigned int width, unsigned int height, uint64_t texture_descriptor); // Draw texture
    static void DrawText(int x, int y, const wchar_t* text, int characterSize);  // Draw text
    static void InitOffScreen(
        unsigned int width, unsigned int height);  // Initialize new target for off-screen rendering
    static void FlushOffScreen(int x, int y);      // Render off-screen buffer at a certain position
    static void DrawBitmap(int x, int y, uint32_t width, uint32_t height,
                           uint32_t* data);      // Draw array of pixels
    static void pushGlobalOffset(int x, int y);  // Push offset settings on the stack
    static void pushRelGlobalOffset(int x, int y);
    static uint64_t LoadTexture(const char *texture); // Load texture and return its descriptor 
    static int getGlobalXOffset();
    static int getGlobalYOffset();
    static void popGlobalOffset();  // Pop offset settings
    static void SaveToImage(const wchar_t *path, uint32_t *img, unsigned int width, unsigned int height);
    static std::tuple<unsigned int, unsigned int, uint32_t *> LoadFromImage(const wchar_t *path);
    // static void RenderToMain(); // Set current target to mainWindow
    // static void SetRenderTarget(RenderTarget* target); // Set current render target

   private:
    static bool TranslateEvent(sf::Event sfmlEv,
                               Event& ev);  // Translate SFML event into own event type
    static Event::MOUSE_BUTTON TranslateMouseButton(
        sf::Mouse::Button button);  // Translate SFML mouse key identifier to own event system
    static std::stack<sf::Vector2i> globalOffsets;  // Global drawing offset
    static std::stack<sf::RenderTarget*>
        targets;  // Stack of off-screen targets for nested viewports and such
    static std::vector<sf::Texture> textures;
    static sf::RenderWindow mainWindow;  // System window for displaying anything
    static sf::Font defaultFont;         // Default text font
    RenderEngine();  // Private constructor ensures that class is a singletone indeed
};
#endif  // RENDERENGINE_HPP_