#ifndef GRAPHIC_EDITOR_HPP_
#define GRAPHIC_EDITOR_HPP_
#include <cstdint>

#include "../WindowSystem/Window.hpp"

class Canvas : public AbstractWindow {
   public:
    Canvas(uint32_t width, uint32_t height);
    ~Canvas();
    uint32_t *getData();
    virtual void draw() override;

   private:
    uint32_t data;
    uint32_t width;
    uint32_t height;
};

class AbstractTool : public AbstractWindow {
    virtual void startApplication() = 0;
    virtual void endApplication() = 0;
    virtual void apply();
};

class ToolManager : public ContainerWindow {

};

class ColorPicker {
};

#endif  // GRAPHIC_EDITOR_HPP_