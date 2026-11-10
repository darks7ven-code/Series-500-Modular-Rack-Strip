#include "../Source/PluginProcessor.h"

#include <array>
#include <cmath>
#include <iostream>

namespace
{
bool checkEffect (int type, const char* label, int parameterToExcite = -1, float value = 0.f)
{
    const auto& definition = rack::info (type);
    auto module = definition.make();
    std::array<float, rack::kMaxParams> values {};
    for (size_t i = 0; i < definition.params.size(); ++i)
        values[i] = definition.params[i].def;
    if (parameterToExcite >= 0)
        values[(size_t) parameterToExcite] = value;

    constexpr double sampleRate = 48000.0;
    constexpr int sampleCount = 4096;
    juce::AudioBuffer<float> audio (2, sampleCount);
    std::array<float, sampleCount> original {};
    for (int i = 0; i < sampleCount; ++i)
    {
        const float x = 0.7f * std::sin (juce::MathConstants<float>::twoPi * 997.f * (float) i / (float) sampleRate);
        original[(size_t) i] = x;
        audio.setSample (0, i, x);
        audio.setSample (1, i, x);
    }

    module->prepare (sampleRate, sampleCount);
    module->process (audio, values.data());

    double difference = 0.0;
    for (int ch = 0; ch < audio.getNumChannels(); ++ch)
        for (int i = 0; i < sampleCount; ++i)
        {
            const auto y = audio.getSample (ch, i);
            if (! std::isfinite (y))
            {
                std::cerr << label << " produced a non-finite sample\n";
                return false;
            }
            difference += std::abs ((double) y - original[(size_t) i]);
        }

    if (difference < 1.0e-3)
    {
        std::cerr << label << " did not change the test signal\n";
        return false;
    }
    std::cout << "PASS DSP: " << label << " (signal changed, output finite)\n";
    return true;
}

bool checkBypassAndState()
{
    RackProcessor source;
    if (! source.addModule (rack::Preamp))
        return false;

    auto* slotOn = source.apvts.getParameter (RackProcessor::onId (0));
    auto* masterOn = source.apvts.getParameter (RackProcessor::onId (rack::kMasterSlot));
    if (slotOn == nullptr || masterOn == nullptr)
        return false;
    slotOn->setValueNotifyingHost (0.f);
    masterOn->setValueNotifyingHost (0.f);

    constexpr int sampleCount = 512;
    source.prepareToPlay (48000.0, sampleCount);
    juce::AudioBuffer<float> audio (2, sampleCount);
    for (int i = 0; i < sampleCount; ++i)
    {
        const float x = 0.4f * std::sin (juce::MathConstants<float>::twoPi * 440.f * (float) i / 48000.f);
        audio.setSample (0, i, x);
        audio.setSample (1, i, x);
    }
    std::array<float, sampleCount> before {};
    for (int i = 0; i < sampleCount; ++i) before[(size_t) i] = audio.getSample (0, i);
    juce::MidiBuffer midi;
    source.processBlock (audio, midi);
    for (int i = 0; i < sampleCount; ++i)
        if (std::abs (audio.getSample (0, i) - before[(size_t) i]) > 1.0e-6f)
        {
            std::cerr << "Slot bypass did not pass audio unchanged\n";
            return false;
        }

    auto* savedParameter = source.apvts.getParameter (RackProcessor::paramId (0, 1));
    savedParameter->setValueNotifyingHost (0.83f);
    juce::MemoryBlock state;
    source.getStateInformation (state);

    RackProcessor restored;
    restored.setStateInformation (state.getData(), (int) state.getSize());
    if (restored.getNumModules() != 1 || restored.getSlotType (0) != rack::Preamp
        || restored.apvts.getRawParameterValue (RackProcessor::onId (0))->load() >= 0.5f
        || std::abs (restored.apvts.getRawParameterValue (RackProcessor::paramId (0, 1))->load() - 0.83f) > 1.0e-4f)
    {
        std::cerr << "Rack state did not round-trip\n";
        return false;
    }

    std::cout << "PASS rack: per-slot bypass passes audio unchanged\n";
    std::cout << "PASS state: module order, bypass and parameter round-trip\n";
    return true;
}

bool checkRackCapacity()
{
    RackProcessor processor;
    for (int i = 0; i < rack::kNumSlots; ++i)
        if (! processor.addModule (rack::Preamp))
        {
            std::cerr << "Rack rejected a module before reaching its advertised capacity\n";
            return false;
        }
    if (processor.getNumModules() != rack::kNumSlots || processor.addModule (rack::Preamp))
    {
        std::cerr << "Rack module capacity is incorrect\n";
        return false;
    }
    std::cout << "PASS rack: all 16 module slots and capacity limit\n";
    return true;
}
}

int main()
{
    juce::ScopedJuceInitialiser_GUI juceInit;
    bool ok = true;
    ok = checkEffect (rack::Preamp, "Preamp") && ok;
    ok = checkEffect (rack::ProEQ, "Parametric EQ", 1, 9.f) && ok;
    ok = checkEffect (rack::Compressor, "Compressor") && ok;
    ok = checkEffect (rack::Reverb, "Reverb") && ok;
    ok = checkBypassAndState() && ok;
    ok = checkRackCapacity() && ok;
    return ok ? 0 : 1;
}
