// Outil de test : génère des captures PNG de l'interface et vérifie les déplacements de modules.
#include "../Source/PluginProcessor.h"
#include "../Source/PluginEditor.h"
using namespace juce;
using namespace rack;

static void setReal (RackProcessor& p, int slot, int j, float real)
{
    const auto& pd = info (p.getSlotType (slot)).params[(size_t) j];
    p.apvts.getParameter (RackProcessor::paramId (slot, j))->setValueNotifyingHost (pd.toNorm (real));
}

static void feed (RackProcessor& p)
{
    AudioBuffer<float> b (2, 512);
    MidiBuffer m;
    Random r (3);
    for (int k = 0; k < 90; ++k)
    {
        for (int i = 0; i < 512; ++i)
        {
            const float t = (float) (i + k * 512);
            const float n = 0.12f * (r.nextFloat() - 0.5f);
            b.setSample (0, i, 0.35f * std::sin (0.07f * t) + 0.12f * std::sin (0.31f * t) + n);
            b.setSample (1, i, 0.30f * std::sin (0.07f * t + 0.9f) + 0.12f * std::sin (0.31f * t - 0.5f) + 0.12f * (r.nextFloat() - 0.5f));
        }
        p.processBlock (b, m);
    }
}

static void snap (RackProcessor& p, const String& name, int scroll = 0)
{
    feed (p);
    std::unique_ptr<AudioProcessorEditor> ed (p.createEditor());
    if (auto* re = dynamic_cast<RackEditor*> (ed.get())) re->scrollRackTo (scroll);
    for (int i = 0; i < 4; ++i) { feed (p); MessageManager::getInstance()->runDispatchLoopUntil (120); }
    auto img = ed->createComponentSnapshot (ed->getLocalBounds(), true, 1.0f);
    File f (File ("/tmp/snaps").getChildFile (name + ".png"));
    f.getParentDirectory().createDirectory(); f.deleteFile();
    FileOutputStream os (f);
    PNGImageFormat().writeImageToStream (img, os);
    std::cout << name << " : " << ed->getWidth() << "x" << ed->getHeight() << std::endl;
}

static String dump (RackProcessor& p)
{
    String s;
    for (int i = 0; i < p.getNumModules(); ++i) s << info (p.getSlotType (i)).name << " | ";
    return s;
}

int main()
{
    ScopedJuceInitialiser_GUI init;
    RackProcessor p;
    p.setPlayConfigDetails (2, 2, 48000, 512);
    p.prepareToPlay (48000, 512);

    snap (p, "0_empty");

    p.loadPreset(0);
    for (auto t : { Preamp, ProEQ, Compressor, rack::Reverb }) p.addModule(t);
    snap(p, "atelier500");
    // 1 : Neve / Opto / FET / ProEQ + bus SSL
    p.addModule (Preamp);      setReal (p, 0, 0, 1); setReal (p, 0, 1, 55);
    p.addModule (ConsoleEQ);   setReal (p, 1, 0, 1); setReal (p, 1, 1, 4); setReal (p, 1, 6, 5);
    p.addModule (Compressor);  setReal (p, 2, 0, 1); setReal (p, 2, 1, -30); setReal (p, 2, 2, 6);
    p.addModule (Compressor);  setReal (p, 3, 0, 2); setReal (p, 3, 1, -30); setReal (p, 3, 2, 8);
    p.addModule (ProEQ);       setReal (p, 4, 1, 5); setReal (p, 4, 4, -4); setReal (p, 4, 10, 6);
    setReal (p, kMasterSlot, 0, 0); setReal (p, kMasterSlot, 1, -20); setReal (p, kMasterSlot, 2, 4);
    snap (p, "1_neve_opto_fet_bus_ssl");

    // 2 : API / SSL
    p.loadPreset (0);
    p.addModule (Preamp);      setReal (p, 0, 0, 2); setReal (p, 0, 1, 60);
    p.addModule (ConsoleEQ);   setReal (p, 1, 0, 2); setReal (p, 1, 1, 6); setReal (p, 1, 4, -3);
    p.addModule (Compressor);  setReal (p, 2, 0, 0); setReal (p, 2, 1, -30); setReal (p, 2, 2, 8);
    p.addModule (Saturation);  setReal (p, 3, 0, 1); setReal (p, 3, 1, 14);
    p.addModule (Filters);     setReal (p, 4, 0, 120); setReal (p, 4, 1, 9000); setReal (p, 4, 2, 1);
    setReal (p, kMasterSlot, 0, 2); setReal (p, kMasterSlot, 1, -20);
    snap (p, "2_api_ssl_bus_api");

    // 3 : Stereo / Delay / Reverb / Limiter / De-esser
    p.loadPreset (0);
    p.addModule (Stereo);      setReal (p, 0, 0, 150); setReal (p, 0, 1, 25);
    p.addModule (Delay);       setReal (p, 1, 4, 1); setReal (p, 1, 1, 60);
    p.addModule (rack::Reverb);
    p.addModule (Limiter);     setReal (p, 3, 0, 10); setReal (p, 3, 1, -6);
    p.addModule (DeEsser);     setReal (p, 4, 1, -40);
    setReal (p, kMasterSlot, 0, 1); setReal (p, kMasterSlot, 1, -20);
    snap (p, "3_stereo_delay_limiter_bus_neve");

    // 4 : beaucoup de modules + défilement
    p.loadPreset (0);
    for (int i = 0; i < 12; ++i) p.addModule (i % 2 == 0 ? Compressor : ConsoleEQ);
    std::cout << "12 modules : n=" << p.getNumModules() << " (max " << kNumSlots << ")" << std::endl;
    snap (p, "4_scrolled", 7 * 170);

    // Test de déplacement / suppression
    p.loadPreset (0);
    p.addModule (Preamp); setReal (p, 0, 0, 2);
    p.addModule (ConsoleEQ); p.addModule (Compressor); p.addModule (Gate);
    std::cout << "avant   : " << dump (p) << std::endl;
    p.moveModule (0, +1);  std::cout << "move 0>  : " << dump (p) << std::endl;
    p.moveModule (3, +1);  std::cout << "move 3> (fin) : " << dump (p) << std::endl;
    p.removeModule (1);    std::cout << "remove 1 : " << dump (p) << " n=" << p.getNumModules() << std::endl;
    for (int i = 0; i < p.getNumModules(); ++i)
        if (p.getSlotType (i) == Preamp)
            std::cout << "  Preamp en slot " << i << " modele="
                      << info (Preamp).params[0].toReal (p.apvts.getParameter (RackProcessor::paramId (i, 0))->getValue()) << " (API=2)" << std::endl;
    std::cout << "master type=" << p.getSlotType (kMasterSlot) << " (BusComp=" << (int) BusComp << ")" << std::endl;

    p.loadPreset (1);
    snap (p, "5_vocal_chain");
    return 0;
}
