#pragma once
#include "Modules.h"
#include <array>

class RackProcessor : public juce::AudioProcessor
{
public:
    RackProcessor();
    ~RackProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Series 500 Modular Rack"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    // --- Rack ---
    void setSlotType (int slot, int type, bool resetToDefaults);
    bool addModule (int type);              // ajoute à la fin du rack
    void removeModule (int slot);           // retire et décale les suivants
    void moveModule (int slot, int dir);    // dir = -1 (gauche) ou +1 (droite)
    int getNumModules() const { return (int) countP->load(); }
    void loadPreset (int index);
    static int numPresets();
    static const char* presetName (int index);

    static juce::String typeId (int s)            { return "s" + juce::String (s) + "_type"; }
    static juce::String onId (int s)              { return "s" + juce::String (s) + "_on"; }
    static juce::String paramId (int s, int j)    { return "s" + juce::String (s) + "_p" + juce::String (j); }

    int getSlotType (int slot) const { return (int) typeP[slot]->load(); }

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float> slotMeter[rack::kTotalSlots];
    std::atomic<float> slotLevel[rack::kTotalSlots];

    // Tampons circulaires pour les analyseurs de l'interface (entrée mono, sortie gauche/droite)
    struct Scope
    {
        static constexpr int N = 4096;
        std::array<float, N> inM {}, outL {}, outR {};
        std::atomic<int> pos { 0 };
    };
    Scope scopes[rack::kTotalSlots];
    std::atomic<float> inPeak { 0.f }, outPeak { 0.f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void setNorm (const juce::String& id, float norm);
    void setCount (int n);
    void copySlot (int from, int to);
    void swapSlotsInternal (int a, int b);
    juce::CriticalSection slotLock;
    float getNorm (const juce::String& id) const;

    std::array<std::array<std::unique_ptr<rack::Module>, rack::NumTypes>, rack::kTotalSlots> mods;
    int lastType[rack::kTotalSlots] {};
    void runSlot (int s, int type, juce::AudioBuffer<float>& buffer);

    std::atomic<float>* typeP[rack::kTotalSlots];
    std::atomic<float>* onP[rack::kTotalSlots];
    std::atomic<float>* pP[rack::kTotalSlots][rack::kMaxParams];
    std::atomic<float>* inP = nullptr;
    std::atomic<float>* outP = nullptr;
    std::atomic<float>* bypassP = nullptr;
    std::atomic<float>* countP = nullptr;
    float lastIn = 1.f, lastOut = 1.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RackProcessor)
};
