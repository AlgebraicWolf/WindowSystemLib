#ifndef GRAPHIC_EDITOR_HPP_
#define GRAPHIC_EDITOR_HPP_
#include <cstdint>

#include "../WindowSystem/Window.hpp"

// Canvas is a renderable array array of pixels that supports drawing on it
class Canvas : public RectangleWindow {
   public:
    Canvas(uint32_t width, uint32_t height);
    ~Canvas();
    uint32_t *getData();
    uint32_t getWidth();
    uint32_t getHeight();
    virtual void draw() override;

   private:
    uint32_t *data;
    // uint32_t prev_x;
    // uint32_t prev_y;
    bool pressed;
    virtual void handleEvent(Event ev) override;
};

// Tool interface
class AbstractTool : public RectangleButton {
   public:
    AbstractTool();
    virtual void click(const Event &ev) override;

    virtual void startApplication(Canvas &canvas, uint32_t x, uint32_t y, uint32_t frgColor, uint32_t bkgColor) = 0;
    virtual void endApplication(Canvas &canvas, uint32_t x, uint32_t y) = 0;
    virtual void apply(Canvas &canvas, uint32_t x, uint32_t y) = 0;
    void deactivate();
    void activate();
};

// Manager for tools that handles boring stuff as well a selection of a certain tool
class ToolManager : public RectangleWindow {
   public:
    ToolManager();
    void setActiveTool(AbstractTool *tool);
    void attachTool(AbstractTool *tool);
    AbstractTool *getActiveTool();

   private:
    AbstractTool *activeTool;
};

class HSVSlider : public RectangleButton {
   public:
    HSVSlider(uint32_t width, uint32_t height);
    virtual void draw() override;

   private:
    virtual void onMouseMove(const Event &ev) override;
    virtual void click(const Event &ev) override;
    //    virtual void handleEvent(Event ev) override;
    uint32_t *rainbowBkg;
};

class HSVFader : public RectangleButton {
   public:
    HSVFader(uint32_t width, uint32_t height);
    
    void updateHue(uint16_t hue);
    virtual void draw() override;

   private:
    virtual void onMouseMove(const Event &ev) override;
    virtual void click(const Event &ev) override;
    uint32_t *SVBkg;
};

// Thing for selecting colors
class ColorPicker : public RectangleWindow {
   public:
    ColorPicker();
    void updateHue(uint16_t H);
    void updateSV(uint8_t S, uint8_t V);

   private:
    uint16_t H;
    uint8_t S;
    uint8_t V;
    HSVSlider *hue;
};

// Class that handles all the drawing activities
class DrawingManager : public ContainerWindow {
   public:
    DrawingManager();

    void createCanvas(uint32_t width, uint32_t height);
    ToolManager *getToolManager();
    void startToolApplication(uint32_t x, uint32_t y);
    void endToolApplication(uint32_t x, uint32_t y);
    void applyTool(uint32_t x, uint32_t y);
    void updateActiveColor(uint32_t color);

    // ~DrawingManager();

   private:
    uint32_t bkgColor;
    uint32_t frgColor;

    ToolManager *toolManager;
    Canvas *canvas;
    ColorPicker *colorPicker;
};

// There go tools
class Brush : public AbstractTool {
   public:
    virtual void startApplication(Canvas &canvas, uint32_t x, uint32_t y, uint32_t frgColor, uint32_t bkgColor) override;
    virtual void endApplication(Canvas &canvas, uint32_t x, uint32_t y) override;
    virtual void apply(Canvas &canvas, uint32_t x, uint32_t y) override;
    void setColor(uint32_t color);

   private:
    uint32_t color;
    uint32_t prev_x;
    uint32_t prev_y;
};

#endif  // GRAPHIC_EDITOR_HPP_