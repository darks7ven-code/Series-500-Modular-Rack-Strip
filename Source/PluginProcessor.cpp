#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace rack;

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout RackProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    juce::StringArray names;
    for (int t = 0; t < NumTypes; ++t) names.add (info (t).name);

    for (int s = 0; s < kTotalSlots; ++s)
    {
        const bool master = (s == kMasterSlot);
        const auto n = master ? juce::String ("Master Bus") : "Slot " + juce::String (s + 1);
        layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { typeId (s), 1 }, n + " Module", names, master ? (int) BusComp : 0));
        layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { onId (s), 1 }, n + " On", true));
        for (int j = 0; j < kMaxParams; ++j)
        {
            float def = 0.5f;
            if (master && j < (int) info (BusComp).params.size())
                def = info (BusComp).params[(size_t) j].toNorm (info (BusComp).params[(size_t) j].def);
            layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { paramId (s, j), 1 },
                        n + " Param " + juce::String (j + 1), juce::NormalisableRange<float> (0.f, 1.f), def));
        }
    }
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { "in_gain", 1 }, "Input",
                juce::NormalisableRange<float> (-24.f, 24.f, 0.1f), 0.f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { "out_gain", 1 }, "Output",
                juce::NormalisableRange<float> (-24.f, 24.f, 0.1f), 0.f));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { "bypass", 1 }, "Bypass", false));
    layout.add (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { "count", 1 }, "Modules", 0, kNumSlots, 0));
    return layout;
}

RackProcessor::RackProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "RACKSTRIP", createLayout())
{
    for (int s = 0; s < kTotalSlots; ++s)
    {
        slotMeter[s] = 0.f;
        slotLevel[s] = 0.f;
        typeP[s] = apvts.getRawParameterValue (typeId (s));
        onP[s]   = apvts.getRawParameterValue (onId (s));
        for (int j = 0; j < kMaxParams; ++j) pP[s][j] = apvts.getRawParameterValue (paramId (s, j));
        if (s == kMasterSlot) mods[(size_t) s][(size_t) BusComp] = info (BusComp).make();
        else for (int t = 1; t < NumTypes; ++t) if (t != BusComp) mods[(size_t) s][(size_t) t] = info (t).make();
        lastType[s] = 0;
    }
    inP = apvts.getRawParameterValue ("in_gain");
    outP = apvts.getRawParameterValue ("out_gain");
    bypassP = apvts.getRawParameterValue ("bypass");
    countP = apvts.getRawParameterValue ("count");
}

bool RackProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    const auto& o = l.getMainOutputChannelSet();
    return (o == juce::AudioChannelSet::mono() || o == juce::AudioChannelSet::stereo()) && o == l.getMainInputChannelSet();
}

void RackProcessor::prepareToPlay (double sr, int bs)
{
    for (auto& slot : mods)
        for (auto& m : slot)
            if (m) { m->prepare (sr, bs); m->reset(); }
    for (int s = 0; s < kTotalSlots; ++s) lastType[s] = 0;
    lastIn = juce::Decibels::decibelsToGain (inP->load());
    lastOut = juce::Decibels::decibelsToGain (outP->load());
}

void RackProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int n = buffer.getNumSamples();
    for (int c = getTotalNumInputChannels(); c < getTotalNumOutputChannels(); ++c) buffer.clear (c, 0, n);

    const float inG = juce::Decibels::decibelsToGain (inP->load());
    buffer.applyGainRamp (0, n, lastIn, inG);
    lastIn = inG;

    float pk = 0.f;
    for (int c = 0; c < buffer.getNumChannels(); ++c) pk = juce::jmax (pk, buffer.getMagnitude (c, 0, n));
    inPeak.store (pk);

    // Le thread de l'interface verrouille pendant qu'il réorganise le rack :
    // dans ce cas on laisse passer le son tel quel pour ce bloc (aucune lecture incohérente).
    juce::CriticalSection::ScopedTryLockType tryLock (slotLock);
    if (bypassP->load() < 0.5f && tryLock.isLocked())
    {
        const int count = juce::jlimit (0, kNumSlots, (int) countP->load());
        for (int s = 0; s < kNumSlots; ++s)
        {
            const int type = (int) typeP[s]->load();
            if (s >= count || type <= 0 || type >= NumTypes || type == BusComp)
            { slotMeter[s] = 0.f; slotLevel[s] = 0.f; lastType[s] = 0; continue; }
            runSlot (s, type, buffer);
        }
        runSlot (kMasterSlot, BusComp, buffer);      // le bus compresseur est toujours en fin de chaîne
    }
    else
        for (int s = 0; s < kTotalSlots; ++s) { slotMeter[s] = 0.f; slotLevel[s] = 0.f; }

    const float outG = juce::Decibels::decibelsToGain (outP->load());
    buffer.applyGainRamp (0, n, lastOut, outG);
    lastOut = outG;

    pk = 0.f;
    for (int c = 0; c < buffer.getNumChannels(); ++c) pk = juce::jmax (pk, buffer.getMagnitude (c, 0, n));
    outPeak.store (pk);
}

//==============================================================================
void RackProcessor::runSlot (int s, int type, juce::AudioBuffer<float>& buffer)
{
    auto& m = mods[(size_t) s][(size_t) type];
    if (m == nullptr) return;
    if (type != lastType[s]) { m->reset(); lastType[s] = type; }
    if (onP[s]->load() < 0.5f) { slotMeter[s] = 0.f; slotLevel[s] = 0.f; return; }

    const auto& inf = info (type);
    float v[kMaxParams] {};
    for (size_t j = 0; j < inf.params.size(); ++j) v[j] = inf.params[j].toReal (pP[s][j]->load());

    const int n = buffer.getNumSamples(), nc = juce::jmin (2, buffer.getNumChannels());
    auto& sc = scopes[s];
    const int w = sc.pos.load();
    constexpr int mask = Scope::N - 1;
    for (int i = 0; i < n; ++i)
        sc.inM[(size_t) ((w + i) & mask)] = nc > 1 ? 0.5f * (buffer.getSample (0, i) + buffer.getSample (1, i)) : buffer.getSample (0, i);

    m->process (buffer, v);

    for (int i = 0; i < n; ++i)
    {
        sc.outL[(size_t) ((w + i) & mask)] = buffer.getSample (0, i);
        sc.outR[(size_t) ((w + i) & mask)] = nc > 1 ? buffer.getSample (1, i) : buffer.getSample (0, i);
    }
    sc.pos.store ((w + n) & mask);
    slotMeter[s] = m->meter;
    slotLevel[s] = m->level;
}

//==============================================================================
void RackProcessor::setNorm (const juce::String& id, float norm)
{
    if (auto* p = apvts.getParameter (id)) p->setValueNotifyingHost (juce::jlimit (0.f, 1.f, norm));
}
float RackProcessor::getNorm (const juce::String& id) const
{
    if (auto* p = apvts.getParameter (id)) return p->getValue();
    return 0.f;
}

void RackProcessor::setSlotType (int slot, int type, bool resetToDefaults)
{
    type = juce::jlimit (0, NumTypes - 1, type);
    setNorm (typeId (slot), (float) type / (float) (NumTypes - 1));
    if (! resetToDefaults) return;
    const auto& inf = info (type);
    for (int j = 0; j < kMaxParams; ++j)
        setNorm (paramId (slot, j), j < (int) inf.params.size() ? inf.params[(size_t) j].toNorm (inf.params[(size_t) j].def) : 0.5f);
    setNorm (onId (slot), 1.f);
}

void RackProcessor::setCount (int n)
{
    if (auto* p = apvts.getParameter ("count"))
        p->setValueNotifyingHost (p->convertTo0to1 ((float) juce::jlimit (0, kNumSlots, n)));
}

void RackProcessor::copySlot (int from, int to)
{
    setNorm (typeId (to), getNorm (typeId (from)));
    setNorm (onId (to), getNorm (onId (from)));
    for (int j = 0; j < kMaxParams; ++j) setNorm (paramId (to, j), getNorm (paramId (from, j)));
}

void RackProcessor::swapSlotsInternal (int a, int b)
{
    for (const auto& pair : { std::make_pair (typeId (a), typeId (b)), std::make_pair (onId (a), onId (b)) })
    {
        const float va = getNorm (pair.first), vb = getNorm (pair.second);
        setNorm (pair.first, vb); setNorm (pair.second, va);
    }
    for (int j = 0; j < kMaxParams; ++j)
    {
        const float va = getNorm (paramId (a, j)), vb = getNorm (paramId (b, j));
        setNorm (paramId (a, j), vb); setNorm (paramId (b, j), va);
    }
}

bool RackProcessor::addModule (int type)
{
    const juce::ScopedLock sl (slotLock);
    const int n = getNumModules();
    if (n >= kNumSlots || type <= 0) return false;
    setSlotType (n, type, true);
    setCount (n + 1);
    return true;
}

void RackProcessor::removeModule (int slot)
{
    const juce::ScopedLock sl (slotLock);
    const int n = getNumModules();
    if (slot < 0 || slot >= n) return;
    for (int i = slot; i < n - 1; ++i) copySlot (i + 1, i);
    setSlotType (n - 1, 0, true);
    setCount (n - 1);
}

void RackProcessor::moveModule (int slot, int dir)
{
    const juce::ScopedLock sl (slotLock);
    const int n = getNumModules(), other = slot + dir;
    if (slot < 0 || slot >= n || other < 0 || other >= n) return;
    swapSlotsInternal (slot, other);
}

//==============================================================================
namespace
{
struct PS { int type; std::vector<std::pair<int, float>> vals; };   // (index du réglage, valeur réelle)
struct Preset { const char* name; std::vector<PS> slots; };

const std::vector<Preset>& presets()
{
    static const std::vector<Preset> p = {
        { "Empty rack", {} },
        { "Vocal chain", {
            { Filters,    { { 0, 90.f } } },
            { Preamp,     { { 0, 1.f }, { 1, 30.f } } },
            { ConsoleEQ,  { { 0, 1.f }, { 1, 1.5f }, { 2, -2.f }, { 3, 350.f }, { 4, 2.f }, { 5, 3500.f }, { 6, 3.f } } },
            { Compressor, { { 0, 1.f }, { 1, -22.f }, { 2, 3.5f }, { 3, 10.f }, { 4, 250.f }, { 5, 4.f } } },
            { DeEsser,    { { 0, 7000.f }, { 1, -28.f }, { 2, 6.f } } },
            { Saturation, { { 0, 0.f }, { 1, 5.f }, { 3, 40.f } } },
            { Reverb,     { { 0, 35.f }, { 1, 50.f }, { 3, 10.f } } },
            { Limiter,    { { 1, -1.f } } } } },
        { "Drum bus", {
            { Filters,    { { 0, 30.f } } },
            { ConsoleEQ,  { { 0, 2.f }, { 1, 3.f }, { 6, 2.f } } },
            { Compressor, { { 0, 0.f }, { 1, -16.f }, { 2, 4.f }, { 3, 10.f }, { 4, 100.f }, { 5, 4.f } } },
            { Saturation, { { 0, 0.f }, { 1, 8.f }, { 3, 70.f } } },
            { Limiter,    { { 1, -1.f } } } } },
        { "Mix bus", {
            { ConsoleEQ,  { { 0, 0.f }, { 1, 1.f }, { 6, 1.5f } } },
            { Compressor, { { 0, 0.f }, { 1, -14.f }, { 2, 2.f }, { 3, 30.f }, { 4, 200.f }, { 5, 2.f } } },
            { Saturation, { { 0, 0.f }, { 1, 3.f }, { 3, 50.f } } },
            { Stereo,     { { 0, 110.f } } },
            { Limiter,    { { 1, -0.5f } } } } }
    };
    return p;
}
}

int RackProcessor::numPresets() { return (int) presets().size(); }
const char* RackProcessor::presetName (int i) { return presets()[(size_t) juce::jlimit (0, numPresets() - 1, i)].name; }

void RackProcessor::loadPreset (int index)
{
    const juce::ScopedLock sl (slotLock);
    for (int s = 0; s < kNumSlots; ++s) setSlotType (s, 0, true);
    setCount (0);
    const auto& pr = presets()[(size_t) juce::jlimit (0, numPresets() - 1, index)];
    for (size_t s = 0; s < pr.slots.size() && (int) s < kNumSlots; ++s)
    {
        const auto& ps = pr.slots[s];
        setSlotType ((int) s, ps.type, true);
        const auto& inf = info (ps.type);
        for (auto& kv : ps.vals)
            if (kv.first < (int) inf.params.size())
                setNorm (paramId ((int) s, kv.first), inf.params[(size_t) kv.first].toNorm (kv.second));
        setCount ((int) s + 1);
    }
}

//==============================================================================
void RackProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml()) copyXmlToBinary (*xml, dest);
}

void RackProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* RackProcessor::createEditor() { return new RackEditor (*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new RackProcessor(); }
