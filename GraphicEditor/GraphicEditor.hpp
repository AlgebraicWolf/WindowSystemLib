#ifndef GRAPHIC_EDITOR_HPP_
#define GRAPHIC_EDITOR_HPP_
#include <cstdint>

#include "../WindowSystem/Window.hpp"
#include "../editor_plugin_api/api/api.hpp"

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

union Setting {
    bool checkbox;
    double slider_pos;
    char *str;
};

class SettingElement : public RectangleWindow {
   public:
    virtual Setting getSettingValue() = 0;
    virtual int getHeight() = 0;  // Height getter for the sake of settings fetching
    virtual void processEvent(Event ev) override;

   private:
};

class Checkbox : public RectangleButton {
   public:
    Checkbox();
    virtual void click(const Event &ev) override;
    bool getValue();

   private:
    bool value;
};

class SliderSetting : public SettingElement {
   public:
    SliderSetting(const wchar_t *label);

    virtual Setting getSettingValue() override;
    virtual int getHeight() override;
    virtual void draw() override;
    //  void adjustPosition(int x, int y);

   private:
    const wchar_t *label;
    Slider *slider;
};

class CheckboxSetting : public SettingElement {
   public:
    CheckboxSetting(const wchar_t *label);

    virtual Setting getSettingValue() override;
    virtual int getHeight() override;
    virtual void draw() override;

   private:
    const wchar_t *label;
    Checkbox *checkbox;
};

using SettingKey = uint32_t;

// Collection of settings
class SettingsCollection : public RectangleWindow {
   public:
    SettingsCollection();
    std::unordered_map<SettingKey, Setting> getCurrentSettings();
    void addSetting(SettingKey key, SettingElement *el);
    virtual void draw() override;
    virtual void processEvent(Event ev) override;

   private:
    int accumulatedHeight;
    std::unordered_map<SettingKey, SettingElement *> elems;
    Setting getSettingElementValue(SettingElement *elem);
};

// Container for different setting collection windows
class SettingsContainer : public RectangleWindow {
   public:
    SettingsContainer();
    virtual void draw() override;
    void setCurrentCollection(SettingsCollection *collection);
    std::unordered_map<SettingKey, Setting> getSettings();

   private:
    SettingsCollection *current;
};

// Tool interface
class AbstractTool : public TexturedButton {
   public:
    AbstractTool();
    virtual void click(const Event &ev) override;

    virtual void startApplication(Canvas &canvas, uint32_t x, uint32_t y, uint32_t frgColor,
                                  uint32_t bkgColor,
                                  std::unordered_map<SettingKey, Setting> settings) = 0;
    virtual void endApplication(Canvas &canvas, uint32_t x, uint32_t y) = 0;
    virtual void apply(Canvas &canvas, uint32_t x, uint32_t y) = 0;
    void deactivate();
    SettingsCollection *activate();

   protected:
    SettingsCollection *mySettings;
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
    uint16_t cur_hue;
    uint32_t *rainbowBkg;
};

class ColorSample : public RectangleButton {
   public:
    ColorSample(uint32_t tag);
    void setColor(uint32_t newColor);
    virtual void click(const Event &ev) override;
    void activate();
    void deactivate();

   private:
    uint32_t tag;
    Color c;
};

class HSVFader : public RectangleButton {
   public:
    HSVFader(uint32_t width, uint32_t height);

    void updateHue(uint16_t hue);
    virtual void draw() override;

   private:
    virtual void onMouseMove(const Event &ev) override;
    virtual void click(const Event &ev) override;
    void redrawBkg();

    uint32_t *SVBkg;
    uint16_t H;
    uint8_t cur_sat;
    uint8_t cur_val;
    bool upToDate;
};

// Thing for selecting colors
class ColorPicker : public RectangleWindow {
   public:
    ColorPicker();
    void updateHue(uint16_t H);
    void updateSV(uint8_t S, uint8_t V);
    void activateColor(uint32_t tag);

    void setPosition(int x, int y);

    uint32_t getBkgColor();
    uint32_t getFrgColor();

   private:
    void updateActiveColor();

    uint16_t H;
    uint8_t S;
    uint8_t V;
    bool foreground;

    uint32_t curFrg;
    uint32_t curBkg;

    HSVSlider *hue;
    HSVFader *sat_val;
    ColorSample *foregroundColor;
    ColorSample *backgroundColor;
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
    void setCurrentSettingsCollection(SettingsCollection *collection);

    // ~DrawingManager();

   private:
    // uint32_t bkgColor;
    // uint32_t frgColor;

    void loadPlugins(const char *plugins_dir);
    ToolManager *toolManager;
    Canvas *canvas;
    ColorPicker *colorPicker;
    SettingsContainer *settingsContainer;
};

// There go important windows

class LoadDialog : public ModalWindow {};

class SaveDialog : public ModalWindow {};

// There go important buttons

// class LoadButton : public TexturedButton {
//    public:
//       LoadButton();
//       virtual void click(const Event& ev);
//    private:
//       LoadDialog *dialog;
// };

// class SaveButton : public TexturedButton {
//    public:
//       SaveButton();
//       virtual void click(const Event& ev);
//    private:
//       SaveDialog *dialog;
// };

class ModalInvokerButton : public TexturedButton {
   public:
    ModalInvokerButton();
    void attachModal(ModalWindow *modal);
    virtual void click(const Event &ev) override;

   private:
    ModalWindow *modal;
};

// There go tools

// PluginTool is a tool wrapper for plugin
class PluginTool : public AbstractTool {
   public:
    PluginTool(void *handle, PluginAPI::Plugin *plugin);
    ~PluginTool();

    virtual void startApplication(Canvas &canvas, uint32_t x, uint32_t y, uint32_t frgColor,
                                  uint32_t bkgColor,
                                  std::unordered_map<SettingKey, Setting> settings) override;
    virtual void endApplication(Canvas &canvas, uint32_t x, uint32_t y) override;
    virtual void apply(Canvas &canvas, uint32_t x, uint32_t y) override;

   private:
    void *handle;
    PluginAPI::Plugin *plugin;
};

// Brush tool class
class Brush : public AbstractTool {
   public:
    Brush();
    virtual void startApplication(Canvas &canvas, uint32_t x, uint32_t y, uint32_t frgColor,
                                  uint32_t bkgColor,
                                  std::unordered_map<SettingKey, Setting> settings) override;
    virtual void endApplication(Canvas &canvas, uint32_t x, uint32_t y) override;
    virtual void apply(Canvas &canvas, uint32_t x, uint32_t y) override;
    void setColor(uint32_t color);

   protected:
    uint32_t color;
    uint32_t prev_x;
    uint32_t prev_y;
    uint32_t radius;
};

// Eraser is a modification of brush that uses background color instead of foreground color
class Eraser : public Brush {
   public:
    Eraser();
    virtual void startApplication(Canvas &canvas, uint32_t x, uint32_t y, uint32_t frgColor,
                                  uint32_t bkgColor,
                                  std::unordered_map<SettingKey, Setting> settings) override;
};

#endif  // GRAPHIC_EDITOR_HPP_