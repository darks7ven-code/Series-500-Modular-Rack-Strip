#pragma once
#include "PluginProcessor.h"
#include <array>

//==============================================================================
// Habillage d'un module : dépend du module ET du modèle choisi (SSL / Neve / API, Opto / FET...)
struct Skin
{
    juce::Colour top, bottom, text, legend, accent, stripe;
    int knobStyle = 5;      // 0 Marconi (Neve), 1 capuchon SSL, 2 aluminium (API), 3 bakélite, 4 FET, 5 ivoire
    int display = 0;        // 0 rien, 2 VU crème, 3 VU clair, 4 LED réduction, 5 courbe, 6 spectre, 7 vectorscope, 8 répétitions
    int strip = 0;          // 0 rien, 1 LED niveau, 2 LED réduction (sous l'écran)
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
    void drawScrollbar (juce::Graphics&, juce::ScrollBar&, int x, int y, int w, int h, bool vertical,
                        int thumbStart, int thumbSize, bool over, bool down) override;
    juce::Font getLabelFont (juce::Label&) override;
};

//==============================================================================
// Analyseur de spectre (bandes logarithmiques 20 Hz - 20 kHz)
class SpectrumAnalyzer
{
public:
    static constexpr int kBands = 72;
    SpectrumAnalyzer();

    template <class Getter>
    void analyse (Getter get, double sampleRate)
    {
        constexpr int N = 2048;
        for (int i = 0; i < N; ++i) work[(size_t) i] = get (i);
        for (int i = N; i < 2 * N; ++i) work[(size_t) i] = 0.f;
        window.multiplyWithWindowingTable (work.data(), (size_t) N);
        fft.performFrequencyOnlyForwardTransform (work.data());
        finish (sampleRate);
    }
    void clear() { bands.fill (0.f); }
    std::array<float, kBands> bands {};

private:
    void finish (double sampleRate);
    juce::dsp::FFT fft { 11 };
    juce::dsp::WindowingFunction<float> window { 2048, juce::dsp::WindowingFunction<float>::hann };
    std::array<float, 4096> work {};
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
    static constexpr int kWidth = 170, kHeight = 620;

    ModulePanel (RackProcessor&, int slot, int width);
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
    void buildBackground();
    int mainDisplayHeight() const;
    int displayHeight() const;
    juce::Rectangle<int> displayRect() const { return { 10, 96, getWidth() - 20, displayHeight() }; }
    void paintDisplay (juce::Graphics&, juce::Rectangle<float>);
    void drawVU (juce::Graphics&, juce::Rectangle<float>, float pos);
    void drawLeds (juce::Graphics&, juce::Rectangle<float>, float pos, bool levelColours);
    void drawScreen (juce::Graphics&, juce::Rectangle<float>);
    void drawSpectrum (juce::Graphics&, juce::Rectangle<float>, bool showInput, float markerHz, float fillAlpha);
    void drawCurve (juce::Graphics&, juce::Rectangle<float>);
    void drawScope (juce::Graphics&, juce::Rectangle<float>);
    void drawTaps (juce::Graphics&, juce::Rectangle<float>);
    void updateSpectra();
    std::array<float, rack::kMaxParams> realValues() const;

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
    const bool isMaster;
    int type = -1, model = 0, tick = 0;
    Skin skin;
    juce::Image bg;
    float vuPos = 0.f, ledPos = 0.f, corr = 0.f, peakL = 0.f, peakR = 0.f;
    SpectrumAnalyzer specIn, specOut;

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
    void resized() override;
    void paint (juce::Graphics&) override;
    void scrollRackTo(int px) { rackView.setViewPosition(0, px); }
private:
    RackProcessor& proc;
    RackLnF skin;
    juce::Image logo;
    juce::TextEditor search;
    juce::ComboBox presets;
    juce::Viewport rackView, libraryView;
    juce::Component rows, library;
    juce::OwnedArray<juce::Component> libraryItems, rackItems;
    std::unique_ptr<ModulePanel> preview;
    juce::ToggleButton bypass {"Bypass"};
    std::unique_ptr<juce::ButtonParameterAttachment> bypassLink;
    juce::Slider input, output;
    std::unique_ptr<juce::SliderParameterAttachment> inputLink, outputLink;
    int selected = rack::kMasterSlot;
    std::vector<int> displayed;
    void rebuildLibrary();
    void rebuildRack();
    void select(int);
    void timerCallback() override;
};
