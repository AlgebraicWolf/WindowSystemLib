#include "GraphicEditor.hpp"

#include <inttypes.h>

#include <cstring>

#include "../ColorConverter.hpp"
#include "../SFMLRenderEngine/RenderEngine.hpp"

static constexpr uint32_t brush_radius = 5;

Canvas::Canvas(uint32_t width, uint32_t height) {
    setSize(width, height);
    pressed = false;

    data = new uint32_t[width * height];
    memset(data, 0xFF, width * height * 4);
    updateEventMask(EV_MOUSE_MOVE | EV_MOUSE_KEY_PRESS | EV_MOUSE_KEY_RELEASE);
}

Canvas::~Canvas() {
    delete[] data;
}

uint32_t *Canvas::getData() {
    return data;
}

uint32_t Canvas::getWidth() {
    return width;
}

uint32_t Canvas::getHeight() {
    return height;
}

void Canvas::draw() {
    RenderEngine::DrawBitmap(x, y, width, height, data);
}

void Canvas::handleEvent(Event ev) {
    // ToolManager *manager = static_cast<DrawingManager *>(parent)->getToolManager();
    uint32_t relX = ev.mouse.x - x;
    uint32_t relY = ev.mouse.y - y;

    if (ev.eventType == EV_MOUSE_KEY_PRESS && isInsideRect(ev.mouse.x, ev.mouse.y)) {
        pressed = true;
        static_cast<DrawingManager *>(parent)->startToolApplication(relX, relY);
    } else if (ev.eventType == EV_MOUSE_KEY_RELEASE) {
        pressed = false;
        static_cast<DrawingManager *>(parent)->endToolApplication(relX, relY);
    }

    // fprintf(stderr, "X: %d, Y: %d, inside: %s\n", ev.mouse.x, ev.mouse.y, isInsideRect(ev.mouse.x, ev.mouse.y) ? "True" : "False");

    if (pressed && isInsideRect(ev.mouse.x, ev.mouse.y)) {
        static_cast<DrawingManager *>(parent)->applyTool(relX, relY);
    }
}

DrawingManager::DrawingManager() {
    toolManager = new ToolManager;
    Brush *brush = new Brush;
    brush->setColor(HSVtoHEX(0, 100, 100));
    // fprintf(stderr, "Current color: %x\n", HSVtoHEX(0, 100, 100));
    Brush *eraser = new Brush;
    colorPicker = new ColorPicker();

    eraser->setColor(0xFFFFFFFF);
    canvas = nullptr;

    toolManager->setPosition(2, 100);
    toolManager->setSize(60, 400);
    toolManager->setThickness(2);
    toolManager->setBackgroundColor({0, 0, 0, 0});
    toolManager->setOutlineColor({255, 255, 255, 255});

    toolManager->attachTool(brush);
    toolManager->attachTool(eraser);

    attachChild(colorPicker);
    attachChild(toolManager);
}

void DrawingManager::createCanvas(uint32_t width, uint32_t height) {
    delete canvas;
    canvas = new Canvas(width, height);

    canvas->setPosition(200, 50);

    attachChild(canvas);
}

void DrawingManager::startToolApplication(uint32_t x, uint32_t y) {
    toolManager->getActiveTool()->startApplication(*canvas, x, y, frgColor, bkgColor);
}

void DrawingManager::endToolApplication(uint32_t x, uint32_t y) {
    toolManager->getActiveTool()->endApplication(*canvas, x, y);
}

void DrawingManager::applyTool(uint32_t x, uint32_t y) {
    toolManager->getActiveTool()->apply(*canvas, x, y);
}

void DrawingManager::updateActiveColor(uint32_t color) {
    frgColor = color;
    fprintf(stderr, "New active color value is %x" PRIu32 "\n", color);
}

ToolManager *DrawingManager::getToolManager() {
    return toolManager;
}

ToolManager::ToolManager() {
    activeTool = nullptr;
}

void ToolManager::attachTool(AbstractTool *tool) {
    fprintf(stderr, "Setting position (%d, %d)", x, y);
    tool->setPosition(x + 5, y + 5 + 60 * children.size());

    attachChild(tool);

    if (!activeTool)
        setActiveTool(tool);
}

void ToolManager::setActiveTool(AbstractTool *tool) {
    if (activeTool) activeTool->deactivate();
    tool->activate();
    activeTool = tool;
}

AbstractTool *ToolManager::getActiveTool() {
    return activeTool;
}

HSVSlider::HSVSlider(uint32_t width, uint32_t height) {
    setSize(width, height);

    rainbowBkg = new uint32_t[width * height];

    setThickness(2);
    setBackgroundColor({0, 0, 0, 0});
    setHoverColor({0, 0, 0, 0});
    setPressColor({0, 0, 0, 0});
    setOutlineColor({255, 255, 255, 255});

    for (uint32_t y = 0; y < height; y++) {
        uint32_t color = HSVtoHEX(y * 360 / height, 100, 100);
        for (uint64_t x = 0; x < width; x++) {
            rainbowBkg[y * width + x] = color;
        }
    }
}

void HSVSlider::draw() {
    RenderEngine::DrawBitmap(x, y, width, height, rainbowBkg);
    RectangleButton::draw();
}

void HSVSlider::click(const Event &ev) {
    uint16_t new_hue = (ev.mouse.y - y) * 360 / height;
    fprintf(stderr, "New hue value: %" PRIu16 "\n", new_hue);
    static_cast<ColorPicker *>(parent)->updateHue(new_hue);
}

void HSVSlider::onMouseMove(const Event &ev) {
    if (pressed && isInside(ev.mouse.x, ev.mouse.y)) {
        uint16_t new_hue = (ev.mouse.y - y) * 360 / height;
        fprintf(stderr, "New hue value: %" PRIu16 "\n", new_hue);
        static_cast<ColorPicker *>(parent)->updateHue(new_hue);
    }
}

ColorPicker::ColorPicker() {
    hue = new HSVSlider(30, 150);
    hue->setPosition(1500, 200);

    H = 0;
    S = 100;
    V = 100;

    attachChild(hue);
}

void ColorPicker::updateHue(uint16_t hue) {
    if (H == hue) return;

    H = hue;

    static_cast<DrawingManager *>(parent)->updateActiveColor(HSVtoHEX(H, S, V));
    // TODO redraw SV selector
}

void ColorPicker::updateSV(uint8_t, uint8_t) {
}

AbstractTool::AbstractTool() {
    setSize(50, 50);
    setThickness(-2);
    setBackgroundColor({0, 0, 0, 0});
    setHoverColor({100, 100, 100, 255});
    setOutlineColor({255, 255, 255, 255});
    setPressColor({255, 255, 255, 255});
}

void AbstractTool::click(const Event &) {
    // fprintf(stderr, "%p was clicked\n", static_cast<void *>(this));
    if (parent) {
        static_cast<ToolManager *>(parent)->setActiveTool(this);
    }
}

void AbstractTool::activate() {
    setBackgroundColor({255, 255, 255, 255});
    setHoverColor({255, 255, 255, 255});
}

void AbstractTool::deactivate() {
    setBackgroundColor({0, 0, 0, 0});
    setHoverColor({100, 100, 100, 255});
}

void Brush::startApplication(Canvas &, uint32_t x, uint32_t y, uint32_t frgColor, uint32_t) {
    prev_x = x;
    prev_y = y;
    color = frgColor;
}

void Brush::endApplication(Canvas &, uint32_t, uint32_t) {}

void Brush::apply(Canvas &canvas, uint32_t x, uint32_t y) {
    uint32_t *data = canvas.getData();
    uint32_t width = canvas.getWidth();
    uint32_t height = canvas.getHeight();

    int32_t x0 = prev_x;
    int32_t x1 = x;
    int32_t y0 = prev_y;
    int32_t y1 = y;

    int32_t delta_y = y1 - y0;
    int32_t delta_x = x1 - x0;

    uint32_t num_steps = (1 + std::max(std::abs(delta_x), std::abs(delta_y)));

    double x_step = static_cast<double>(delta_x) / num_steps;
    double y_step = static_cast<double>(delta_y) / num_steps;

    for (uint32_t i = 0; i < num_steps; i++) {
        double xt = x0 + x_step * i;
        double yt = y0 + y_step * i;

        double x_from = (xt >= brush_radius) ? xt - brush_radius : 0;
        double y_from = (yt >= brush_radius) ? yt - brush_radius : 0;

        double x_to = std::min(xt + brush_radius, static_cast<double>(width));
        double y_to = std::min(yt + brush_radius, static_cast<double>(height));

        for (uint32_t x_cur = x_from; x_cur < x_to; x_cur++) {
            for (uint32_t y_cur = y_from; y_cur < y_to; y_cur++) {
                if ((xt - x_cur) * (xt - x_cur) + (yt - y_cur) * (yt - y_cur) <= brush_radius * brush_radius) {
                    data[static_cast<uint32_t>(y_cur) * width + static_cast<uint32_t>(x_cur)] = color;
                }
            }
        }
    }

    prev_x = x;
    prev_y = y;
}

void Brush::setColor(uint32_t color) {
    this->color = color;
}