#pragma once
#include "PluginProcessor.h"
#include <array>

//==============================================================================
// Habillage visuel d'un module (dépend du module ET du modèle choisi : SSL / Neve / API, Opto / FET...)
struct Skin
{
    juce::Colour top, bottom, text, legend, accent, stripe;
    int knobStyle = 5;      // 0 Marconi (Neve), 1 capuchon SSL, 2 aluminium (API), 3 bakélite, 4 FET, 5 ivoire
    int display = 0;        // 0 rien, 1 LED niveau, 2 VU crème, 3 VU noir/bleu, 4 LED réduction, 5 courbe EQ
    bool brushed = false;
    bool hasStripe = false;
};

Skin skinFor (int type, int model);
juce::Colour capFor (int type, int model, int param);

//==============================================================================
class RackLnF : public juce::LookAndFeel_V4
{
public:
    RackLnF();
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos, float startAngle, float endAngle, juce::Slider&) override;
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool highlighted, bool down) override;
    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool highlighted, bool down) override;
    juce::Font getLabelFont (juce::Label&) override;
};

//==============================================================================
// Sélecteur à boutons (ex. SSL | NEVE | API)
class ChoiceButtons : public juce::Component
{
public:
    std::function<void (int)> onSelect;
    void setup (const juce::StringArray& items, int selected, const Skin& skin);
    void setSelected (int i) { if (i != selected) { selected = i; repaint(); } }
    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    bool useTwoRows() const;
    int wantedHeight() const { return useTwoRows() ? 42 : 28; }

private:
    juce::Rectangle<float> segment (int i) const;
    juce::StringArray names;
    int selected = 0;
    Skin skin;
};

//==============================================================================
class ModulePanel : public juce::Component, private juce::Timer
{
public:
    static constexpr int kWidth = 170, kHeight = 580;

    ModulePanel (RackProcessor&, int slot);
    ~ModulePanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void refresh();
    std::function<void (int slot, int dir)> onMove;
    std::function<void (int slot)> onRemove;

private:
    void timerCallback() override;
    int currentModel() const;
    void applySkin();
    int displayHeight() const;
    juce::Rectangle<int> displayRect() const { return { 10, 76, getWidth() - 20, displayHeight() }; }
    void paintDisplay (juce::Graphics&, juce::Rectangle<float>);
    void drawVU (juce::Graphics&, juce::Rectangle<float>, float pos);
    void drawLeds (juce::Graphics&, juce::Rectangle<float>, float pos, bool levelColours);

    struct Cell
    {
        juce::Slider slider;
        ChoiceButtons choice;
        juce::Label label;
        std::unique_ptr<juce::SliderParameterAttachment> sAtt;
        std::unique_ptr<juce::ParameterAttachment> cAtt;
        bool isChoice = false;
    };

    RackProcessor& proc;
    const int slot;
    int type = -1, model = 0;
    Skin skin;
    bool updating = false;
    float vuPos = 0.f, ledPos = 0.f;

    juce::ToggleButton powerBtn;
    juce::TextButton leftBtn { "<" }, rightBtn { ">" }, removeBtn { "x" };
    std::array<Cell, rack::kMaxParams> cells;
    std::unique_ptr<juce::ParameterAttachment> typeAtt;
    std::unique_ptr<juce::ButtonParameterAttachment> onAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulePanel)
};

//==============================================================================
class AddTile : public juce::Component
{
public:
    std::function<void (int type)> onPick;
    void paint (juce::Graphics&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseEnter (const juce::MouseEvent&) override { hover = true; repaint(); }
    void mouseExit (const juce::MouseEvent&) override { hover = false; repaint(); }
private:
    bool hover = false;
};

//==============================================================================
class RackEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit RackEditor (RackProcessor&);
    ~RackEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class Content : public juce::Component
    {
    public:
        explicit Content (RackEditor& o) : owner (o) {}
        void paint (juce::Graphics& g) override { owner.paintContent (g); }
        RackEditor& owner;
    };
    void paintContent (juce::Graphics&);
    void timerCallback() override;
    void layoutForCount();

    static constexpr int kRail = 30, kTop = 54, kH = kTop + ModulePanel::kHeight + 10;

    RackProcessor& proc;
    RackLnF lnf;
    Content content { *this };
    std::array<std::unique_ptr<ModulePanel>, rack::kNumSlots> panels;
    AddTile addTile;

    juce::Slider inKnob, outKnob;
    juce::Label inLabel, outLabel;
    juce::ToggleButton bypassBtn { "Bypass" };
    juce::ComboBox presetBox;
    std::unique_ptr<juce::SliderParameterAttachment> inAtt, outAtt;
    std::unique_ptr<juce::ButtonParameterAttachment> bypassAtt;
    std::unique_ptr<juce::ParameterAttachment> countAtt;
    float dispIn = 0.f, dispOut = 0.f, scale = 0.85f;
    int contentW = 900, shownCount = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RackEditor)
};
