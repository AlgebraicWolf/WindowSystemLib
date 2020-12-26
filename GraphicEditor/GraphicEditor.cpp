#include "GraphicEditor.hpp"

#include <dlfcn.h>
#include <inttypes.h>

#include <concepts>
#include <cstring>
#include <filesystem>

#include "../ColorConverter.hpp"
#include "../SFMLRenderEngine/RenderEngine.hpp"

Canvas *current_canvas = nullptr;
DrawingManager *dm = nullptr;

constexpr int32_t MAX_THICKNESS = 100;

Color from_hex(uint32_t clr) {
    Color c;
    c.red = clr;
    c.green = clr >> 8;
    c.blue = clr >> 16;
    c.alpha = clr >> 24;
    // fprintf(stderr, "Red: %u, Green: %u, Blue: %u, Alpha: %u", c.red, c.green, c.blue, c.alpha);
    return c;
}

Canvas::Canvas(uint32_t width, uint32_t height) {
    setSize(width, height);
    pressed = false;

    data = new uint32_t[width * height];
    memset(data, 0xFF, width * height * 4);
    updateEventMask(EV_MOUSE_MOVE | EV_MOUSE_KEY_PRESS | EV_MOUSE_KEY_RELEASE);
}

Canvas::~Canvas() { delete[] data; }

uint32_t *Canvas::getData() { return data; }

uint32_t Canvas::getWidth() { return width; }

uint32_t Canvas::getHeight() { return height; }

void Canvas::draw() { RenderEngine::DrawBitmap(x, y, width, height, data); }

void Canvas::handleEvent(Event ev) {
    // ToolManager *manager = static_cast<DrawingManager *>(parent)->getToolManager();
    uint32_t relX = ev.mouse.x - x;
    uint32_t relY = ev.mouse.y - y;

    if (ev.eventType == EV_MOUSE_KEY_PRESS && isInsideRect(ev.mouse.x, ev.mouse.y)) {
        pressed = true;
        static_cast<DrawingManager *>(parent)->startToolApplication(relX, relY);
    } else if (ev.eventType == EV_MOUSE_KEY_RELEASE && pressed) {
        pressed = false;
        static_cast<DrawingManager *>(parent)->endToolApplication(relX, relY);
    }

    // fprintf(stderr, "X: %d, Y: %d, inside: %s\n", ev.mouse.x, ev.mouse.y,
    // isInsideRect(ev.mouse.x, ev.mouse.y) ? "True" : "False");

    if (pressed && isInsideRect(ev.mouse.x, ev.mouse.y)) {
        static_cast<DrawingManager *>(parent)->applyTool(relX, relY);
    }
}

void Canvas::emplace(uint32_t width, uint32_t height, uint32_t *data) {
    delete[] this->data;
    this->width = width;
    this->height = height;
    this->data = data;
}

DrawingManager::DrawingManager() {
    toolManager = new ToolManager;
    colorPicker = new ColorPicker();
    settingsContainer = new SettingsContainer;

    ModalInvokerButton *load_button = new ModalInvokerButton;
    ModalInvokerButton *save_button = new ModalInvokerButton;

    load_button->setPosition(10, 10);
    save_button->setPosition(70, 10);

    load_button->attachTexture(RenderEngine::LoadTexture("img/open.png"));
    save_button->attachTexture(RenderEngine::LoadTexture("img/save.png"));

    load_button->attachModal(new LoadDialog);
    save_button->attachModal(new SaveDialog);

    attachChild(load_button);
    attachChild(save_button);

    attachChild(toolManager);
    attachChild(colorPicker);
    attachChild(settingsContainer);

    Brush *brush = new Brush;
    Eraser *eraser = new Eraser;

    toolManager->setPosition(2, 100);
    toolManager->setSize(60, 400);
    toolManager->setThickness(2);
    toolManager->setBackgroundColor({0, 0, 0, 0});
    toolManager->setOutlineColor({255, 255, 255, 255});

    toolManager->attachTool(brush);
    toolManager->attachTool(eraser);

    brush->setColor(HSVtoHEX(0, 100, 100));

    eraser->setColor(0xFFFFFFFF);

    colorPicker->setPosition(1340, 625);

    canvas = nullptr;

    loadPlugins("editor_plugin_api/plugins/");

    dm = this;
}

void DrawingManager::createCanvas(uint32_t width, uint32_t height) {
    delete canvas;
    canvas = new Canvas(width, height);

    canvas->setPosition(125, 50);

    attachChild(canvas);
    current_canvas = canvas;
}

void DrawingManager::startToolApplication(uint32_t x, uint32_t y) {
    toolManager->getActiveTool()->startApplication(*canvas, x, y, colorPicker->getFrgColor(),
                                                   colorPicker->getBkgColor(),
                                                   settingsContainer->getSettings());
}

void DrawingManager::endToolApplication(uint32_t x, uint32_t y) {
    toolManager->getActiveTool()->endApplication(*canvas, x, y);
}

void DrawingManager::applyTool(uint32_t x, uint32_t y) {
    // fprintf(stderr, "Applying tool");
    toolManager->getActiveTool()->apply(*canvas, x, y);
}

void DrawingManager::updateActiveColor(uint32_t color) {
    // frgColor = color;
    fprintf(stderr, "New active color value is %x" PRIu32 "\n", color);
}

void DrawingManager::setCurrentSettingsCollection(SettingsCollection *collection) {
    fprintf(stderr, "Currect settings collection is %p\n", static_cast<void *>(settingsContainer));

    settingsContainer->setCurrentCollection(collection);
}

void DrawingManager::loadPlugins(const char *plugins_dir) {
    for (auto &entry : std::filesystem::directory_iterator(plugins_dir)) {
        fprintf(stderr, "Detected plugin directory %s\n", entry.path().c_str());

        auto plugin_executable = entry.path() / entry.path().filename();
        plugin_executable += ".so";

        auto icon_path = entry.path() / "icon.png";

        fprintf(stderr, "Executable path is %s and icon path is %s\n", plugin_executable.c_str(),
                icon_path.c_str());
        fprintf(stderr, "Trying to load plugin\n");

        void *handle = dlopen(plugin_executable.c_str(), RTLD_NOW);
        if (nullptr == handle) {
            fprintf(stderr, "An error occurred while opening .so file\n");
            break;
        }

        PluginAPI::Plugin *(*get_plugin)() =
            reinterpret_cast<PluginAPI::Plugin *(*)()>(dlsym(handle, "get_plugin"));

        if (nullptr == get_plugin) {
            fprintf(stderr, "An error occurred while loading get_plugin function");
            break;
        }

        PluginAPI::Plugin *plugin_instance = get_plugin();
        if (nullptr == plugin_instance) {
            fprintf(stderr,
                    "An error occurred while getting instance of PluginAPI::Plugin object\n");
            break;
        }

        uint64_t texture = RenderEngine::LoadTexture(icon_path.c_str());

        PluginTool *tool = new PluginTool(handle, plugin_instance);
        tool->attachTexture(texture);

        toolManager->attachTool(tool);
    }
}

PluginTool::PluginTool(void *handle, PluginAPI::Plugin *plugin) : handle(handle), plugin(plugin) {
    plugin->init();

    mySettings = new SettingsCollection;
    for (auto &property : plugin->properties) {
        if (property.first != PluginAPI::TYPE::PRIMARY_COLOR &&
            property.first != PluginAPI::TYPE::SECONDARY_COLOR &&
            property.first != PluginAPI::TYPE::THICKNESS) {
            wchar_t *wchar_label = nullptr;
            if (property.second.label) {
                wchar_label =
                    static_cast<wchar_t *>(calloc(strlen(property.second.label), sizeof(wchar_t)));
                mbstowcs(wchar_label, property.second.label, strlen(property.second.label));
            } else {
                wchar_label = const_cast<wchar_t *>(L"");
            }
            switch (property.second.display_type) {
                case PluginAPI::Property::DISPLAY_TYPE::SLIDER:
                    mySettings->addSetting(property.first, new SliderSetting(wchar_label));
                    break;

                case PluginAPI::Property::DISPLAY_TYPE::CHECKBOX:
                    mySettings->addSetting(property.first, new CheckboxSetting(wchar_label));
                    break;

                default:
                    fprintf(stderr,
                            "The required property has non-implemented selector %d, so this "
                            "whole abomination is likely to crush\n",
                            property.second.display_type);
                    break;
            }
        }
    }

    if (plugin->properties.contains(PluginAPI::TYPE::THICKNESS)) {
        mySettings->addSetting(PluginAPI::TYPE::THICKNESS, new SliderSetting(L"Thickness"));
    }
}

PluginTool::~PluginTool() {
    plugin->deinit();
    dlclose(handle);
}

void PluginTool::startApplication(Canvas &canvas, uint32_t x, uint32_t y, uint32_t frgColor,
                                  uint32_t bkgColor,
                                  std::unordered_map<SettingKey, Setting> settings) {
    PluginAPI::Position pos = {x, y};
    PluginAPI::Canvas api_canvas = {reinterpret_cast<uint8_t *>(canvas.getData()),
                                    canvas.getHeight(), canvas.getWidth()};

    for (auto &property : plugin->properties) {
        switch (property.second.display_type) {
            case PluginAPI::Property::DISPLAY_TYPE::SLIDER:
                property.second.double_value = settings[property.first].slider_pos;
                break;

            case PluginAPI::Property::DISPLAY_TYPE::CHECKBOX:
                property.second.int_value = settings[property.first].checkbox;
                break;

            default:
                break;
        }
    }

    if (plugin->properties.contains(PluginAPI::TYPE::THICKNESS))
        plugin->properties[PluginAPI::TYPE::THICKNESS].int_value =
            MAX_THICKNESS * plugin->properties[PluginAPI::TYPE::THICKNESS].double_value;

    if (plugin->properties.contains(PluginAPI::TYPE::PRIMARY_COLOR))
        plugin->properties[PluginAPI::TYPE::PRIMARY_COLOR].int_value = frgColor;

    if (plugin->properties.contains(PluginAPI::TYPE::SECONDARY_COLOR))
        plugin->properties[PluginAPI::TYPE::SECONDARY_COLOR].int_value = bkgColor;

    plugin->start_apply(api_canvas, pos);
}

void PluginTool::endApplication(Canvas &canvas, uint32_t x, uint32_t y) {
    fprintf(stderr, "Ending plugin tool application\n");
    PluginAPI::Position pos = {x, y};
    PluginAPI::Canvas api_canvas = {reinterpret_cast<uint8_t *>(canvas.getData()),
                                    canvas.getHeight(), canvas.getWidth()};
    plugin->stop_apply(api_canvas, pos);
}

void PluginTool::apply(Canvas &canvas, uint32_t x, uint32_t y) {
    // fprintf(stderr, "Applying plugin tool\n");
    PluginAPI::Position pos = {x, y};
    PluginAPI::Canvas api_canvas = {reinterpret_cast<uint8_t *>(canvas.getData()),
                                    canvas.getHeight(), canvas.getWidth()};
    plugin->stop_apply(api_canvas, pos);
}

ToolManager *DrawingManager::getToolManager() { return toolManager; }

ToolManager::ToolManager() { activeTool = nullptr; }

void ToolManager::attachTool(AbstractTool *tool) {
    fprintf(stderr, "Setting position (%d, %d)\n", x, y);
    tool->setPosition(x + 5, y + 5 + 60 * children.size());

    attachChild(tool);

    if (!activeTool) setActiveTool(tool);
}

void ToolManager::setActiveTool(AbstractTool *tool) {
    if (activeTool) activeTool->deactivate();
    SettingsCollection *current_settings = tool->activate();

    static_cast<DrawingManager *>(parent)->setCurrentSettingsCollection(current_settings);

    activeTool = tool;
}

AbstractTool *ToolManager::getActiveTool() { return activeTool; }

HSVSlider::HSVSlider(uint32_t width, uint32_t height) : cur_hue(0) {
    setSize(width, height);

    rainbowBkg = new uint32_t[width * height];

    setThickness(1);
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

    RenderEngine::DrawRect(x, y + height * cur_hue / 360, width, 3, {0, 0, 0, 0},
                           {255, 255, 255, 255}, -2);
    RectangleButton::draw();
}

void HSVSlider::click(const Event &ev) {
    cur_hue = (ev.mouse.y - y) * 360 / height;
    fprintf(stderr, "New hue value: %" PRIu16 "\n", cur_hue);
    static_cast<ColorPicker *>(parent)->updateHue(cur_hue);
}

void HSVSlider::onMouseMove(const Event &ev) {
    if (pressed && isInside(ev.mouse.x, ev.mouse.y)) {
        cur_hue = (ev.mouse.y - y) * 360 / height;
        fprintf(stderr, "New hue value: %" PRIu16 "\n", cur_hue);
        static_cast<ColorPicker *>(parent)->updateHue(cur_hue);
    }
}

HSVFader::HSVFader(uint32_t width, uint32_t height) : cur_sat(0), cur_val(0) {
    setSize(width, height);
    setBackgroundColor({0, 0, 0, 0});
    setHoverColor({0, 0, 0, 0});
    setPressColor({0, 0, 0, 0});
    setOutlineColor({255, 255, 255, 255});
    setThickness(1);

    SVBkg = new uint32_t[width * height];
    H = 0;
    upToDate = false;
}

void HSVFader::draw() {
    if (!upToDate) redrawBkg();

    RenderEngine::DrawBitmap(x, y, width, height, SVBkg);
    RenderEngine::DrawRect(x + width * cur_sat / 100 - 2, y + height - height * cur_val / 100 - 2,
                           5, 5, {0, 0, 0, 0}, from_hex(HSVtoHEX(0, 0, 100 - cur_val)), -2);
    RectangleButton::draw();
}

void HSVFader::redrawBkg() {
    for (uint32_t y = 0; y < static_cast<uint32_t>(height); y++) {
        for (uint32_t x = 0; x < static_cast<uint32_t>(width); x++) {
            SVBkg[y * width + x] = HSVtoHEX(H, 100 * x / width, 100 * (height - y - 1) / height);
        }
    }

    upToDate = true;
}

void HSVFader::click(const Event &ev) {
    cur_val = 100 - (ev.mouse.y - y) * 100 / height;
    cur_sat = (ev.mouse.x - x) * 100 / width;
    fprintf(stderr, "New saturation: %" PRIu8 ", new value: %" PRIu8 "\n", cur_sat, cur_val);
    static_cast<ColorPicker *>(parent)->updateSV(cur_sat, cur_val);
}

void HSVFader::onMouseMove(const Event &ev) {
    if (pressed && isInside(ev.mouse.x, ev.mouse.y)) {
        click(ev);
    }
}

void HSVFader::updateHue(uint16_t hue) {
    H = hue;
    upToDate = false;
}

void ColorPicker::setPosition(int x, int y) {
    RectangleWindow::setPosition(x, y);
    hue->setPosition(x + 200, y + 100);
    sat_val->setPosition(x + 25, y + 100);
    foregroundColor->setPosition(x + 25, y + 25);
    backgroundColor->setPosition(x + 180, y + 25);
}

ColorPicker::ColorPicker() {
    // setPosition(1340, 700);
    setBackgroundColor({0, 0, 0, 0});
    setOutlineColor({255, 255, 255, 255});
    setThickness(-2);
    setSize(260, 275);

    hue = new HSVSlider(30, 150);
    sat_val = new HSVFader(150, 150);

    // sat_val->setPosition(1365, 725);
    // hue->setPosition(1540, 725);

    foregroundColor = new ColorSample(0);
    backgroundColor = new ColorSample(1);

    foregroundColor->activate();
    foreground = true;

    // foregroundColor->setPosition(1360, 625);
    // backgroundColor->setPosition(1425, 625);

    foregroundColor->setColor(HSVtoHEX(0, 0, 0));
    backgroundColor->setColor(HSVtoHEX(0, 0, 100));

    foregroundColor->setSize(50, 50);
    backgroundColor->setSize(50, 50);

    curFrg = 0;
    curBkg = 0xFFFFFFFF;

    H = 0;
    S = 100;
    V = 100;

    attachChild(hue);
    attachChild(sat_val);
    attachChild(foregroundColor);
    attachChild(backgroundColor);
}

void ColorPicker::updateActiveColor() {
    if (foreground) {
        curFrg = HSVtoHEX(H, S, V);
        foregroundColor->setColor(curFrg);
    } else {
        curBkg = HSVtoHEX(H, S, V);
        backgroundColor->setColor(curBkg);
    }
}

void ColorPicker::updateHue(uint16_t hue) {
    if (H == hue) return;
    updateActiveColor();
    H = hue;

    static_cast<DrawingManager *>(parent)->updateActiveColor(HSVtoHEX(H, S, V));
    sat_val->updateHue(H);
}

void ColorPicker::updateSV(uint8_t saturation, uint8_t value) {
    if (saturation == S && value == V) return;

    S = saturation;
    V = value;
    updateActiveColor();
    static_cast<DrawingManager *>(parent)->updateActiveColor(HSVtoHEX(H, S, V));
}

void ColorPicker::activateColor(uint32_t tag) {
    // TODO deactivate previous one, set suitable one as the target for color

    if (tag == 0) {
        backgroundColor->deactivate();
        foreground = true;
    } else {
        foregroundColor->deactivate();
        foreground = false;
    }
}

uint32_t ColorPicker::getFrgColor() { return curFrg; }

uint32_t ColorPicker::getBkgColor() { return curBkg; }

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

SettingsCollection *AbstractTool::activate() {
    setBackgroundColor({255, 255, 255, 255});
    setHoverColor({255, 255, 255, 255});

    return mySettings;
}

void AbstractTool::deactivate() {
    setBackgroundColor({0, 0, 0, 0});
    setHoverColor({100, 100, 100, 255});
}

// TODO different icons for selected and not selected tools

Eraser::Eraser() { attachTexture(RenderEngine::LoadTexture("img/eraser.png")); }

Brush::Brush() {
    attachTexture(RenderEngine::LoadTexture("img/brush.png"));
    mySettings = new SettingsCollection;
    mySettings->addSetting(2, new SliderSetting(L"Thickness"));
}

void Brush::startApplication(Canvas &, uint32_t x, uint32_t y, uint32_t frgColor, uint32_t,
                             std::unordered_map<SettingKey, Setting> settings) {
    prev_x = x;
    prev_y = y;
    radius = settings[2].slider_pos * 100;
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
        double x_from = std::max(xt - static_cast<double>(radius), 0.0);
        double y_from = std::max(yt - static_cast<double>(radius), 0.0);

        double x_to = std::min(xt + radius, static_cast<double>(width));
        double y_to = std::min(yt + radius, static_cast<double>(height));

        // fprintf(stderr, "Drawing circle in boundary (%lf, %lf) - (%lf, %lf)\n", x_from, y_from,
        //         x_to, y_to);

        for (uint32_t x_cur = x_from; x_cur < x_to; x_cur++) {
            for (uint32_t y_cur = y_from; y_cur < y_to; y_cur++) {
                if ((xt - x_cur) * (xt - x_cur) + (yt - y_cur) * (yt - y_cur) <= radius * radius) {
                    data[static_cast<uint32_t>(y_cur) * width + static_cast<uint32_t>(x_cur)] =
                        color;
                }
            }
        }
    }

    prev_x = x;
    prev_y = y;
}

void Brush::setColor(uint32_t color) { this->color = color; }

ColorSample::ColorSample(uint32_t tag) : tag(tag) {
    setThickness(-2);
    setOutlineColor({255, 255, 255, 255});
}

void ColorSample::setColor(uint32_t color) {
    c = from_hex(color);

    setBackgroundColor(c);
    setHoverColor(c);
    setPressColor(c);
}

void ColorSample::click(const Event &) {
    if (parent) {
        static_cast<ColorPicker *>(parent)->activateColor(tag);
    }

    activate();
}

void ColorSample::activate() { setThickness(-5); }

void ColorSample::deactivate() { setThickness(-2); }

void Eraser::startApplication(Canvas &, uint32_t x, uint32_t y, uint32_t, uint32_t bkgColor,
                              std::unordered_map<SettingKey, Setting> settings) {
    prev_x = x;
    prev_y = y;
    color = bkgColor;
    radius = settings[2].slider_pos * 100;
}

SettingsContainer::SettingsContainer() : current(nullptr) {
    setThickness(-2);
    setBackgroundColor({0, 0, 0, 0});
    setOutlineColor({255, 255, 255, 255});
    setPosition(1340, 0);
    setSize(260, 625);
}

void SettingsContainer::setCurrentCollection(SettingsCollection *collection) {
    if (current) {
        current->detach();
    }

    current = collection;
    attachChild(current);
}

std::unordered_map<SettingKey, Setting> SettingsContainer::getSettings() {
    return current->getCurrentSettings();
}

SettingsCollection::SettingsCollection() : accumulatedHeight(0) {}

void SettingsContainer::draw() {
    if (current) current->setPosition(x, y);
    RectangleWindow::draw();
}

void SettingsCollection::draw() {
    RenderEngine::pushGlobalOffset(-x, -y);
    RectangleWindow::draw();
    RenderEngine::popGlobalOffset();
}

void SettingsCollection::addSetting(SettingKey key, SettingElement *elem) {
    elem->setPosition(0, accumulatedHeight);
    accumulatedHeight += elem->getHeight();
    attachChild(elem);

    elems[key] = elem;
}

void SettingsCollection::processEvent(Event ev) {
    if (ev.eventType == EV_MOUSE_KEY_PRESS || ev.eventType == EV_MOUSE_KEY_RELEASE ||
        ev.eventType == EV_MOUSE_MOVE) {
        ev.mouse.x -= x;
        ev.mouse.y -= y;
    }

    RectangleWindow::processEvent(ev);
}

template <typename T, typename From, typename To>
concept Mapping = std::is_invocable_r<To, T, From>::value;

template <typename KeyType, typename ValueType, typename ResultType, typename Func>
requires Mapping<Func, ValueType, ResultType> std::unordered_map<KeyType, ResultType>
applyTransformation(std::unordered_map<KeyType, ValueType> container, Func f) {
    std::unordered_map<KeyType, ResultType> result;

    for (const auto &elem : container) {
        result[elem.first] = f(elem.second);
    }

    return result;
}

std::unordered_map<SettingKey, Setting> SettingsCollection::getCurrentSettings() {
    return applyTransformation<SettingKey, SettingElement *, Setting>(
        elems, [](SettingElement *el) -> Setting { return el->getSettingValue(); });
}

Checkbox::Checkbox() : value(false) {
    setSize(30, 30);
    setThickness(-2);
    setHoverColor({255, 255, 255, 100});
    setBackgroundColor({0, 0, 0, 0});
    setOutlineColor({255, 255, 255, 255});
    setPressColor({255, 255, 255, 255});
}

void Checkbox::click(const Event &) {
    value = !value;

    if (value) {
        setBackgroundColor({255, 255, 255, 200});
    } else {
        setBackgroundColor({0, 0, 0, 0});
    }
}

bool Checkbox::getValue() { return value; }

void SettingElement::processEvent(Event ev) {
    if (ev.eventType == EV_MOUSE_KEY_PRESS || ev.eventType == EV_MOUSE_KEY_RELEASE ||
        ev.eventType == EV_MOUSE_MOVE) {
        ev.mouse.x -= x;
        ev.mouse.y -= y;
    }

    RectangleWindow::processEvent(ev);
}

CheckboxSetting::CheckboxSetting(const wchar_t *label) : label(label) {
    TextWindow *labelWindow = new TextWindow;
    labelWindow->setText(label);
    labelWindow->setPosition(8, 3);
    labelWindow->setCharSize(25);

    checkbox = new Checkbox;
    checkbox->setPosition(220, 3);

    setOutlineColor({255, 255, 255, 255});
    setPosition(0, 0);
    setBackgroundColor({0, 0, 0, 0});
    setThickness(-1);
    setSize(260, 35);

    fprintf(stderr, "Created checkbox with the label %ls\n", this->label);

    attachChild(checkbox);
    attachChild(labelWindow);
}

int CheckboxSetting::getHeight() { return height; }

Setting CheckboxSetting::getSettingValue() {
    Setting result;
    result.checkbox = checkbox->getValue();
    return result;
}

void CheckboxSetting::draw() {
    RenderEngine::pushRelGlobalOffset(-x, -y);
    ContainerWindow::draw();
    RenderEngine::popGlobalOffset();

    RenderEngine::DrawRect(x, y, width, height, bkg, frg, thickness);
}

SliderSetting::SliderSetting(const wchar_t *label) : label(label) {
    TextWindow *labelWindow = new TextWindow;
    labelWindow->setText(label);
    labelWindow->setPosition(8, 3);
    labelWindow->setCharSize(25);

    slider = new Slider(true);
    slider->setSize(10, 30);
    slider->setPosition(10, 35);
    slider->setBackgroundColor({0, 0, 0, 0});
    slider->setHoverColor({255, 255, 255, 100});
    slider->setPressColor({255, 255, 255, 255});
    slider->setOutlineColor({255, 255, 255, 255});
    slider->setLimit(220);
    slider->setThickness(2);

    RectangleWindow *sliderBkg = new RectangleWindow;
    sliderBkg->setSize(230, 4);
    sliderBkg->setBackgroundColor({255, 255, 255, 255});
    sliderBkg->setPosition(10, 48);

    setOutlineColor({255, 255, 255, 255});
    setPosition(0, 0);
    setBackgroundColor({0, 0, 0, 0});
    setThickness(-1);
    setSize(260, 70);

    fprintf(stderr, "Created slider with the label %ls\n", this->label);

    attachChild(sliderBkg);
    attachChild(labelWindow);
    attachChild(slider);
}

int SliderSetting::getHeight() { return height; }

Setting SliderSetting::getSettingValue() {
    Setting res;
    res.slider_pos = static_cast<double>(slider->getPositionAlongAxis() - 10) / 220;
    printf("Slider value was fetched. It is %lf\n", res.slider_pos);
    return res;
}

void SliderSetting::draw() {
    RenderEngine::pushRelGlobalOffset(-x, -y);
    ContainerWindow::draw();
    RenderEngine::popGlobalOffset();

    RenderEngine::DrawRect(x, y, width, height, bkg, frg, thickness);
}

ModalInvokerButton::ModalInvokerButton() : modal(nullptr) {
    setThickness(-2);
    setSize(50, 50);

    setBackgroundColor({0, 0, 0, 0});
    setOutlineColor({255, 255, 255, 255});
    setHoverColor({255, 255, 255, 100});
    setPressColor({255, 255, 255, 255});
}

void ModalInvokerButton::attachModal(ModalWindow *modal) { this->modal = modal; }

void ModalInvokerButton::click(const Event &) {
    if (modal) invokeModalWindow(modal);
}

// There go dialog windows

class FinalSaveButton : public TexturedButton {
   public:
    FinalSaveButton();
    virtual void click(const Event &ev) override;

   private:
};

class FinalLoadButton : public TexturedButton {
   public:
    FinalLoadButton();
    virtual void click(const Event &ev) override;
};

FinalSaveButton::FinalSaveButton() {
    setPosition(540, 120);
    setSize(30, 30);
    setOutlineColor({255, 255, 255, 255});
    setBackgroundColor({0, 0, 0, 0});
    setHoverColor({255, 255, 255, 100});
    setPressColor({255, 255, 255, 255});

    setThickness(2);
    attachTexture(1);
}

void FinalSaveButton::click(const Event &) {
    RenderEngine::SaveToImage(static_cast<SaveDialog *>(parent)->getPath(),
                              current_canvas->getData(), current_canvas->getWidth(),
                              current_canvas->getHeight());
    static_cast<SaveDialog *>(parent)->finish();
}

FinalLoadButton::FinalLoadButton() {
    setPosition(540, 120);
    setSize(30, 30);
    setOutlineColor({255, 255, 255, 255});
    setBackgroundColor({0, 0, 0, 0});
    setHoverColor({255, 255, 255, 100});
    setPressColor({255, 255, 255, 255});

    setThickness(2);
    attachTexture(0);
}

void FinalLoadButton::click(const Event &) {
    auto [width, height, img] =
        RenderEngine::LoadFromImage(static_cast<LoadDialog *>(parent)->getPath());
    current_canvas->emplace(width, height, img);
    static_cast<SaveDialog *>(parent)->finish();
}

const wchar_t *SaveDialog::getPath() { return inp->getString(); }

const wchar_t *LoadDialog::getPath() { return inp->getString(); }

SaveDialog::SaveDialog() {
    setPosition(100, 100);
    setSize(500, 70);
    setOutlineColor({255, 140, 140, 255});
    setBackgroundColor({0, 0, 0, 255});
    setThickness(6);

    FinalSaveButton *but = new FinalSaveButton;
    inp = new InputBox;
    inp->setPosition(120, 120);
    inp->setSize(400, 30);

    attachChild(but);
    attachChild(inp);
}

LoadDialog::LoadDialog() {
    setPosition(100, 100);
    setSize(500, 70);
    setOutlineColor({255, 140, 140, 255});
    setBackgroundColor({0, 0, 0, 255});
    setThickness(6);

    FinalLoadButton *but = new FinalLoadButton;
    inp = new InputBox;
    inp->setPosition(120, 120);
    inp->setSize(400, 30);

    attachChild(but);
    attachChild(inp);
}