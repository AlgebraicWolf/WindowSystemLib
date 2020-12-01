#ifndef GRAPHIC_EDITOR_HPP_
#define GRAPHIC_EDITOR_HPP_
#include <cstdint>

#include "../WindowSystem/Window.hpp"

// Class that handles all the drawing activities
class DrawingManager {

};

// Canvas is a renderable array array of pixels that supports drawing on it
class Canvas : public RectangleWindow {
   public:
    Canvas(uint32_t width, uint32_t height);
    ~Canvas();
    uint32_t *getData();
    virtual void draw() override;

   private:
    uint32_t *data;
    uint32_t prev_x;
    uint32_t prev_y;
    bool pressed;
    virtual void handleEvent(Event ev) override;
};

// Tool interface
class AbstractTool : public AbstractWindow {
    virtual void startApplication() = 0;
    virtual void endApplication() = 0;
    virtual void apply() = 0;
};


// Manager for tools that handles boring stuff as well a selection of a certain tool
class ToolManager : public ContainerWindow {

};

// Thing for selecting colors 
class ColorPicker {
};


#endif  // GRAPHIC_EDITOR_HPP_