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
    for (int k = 0; k < 80; ++k)
    {
        for (int c = 0; c < 2; ++c)
            for (int i = 0; i < 512; ++i)
                b.setSample (c, i, 0.35f * std::sin (0.07f * (float) (i + k * 512)) + 0.1f * (r.nextFloat() - 0.5f));
        p.processBlock (b, m);
    }
}

static void snap (RackProcessor& p, const String& name)
{
    feed (p);
    std::unique_ptr<AudioProcessorEditor> ed (p.createEditor());
    MessageManager::getInstance()->runDispatchLoopUntil (300);
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

    // Chaîne A : Neve / Opto / FET / ProEQ / Tape...
    p.addModule (Preamp);      setReal (p, 0, 0, 1); setReal (p, 0, 1, 55);
    p.addModule (ConsoleEQ);   setReal (p, 1, 0, 1);
    p.addModule (Compressor);  setReal (p, 2, 0, 1); setReal (p, 2, 1, -30); setReal (p, 2, 2, 6);
    p.addModule (Compressor);  setReal (p, 3, 0, 2); setReal (p, 3, 1, -30); setReal (p, 3, 2, 8);
    p.addModule (ProEQ);       setReal (p, 4, 1, 5); setReal (p, 4, 4, -4); setReal (p, 4, 10, 6);
    p.addModule (Saturation);  setReal (p, 5, 0, 0);
    p.addModule (Delay);
    p.addModule (Limiter);
    snap (p, "A_neve_opto_fet");

    // Chaîne B : API / SSL
    p.loadPreset (0);
    p.addModule (Preamp);      setReal (p, 0, 0, 2); setReal (p, 0, 1, 60);
    p.addModule (ConsoleEQ);   setReal (p, 1, 0, 2);
    p.addModule (Compressor);  setReal (p, 2, 0, 0); setReal (p, 2, 1, -30); setReal (p, 2, 2, 8);
    p.addModule (Preamp);      setReal (p, 3, 0, 0); setReal (p, 3, 1, 40);
    p.addModule (ConsoleEQ);   setReal (p, 4, 0, 0);
    p.addModule (Gate);
    p.addModule (Saturation);  setReal (p, 6, 0, 1);
    p.addModule (Saturation);  setReal (p, 7, 0, 3);
    snap (p, "B_api_ssl");

    // Test de déplacement / suppression
    std::cout << "avant   : " << dump (p) << std::endl;
    p.moveModule (0, +1);  std::cout << "move 0>  : " << dump (p) << std::endl;
    p.moveModule (7, -1);  std::cout << "move 7<  : " << dump (p) << std::endl;
    p.moveModule (7, +1);  std::cout << "move 7>  : " << dump (p) << " (inchange attendu)" << std::endl;
    p.removeModule (2);    std::cout << "remove 2 : " << dump (p) << " n=" << p.getNumModules() << std::endl;
    p.addModule (rack::Reverb);  std::cout << "add      : " << dump (p) << " n=" << p.getNumModules() << std::endl;
    // le modèle suit-il le module déplacé ? (Preamp API doit rester API)
    for (int i = 0; i < p.getNumModules(); ++i)
        if (p.getSlotType (i) == Preamp)
            std::cout << "  Preamp en slot " << i << " modele="
                      << info (Preamp).params[0].toReal (p.apvts.getParameter (RackProcessor::paramId (i, 0))->getValue()) << std::endl;
    snap (p, "C_after_moves");

    p.loadPreset (1);
    snap (p, "D_vocal_chain");
    return 0;
}
