#include "PluginEditor.h"

using namespace juce;
using namespace rack;

//==============================================================================
// SKINS : l'apparence dépend du module et du modèle choisi
//==============================================================================
Skin skinFor (int type, int model)
{
    Skin s;
    auto C = [] (uint32 v) { return Colour (v); };
    s.text = Colours::white; s.legend = Colours::white.withAlpha (0.9f); s.accent = C (0xff6fe8a8);
    const Colour base (info (type).colour);
    s.top = base.brighter (0.12f); s.bottom = base.darker (0.35f);
    s.knobStyle = 5;

    switch (type)
    {
        case Preamp:
        case ConsoleEQ:
            s.display = (type == Preamp) ? 1 : 0;
            if (model == 1)        // NEVE : gris-bleu, boutons Marconi
            {
                s.top = C (0xff93a7b8); s.bottom = C (0xff6c8196); s.text = Colours::white; s.legend = Colours::white;
                s.knobStyle = 0; s.accent = C (0xffff5a4a);
            }
            else if (model == 2)   // API : face noire, légendes bleues, boutons aluminium
            {
                s.top = C (0xff222427); s.bottom = C (0xff101113); s.text = Colours::white; s.legend = C (0xff7db8ff);
                s.knobStyle = 2; s.accent = C (0xff5fa8ff);
            }
            else                   // SSL : gris, capuchons colorés
            {
                s.top = C (0xffa2a5a8); s.bottom = C (0xff82868a); s.text = C (0xff15181a); s.legend = C (0xff15181a);
                s.knobStyle = 1; s.accent = C (0xffe2362d);
            }
            break;

        case Compressor:
            if (model == 1)        // OPTO : aluminium brossé, bakélite, VU crème
            {
                s.top = C (0xffd4d7d9); s.bottom = C (0xffa5a9ac); s.text = C (0xff1a1c1e); s.legend = C (0xff1a1c1e);
                s.knobStyle = 3; s.display = 2; s.brushed = true; s.accent = C (0xffc0392b);
            }
            else if (model == 2)   // FET : face noire, dessus argent, VU avec liseré bleu
            {
                s.top = C (0xff1c1d1f); s.bottom = C (0xff0c0d0e); s.text = C (0xffeeeeee); s.legend = C (0xffdcdcdc);
                s.knobStyle = 4; s.display = 3; s.hasStripe = true; s.stripe = C (0xff2c5f9e); s.accent = C (0xffff9a3c);
            }
            else                   // VCA : gris console, capuchons colorés, LED
            {
                s.top = C (0xffaaaaa2); s.bottom = C (0xff8b8b84); s.text = C (0xff17191a); s.legend = C (0xff17191a);
                s.knobStyle = 1; s.display = 4; s.accent = C (0xffe2362d);
            }
            break;

        case Saturation:
            s.accent = C (0xffffb23e);
            if (model == 1)        { s.top = C (0xff62402a); s.bottom = C (0xff3b2517); s.text = C (0xfff0e2c4); s.legend = C (0xfff0e2c4); s.knobStyle = 5; }
            else if (model == 2)   { s.top = C (0xff2a3a55); s.bottom = C (0xff182134); s.knobStyle = 0; }
            else if (model == 3)   { s.top = C (0xff2d2d30); s.bottom = C (0xff18181a); s.legend = C (0xffff9a4a); s.accent = C (0xffff9a4a); s.knobStyle = 4; }
            else                   { s.top = C (0xff414f49); s.bottom = C (0xff29332f); s.text = C (0xffe9e4d0); s.legend = C (0xffe9e4d0); s.knobStyle = 4; }
            break;

        case ProEQ:
            s.top = C (0xff34383d); s.bottom = C (0xff1b1d20); s.knobStyle = 2; s.display = 5; s.accent = C (0xff4ad6ff); s.legend = C (0xff9fdcff);
            break;
        case Gate:     s.display = 4; s.accent = C (0xff7be36f); break;
        case DeEsser:  s.display = 4; s.accent = C (0xffd28bff); break;
        case Filters:  s.top = C (0xff565a5e); s.bottom = C (0xff34373a); s.knobStyle = 1; break;
        default: break;
    }
    return s;
}

Colour capFor (int type, int model, int j)
{
    auto C = [] (uint32 v) { return Colour (v); };
    const Colour red = C (0xffd5372f), blue = C (0xff2f6fd0), green = C (0xff2e9d57), black = C (0xff1d1d1f),
                 white = C (0xfff2f2f2), grey = C (0xffbfc2c6), navy = C (0xff2b4f9a), neveRed = C (0xffb3262a);
    switch (type)
    {
        case Preamp:
            if (model == 1) return j == 1 ? neveRed : (j == 2 ? grey : navy);
            if (model == 2) return j == 1 ? blue : (j == 2 ? white : black);
            return j == 1 ? red : (j == 2 ? blue : black);
        case ConsoleEQ:
            if (model == 1) return (j == 3 || j == 5) ? navy : grey;
            if (model == 2) return (j == 3 || j == 5) ? blue : white;
            if (j == 1) return black;
            if (j == 2 || j == 3) return blue;
            if (j == 4 || j == 5) return green;
            return red;
        case Compressor:
            if (model == 1) return C (0xff141414);
            if (model == 2) return C (0xffc9ccd0);
            return j == 1 ? black : (j == 2 ? blue : (j == 3 || j == 4 ? white : (j == 5 ? red : green)));
        case ProEQ:     return (j % 3 == 1) ? white : blue;
        case Filters:   return white;
        case Saturation:
            if (model == 2) return grey;
            if (model == 0 || model == 3) return C (0xffc9ccd0);
            return C (0xffe9dcb8);
        default:        return C (0xffe6dcbf);
    }
}

//==============================================================================
// LOOK & FEEL
//==============================================================================
static Colour readCol (const Slider& s, const char* key, uint32 def)
{
    const var v = s.getProperties().getWithDefault (key, var ((int64) def));
    return Colour ((uint32) (int64) v);
}

RackLnF::RackLnF()
{
    setColour (ComboBox::backgroundColourId, Colour (0xff15171a));
    setColour (ComboBox::textColourId, Colours::white);
    setColour (ComboBox::outlineColourId, Colour (0xff555a60));
    setColour (ComboBox::arrowColourId, Colours::white);
    setColour (PopupMenu::backgroundColourId, Colour (0xff1c1f23));
    setColour (PopupMenu::textColourId, Colours::white);
    setColour (PopupMenu::headerTextColourId, Colour (0xffffb23e));
    setColour (PopupMenu::highlightedBackgroundColourId, Colour (0xff3a6ea5));
    setColour (Label::textColourId, Colours::white);
    setColour (Slider::textBoxTextColourId, Colour (0xffe8e8e8));
    setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
    setColour (TextButton::buttonColourId, Colour (0xff15171a));
    setColour (TextButton::textColourOffId, Colours::white);
}

Font RackLnF::getLabelFont (Label& l)
{
    if (dynamic_cast<Slider*> (l.getParentComponent()) != nullptr)
        return Font (FontOptions (11.5f, Font::bold));
    return Font (FontOptions (l.getFont().getHeight()));
}

void RackLnF::drawButtonBackground (Graphics& g, Button& b, const Colour&, bool hl, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced (0.5f);
    g.setGradientFill (ColourGradient (Colour (down ? 0xff111214 : 0xff3a3d41), 0, r.getY(), Colour (0xff17181a), 0, r.getBottom(), false));
    g.fillRoundedRectangle (r, 3.f);
    g.setColour (hl ? Colour (0xffffb23e) : Colour (0xff050506));
    g.drawRoundedRectangle (r, 3.f, 1.f);
}

static Colour readColourProp (const Component& c)
{
    const var v = c.getProperties().getWithDefault ("led", var ((int64) 0xff4cff7a));
    return Colour ((uint32) (int64) v);
}

static void pointer (Graphics& g, float cx, float cy, float ang, float r0, float r1, Colour c, float thick)
{
    g.setColour (c);
    g.drawLine (cx + std::sin (ang) * r0, cy - std::cos (ang) * r0, cx + std::sin (ang) * r1, cy - std::cos (ang) * r1, thick);
}

static void knurl (Graphics& g, float cx, float cy, float r0, float r1, int n, Colour c, float thick = 1.f)
{
    g.setColour (c);
    for (int i = 0; i < n; ++i)
        pointer (g, cx, cy, MathConstants<float>::twoPi * (float) i / (float) n, r0, r1, c, thick);
}

void RackLnF::drawRotarySlider (Graphics& g, int x, int y, int w, int h, float pos, float a0, float a1, Slider& s)
{
    const int style = (int) s.getProperties().getWithDefault ("style", 5);
    const Colour cap = readCol (s, "cap", 0xffe3d9bb), legend = readCol (s, "legend", 0xffffffff);
    const float R = jmin (w, h) * 0.5f - 1.f;
    const float cx = x + w * 0.5f, cy = y + h * 0.5f;
    const float ang = a0 + pos * (a1 - a0);
    const float kr = R - 8.f;

    // graduations imprimées sur la face
    for (int i = 0; i <= 10; ++i)
    {
        const float a = a0 + (a1 - a0) * (float) i / 10.f;
        const float len = (i == 0 || i == 5 || i == 10) ? 6.f : 3.5f;
        pointer (g, cx, cy, a, R - len, R, legend.withAlpha (0.85f), 1.3f);
    }

    // ombre portée
    g.setColour (Colours::black.withAlpha (0.45f));
    g.fillEllipse (cx - kr + 1.5f, cy - kr + 3.f, kr * 2, kr * 2);

    auto disc = [&g, cx, cy] (float r, Colour c1, Colour c2)
    {
        g.setGradientFill (ColourGradient (c1, cx - r * 0.5f, cy - r, c2, cx + r * 0.4f, cy + r, false));
        g.fillEllipse (cx - r, cy - r, r * 2, r * 2);
    };
    auto rim = [&g, cx, cy] (float r, Colour c, float t = 1.f) { g.setColour (c); g.drawEllipse (cx - r, cy - r, r * 2, r * 2, t); };

    switch (style)
    {
        case 0: // Marconi à jupe, cannelé
        {
            disc (kr, cap.darker (0.55f), cap.darker (0.25f));
            rim (kr, Colours::black.withAlpha (0.6f));
            const float cr = kr * 0.78f;
            disc (cr, cap.brighter (0.3f), cap.darker (0.3f));
            knurl (g, cx, cy, cr * 0.88f, cr, 30, Colours::black.withAlpha (0.35f), 1.2f);
            rim (cr, Colours::black.withAlpha (0.55f));
            pointer (g, cx, cy, ang, kr * 0.12f, kr * 0.72f, Colours::white, 2.4f);
            g.setColour (Colours::white.withAlpha (0.18f));
            g.fillEllipse (cx - cr * 0.55f, cy - cr * 0.85f, cr * 0.8f, cr * 0.35f);
            break;
        }
        case 1: // capuchon SSL
        {
            disc (kr, Colour (0xff2b2d30), Colour (0xff0e0f10));
            const float cr = kr * 0.9f;
            disc (cr, cap.brighter (0.25f), cap.darker (0.35f));
            knurl (g, cx, cy, cr * 0.9f, cr, 40, Colours::black.withAlpha (0.3f), 1.f);
            rim (cr, Colours::black.withAlpha (0.7f));
            pointer (g, cx, cy, ang, kr * 0.2f, kr * 0.85f, cap.getPerceivedBrightness() > 0.55f ? Colours::black : Colours::white, 2.6f);
            break;
        }
        case 2: // aluminium usiné + insert coloré
        {
            disc (kr, Colour (0xfff0f2f4), Colour (0xff80858a));
            knurl (g, cx, cy, kr * 0.9f, kr, 56, Colours::black.withAlpha (0.28f), 1.f);
            rim (kr, Colours::black.withAlpha (0.65f));
            const float ir = kr * 0.5f;
            disc (ir, cap.brighter (0.2f), cap.darker (0.4f));
            rim (ir, Colours::black.withAlpha (0.6f));
            pointer (g, cx, cy, ang, kr * 0.58f, kr * 0.97f, Colours::black, 3.f);
            break;
        }
        case 3: // bakélite noire
        {
            disc (kr, Colour (0xff4b4b4d), Colour (0xff070708));
            rim (kr, Colour (0xffb6babd), 1.6f);
            disc (kr * 0.86f, Colour (0xff2e2e30), Colour (0xff0a0a0b));
            pointer (g, cx, cy, ang, kr * 0.1f, kr * 0.8f, Colours::white, 3.f);
            g.setColour (Colours::white.withAlpha (0.14f));
            g.fillEllipse (cx - kr * 0.55f, cy - kr * 0.85f, kr * 0.9f, kr * 0.34f);
            break;
        }
        case 4: // FET : noir à graduation, dessus argent
        {
            disc (kr, Colour (0xff35363a), Colour (0xff09090a));
            knurl (g, cx, cy, kr * 0.9f, kr, 48, Colours::white.withAlpha (0.1f), 1.f);
            rim (kr, Colours::black, 1.2f);
            const float ir = kr * 0.64f;
            disc (ir, Colour (0xfff2f4f5), Colour (0xff8a8f94));
            rim (ir, Colours::black.withAlpha (0.55f));
            pointer (g, cx, cy, ang, kr * 0.08f, ir * 0.95f, Colours::black, 2.6f);
            pointer (g, cx, cy, ang, kr * 0.72f, kr * 0.96f, Colours::white, 2.f);
            break;
        }
        default: // ivoire vintage
        {
            disc (kr, cap.brighter (0.15f), cap.darker (0.45f));
            rim (kr, Colours::black.withAlpha (0.6f));
            disc (kr * 0.82f, cap.brighter (0.3f), cap.darker (0.15f));
            pointer (g, cx, cy, ang, kr * 0.12f, kr * 0.78f, Colour (0xff3a2a18), 2.6f);
            break;
        }
    }
}

void RackLnF::drawToggleButton (Graphics& g, ToggleButton& b, bool, bool)
{
    auto r = b.getLocalBounds().toFloat();
    const bool on = b.getToggleState();
    const Colour led = readColourProp (b);
    const float d = jmin (r.getWidth(), r.getHeight()) - 8.f;
    auto c = r.getCentre();
    if (b.getButtonText().isNotEmpty() && b.getProperties().contains ("textLeft")) c = { r.getX() + d * 0.5f + 4.f, r.getCentreY() };
    g.setColour (Colours::black.withAlpha (0.65f));
    g.fillEllipse (c.x - d / 2 - 2.5f, c.y - d / 2 - 2.5f, d + 5, d + 5);
    g.setColour (on ? led : led.darker (0.85f).withAlpha (0.8f));
    g.fillEllipse (c.x - d / 2, c.y - d / 2, d, d);
    g.setColour (Colours::white.withAlpha (on ? 0.4f : 0.1f));
    g.fillEllipse (c.x - d * 0.28f, c.y - d * 0.42f, d * 0.4f, d * 0.25f);
    if (on)
    {
        g.setColour (led.withAlpha (0.22f));
        g.fillEllipse (c.x - d, c.y - d, d * 2, d * 2);
    }
    if (b.getButtonText().isNotEmpty())
    {
        g.setColour (Colours::white);
        g.setFont (Font (FontOptions (11.f, Font::bold)));
        g.drawText (b.getButtonText(), r.withTrimmedLeft (d + 12).toNearestInt(), Justification::centredLeft);
    }
}

//==============================================================================
// SÉLECTEUR À BOUTONS
//==============================================================================
void ChoiceButtons::setup (const StringArray& items, int sel, const Skin& sk)
{
    names = items; selected = sel; skin = sk; repaint();
}

bool ChoiceButtons::useTwoRows() const
{
    int longest = 0;
    for (auto& nm : names) longest = jmax (longest, nm.length());
    const int n = names.size();
    return (n > 3 && getWidth() < 110) || (n > 1 && getWidth() < 95 && longest > 5);
}

Rectangle<float> ChoiceButtons::segment (int i) const
{
    const int n = names.size();
    const bool twoRows = useTwoRows();
    const int cols = twoRows ? (n + 1) / 2 : n, rows = twoRows ? 2 : 1;
    auto r = getLocalBounds().toFloat().reduced (1.f);
    const float cw = r.getWidth() / (float) cols, ch = r.getHeight() / (float) rows;
    return Rectangle<float> (r.getX() + (float) (i % cols) * cw, r.getY() + (float) (i / cols) * ch, cw, ch).reduced (1.5f);
}

void ChoiceButtons::paint (Graphics& g)
{
    for (int i = 0; i < names.size(); ++i)
    {
        const auto r = segment (i);
        const bool on = (i == selected);
        if (on)
        {
            g.setGradientFill (ColourGradient (skin.accent.brighter (0.35f), 0, r.getY(), skin.accent.darker (0.3f), 0, r.getBottom(), false));
            g.fillRoundedRectangle (r, 3.f);
            g.setColour (skin.accent.withAlpha (0.25f));
            g.drawRoundedRectangle (r.expanded (1.5f), 4.f, 2.f);
        }
        else
        {
            g.setGradientFill (ColourGradient (Colour (0xff45484d), 0, r.getY(), Colour (0xff17181a), 0, r.getBottom(), false));
            g.fillRoundedRectangle (r, 3.f);
        }
        g.setColour (Colours::black.withAlpha (0.8f));
        g.drawRoundedRectangle (r, 3.f, 1.f);
        g.setColour (on ? Colour (0xff101010) : Colour (0xffd8d8d8));
        g.setFont (Font (FontOptions (names.size() > 3 ? 9.5f : 10.5f, Font::bold)));
        g.drawText (names[i].toUpperCase(), r.toNearestInt(), Justification::centred);
    }
}

void ChoiceButtons::mouseDown (const MouseEvent& e)
{
    for (int i = 0; i < names.size(); ++i)
        if (segment (i).contains (e.position))
        {
            selected = i; repaint();
            if (onSelect) onSelect (i);
            return;
        }
}

//==============================================================================
// PANNEAU DE MODULE
//==============================================================================
ModulePanel::ModulePanel (RackProcessor& p, int s) : proc (p), slot (s)
{
    addAndMakeVisible (powerBtn);
    onAtt = std::make_unique<ButtonParameterAttachment> (*proc.apvts.getParameter (RackProcessor::onId (slot)), powerBtn);

    for (auto* b : { &leftBtn, &rightBtn, &removeBtn }) addAndMakeVisible (*b);
    leftBtn.onClick   = [this] { if (onMove) onMove (slot, -1); };
    rightBtn.onClick  = [this] { if (onMove) onMove (slot, +1); };
    removeBtn.onClick = [this] { if (onRemove) onRemove (slot); };
    leftBtn.setTooltip ("Move left");
    rightBtn.setTooltip ("Move right");
    removeBtn.setTooltip ("Remove this module");

    for (int j = 0; j < kMaxParams; ++j)
    {
        auto& c = cells[(size_t) j];
        auto* prm = proc.apvts.getParameter (RackProcessor::paramId (slot, j));

        c.slider.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        c.slider.setRotaryParameters (MathConstants<float>::pi * 1.2f, MathConstants<float>::pi * 2.8f, true);
        c.slider.setTextBoxStyle (Slider::TextBoxBelow, false, kWidth / 2 - 4, 14);
        c.slider.setVelocityBasedMode (false);
        c.sAtt = std::make_unique<SliderParameterAttachment> (*prm, c.slider);
        addChildComponent (c.slider);

        c.label.setJustificationType (Justification::centred);
        c.label.setFont (Font (FontOptions (10.5f, Font::bold)));
        addChildComponent (c.label);
        addChildComponent (c.choice);

        c.cAtt = std::make_unique<ParameterAttachment> (*prm, [this, j] (float v)
        {
            if (type <= 0 || j >= (int) info (type).params.size()) return;
            const auto& pd = info (type).params[(size_t) j];
            if (pd.kind != Kind::Choice) return;
            cells[(size_t) j].choice.setSelected (roundToInt (pd.toReal (v)));
            if (j == 0 && currentModel() != model) { model = currentModel(); applySkin(); }
        });
        c.choice.onSelect = [this, j] (int idx)
        {
            if (type <= 0 || j >= (int) info (type).params.size()) return;
            const auto& pd = info (type).params[(size_t) j];
            cells[(size_t) j].cAtt->setValueAsCompleteGesture (pd.toNorm ((float) idx));
        };
    }

    typeAtt = std::make_unique<ParameterAttachment> (*proc.apvts.getParameter (RackProcessor::typeId (slot)),
                                                     [this] (float) { refresh(); });
    refresh();
    startTimerHz (30);
}

ModulePanel::~ModulePanel() { stopTimer(); }

int ModulePanel::currentModel() const
{
    if (type <= 0 || info (type).params.empty() || info (type).params[0].kind != Kind::Choice) return 0;
    return roundToInt (info (type).params[0].toReal (proc.apvts.getParameter (RackProcessor::paramId (slot, 0))->getValue()));
}

int ModulePanel::displayHeight() const
{
    if (type <= 0) return 0;
    switch (skin.display)
    {
        case 1: case 4: return 34;
        case 2: case 3: return 104;
        case 5:         return 96;
        default:        return 0;
    }
}

void ModulePanel::refresh()
{
    type = proc.getSlotType (slot);
    model = currentModel();
    const auto& inf = info (type);
    const int n = (int) inf.params.size();

    for (int j = 0; j < kMaxParams; ++j)
    {
        auto& c = cells[(size_t) j];
        const bool used = type > 0 && j < n;
        c.slider.setVisible (false);
        c.choice.setVisible (false);
        c.label.setVisible (false);
        if (! used) continue;

        const auto& pd = inf.params[(size_t) j];
        c.label.setText (String (pd.name).toUpperCase(), dontSendNotification);
        c.isChoice = (pd.kind == Kind::Choice);

        if (c.isChoice)
        {
            c.choice.setVisible (true);
            c.label.setVisible (j != 0);
        }
        else
        {
            c.slider.textFromValueFunction = [pd] (double norm) { return pd.format (pd.toReal ((float) norm)); };
            c.slider.valueFromTextFunction = [pd] (const String& t) { return (double) pd.toNorm (t.getFloatValue()); };
            c.slider.setDoubleClickReturnValue (true, (double) pd.toNorm (pd.def));
            c.slider.updateText();
            c.slider.setVisible (true);
            c.label.setVisible (true);
        }
    }
    applySkin();
}

void ModulePanel::applySkin()
{
    skin = skinFor (jmax (0, type), model);
    if (type <= 0) { repaint(); return; }
    const auto& inf = info (type);
    const int n = (int) inf.params.size();

    for (int j = 0; j < n; ++j)
    {
        auto& c = cells[(size_t) j];
        const auto& pd = inf.params[(size_t) j];
        c.label.setColour (Label::textColourId, skin.legend);
        if (pd.kind == Kind::Choice)
        {
            auto items = StringArray::fromTokens (pd.choices, "|", "");
            c.choice.setup (items, roundToInt (pd.toReal (proc.apvts.getParameter (RackProcessor::paramId (slot, j))->getValue())), skin);
        }
        else
        {
            c.slider.getProperties().set ("style", skin.knobStyle);
            c.slider.getProperties().set ("cap", var ((int64) capFor (type, model, j).getARGB()));
            c.slider.getProperties().set ("legend", var ((int64) skin.legend.getARGB()));
            c.slider.setColour (Slider::textBoxTextColourId, skin.text);
            c.slider.repaint();
        }
    }
    powerBtn.getProperties().set ("led", var ((int64) (skin.display == 0 ? Colour (0xff4cff7a) : Colour (0xff4cff7a)).getARGB()));
    resized();
    repaint();
}

void ModulePanel::resized()
{
    const int W = getWidth(), H = getHeight();
    leftBtn.setBounds (8, 6, 26, 22);
    removeBtn.setBounds (W / 2 - 13, 6, 26, 22);
    rightBtn.setBounds (W - 34, 6, 26, 22);
    powerBtn.setBounds (W / 2 - 18, H - 54, 36, 36);

    if (type <= 0) return;
    const auto& params = info (type).params;
    const int n = (int) params.size();
    const int dh = displayHeight();
    int top = 76 + (dh > 0 ? dh + 14 : 6);
    const int bottom = H - 62;

    int startIdx = 0;
    if (params[0].kind == Kind::Choice)
    {
        cells[0].choice.setBounds (10, top, W - 20, 30);
        top += 42; startIdx = 1;
    }
    const int count = n - startIdx;
    const int cols = n > 8 ? 3 : 2, rows = (count + cols - 1) / cols;
    const int availH = bottom - top, rowH = jmin (108, availH / jmax (1, rows));
    const int offset = jmax (0, (availH - rowH * rows) / 2), cw = W / cols;

    for (int j = startIdx; j < n; ++j)
    {
        const int k = j - startIdx, x = (k % cols) * cw, y = top + offset + (k / cols) * rowH;
        auto& c = cells[(size_t) j];
        c.label.setBounds (x, y, cw, 14);
        c.slider.setBounds (x + 2, y + 14, cw - 4, rowH - 14);
        c.slider.setTextBoxStyle (Slider::TextBoxBelow, false, cw - 4, 14);
        c.choice.setBounds (x + 6, y + 14, cw - 12, 28);
        const int hh = c.choice.wantedHeight();
        c.choice.setBounds (x + 6, y + 14 + (rowH - 14 - hh) / 2, cw - 12, hh);
    }
}

void ModulePanel::timerCallback()
{
    if (type <= 0) return;
    if (currentModel() != model) { model = currentModel(); applySkin(); }

    const float gr = proc.slotMeter[slot].load();
    const float lvl = Decibels::gainToDecibels (proc.slotLevel[slot].load(), -60.f);
    float targetVu = 0.f, targetLed = 0.f;
    if (skin.display == 2 || skin.display == 3) targetVu = jlimit (0.f, 1.f, gr / 20.f);
    if (skin.display == 1) targetLed = jlimit (0.f, 1.f, (lvl + 42.f) / 42.f);
    if (skin.display == 4) targetLed = jlimit (0.f, 1.f, gr / (type == Gate ? 40.f : (type == DeEsser ? 12.f : 20.f)));

    vuPos  += (targetVu - vuPos) * 0.28f;
    ledPos += (targetLed - ledPos) * 0.45f;
    if (skin.display != 0) repaint (displayRect().expanded (6, 6));
}

//------------------------------------------------------------------------------
void ModulePanel::paint (Graphics& g)
{
    const auto b = getLocalBounds().toFloat().reduced (2.f, 0.f);
    const int W = getWidth(), H = getHeight();

    g.setGradientFill (ColourGradient (skin.top, 0, 0, skin.bottom, 0, (float) H, false));
    g.fillRect (b);

    if (skin.brushed)
    {
        Random rnd (7);
        for (int y = 0; y < H; y += 2)
        {
            g.setColour ((rnd.nextBool() ? Colours::white : Colours::black).withAlpha (0.035f + rnd.nextFloat() * 0.03f));
            g.drawHorizontalLine (y, b.getX(), b.getRight());
        }
    }
    g.setColour (Colours::white.withAlpha (0.12f)); g.drawVerticalLine ((int) b.getX() + 1, 0.f, (float) H);
    g.setColour (Colours::black.withAlpha (0.6f));  g.drawRect (b, 1.5f);

    // vis
    for (float sx : { 14.f, (float) W - 14.f })
    {
        const float sy = (float) H - 16.f;
        g.setColour (Colours::black.withAlpha (0.4f)); g.fillEllipse (sx - 6, sy - 5, 12, 12);
        g.setGradientFill (ColourGradient (Colour (0xffd0d3d6), sx - 3, sy - 4, Colour (0xff6b6f73), sx + 4, sy + 5, false));
        g.fillEllipse (sx - 5.5f, sy - 5.5f, 11, 11);
        g.setColour (Colour (0xff25272a)); g.drawLine (sx - 3.5f, sy - 1.5f, sx + 3.5f, sy + 1.5f, 1.6f);
    }
    if (type <= 0) return;

    const auto& inf = info (type);

    // plaque de nom
    const bool darkText = skin.text.getPerceivedBrightness() < 0.5f;
    g.setColour (darkText ? Colours::white.withAlpha (0.32f) : Colours::black.withAlpha (0.25f));
    g.fillRoundedRectangle (10.f, 33.f, (float) W - 20.f, 38.f, 3.f);
    g.setColour (Colours::black.withAlpha (0.35f));
    g.drawRoundedRectangle (10.f, 33.f, (float) W - 20.f, 38.f, 3.f, 1.f);
    g.setColour (skin.text);
    g.setFont (Font (FontOptions (17.f, Font::bold)));
    g.drawText (inf.name, 10, 34, W - 20, 22, Justification::centred);
    String sub = inf.tag;
    if (! inf.params.empty() && inf.params[0].kind == Kind::Choice)
        sub = inf.params[0].format ((float) model).toUpperCase() + " STYLE";
    g.setColour (skin.legend.withAlpha (0.85f));
    g.setFont (Font (FontOptions (9.5f, Font::bold)));
    g.drawText (sub, 10, 55, W - 20, 14, Justification::centred);

    if (displayHeight() > 0) paintDisplay (g, displayRect().toFloat());

    g.setColour (skin.legend.withAlpha (0.8f));
    g.setFont (Font (FontOptions (10.f, Font::bold)));
    g.drawText ("IN", W / 2 + 22, H - 44, 24, 14, Justification::centredLeft);
    g.drawText (String (slot + 1), 0, H - 14, W, 12, Justification::centred);
}

void ModulePanel::drawLeds (Graphics& g, Rectangle<float> r, float pos, bool levelColours)
{
    g.setColour (Colour (0xff0b0c0e)); g.fillRoundedRectangle (r, 4.f);
    g.setColour (Colours::black); g.drawRoundedRectangle (r, 4.f, 1.2f);
    const int n = 12;
    auto in = r.reduced (7.f, 8.f);
    const float sw = in.getWidth() / (float) n;
    const int lit = (int) std::ceil (pos * (float) n - 0.001f);
    for (int i = 0; i < n; ++i)
    {
        Colour c = skin.accent;
        if (levelColours) c = i < 8 ? Colour (0xff3be36a) : (i < 11 ? Colour (0xffffc23e) : Colour (0xffff4338));
        auto seg = Rectangle<float> (in.getX() + (float) i * sw + 1.f, in.getY(), sw - 2.f, in.getHeight());
        const bool on = i < lit;
        g.setColour (on ? c : c.darker (0.8f).withAlpha (0.55f));
        g.fillRoundedRectangle (seg, 1.5f);
        if (on) { g.setColour (c.withAlpha (0.22f)); g.fillRoundedRectangle (seg.expanded (1.5f), 2.f); }
    }
}

void ModulePanel::drawVU (Graphics& g, Rectangle<float> r, float pos)
{
    auto bez = r;
    if (skin.hasStripe) { g.setColour (skin.stripe); g.fillRoundedRectangle (r, 6.f); bez = r.reduced (6.f); }
    g.setColour (Colour (0xff0a0a0a)); g.fillRoundedRectangle (bez, 4.f);
    auto face = bez.reduced (3.f);
    const bool cream = (skin.display == 2);
    g.setGradientFill (ColourGradient (Colour (cream ? 0xfff7edcb : 0xfff7f3e8), 0, face.getY(),
                                       Colour (cream ? 0xffd9ca96 : 0xffdcd7c6), 0, face.getBottom(), false));
    g.fillRoundedRectangle (face, 2.f);

    Graphics::ScopedSaveState ss (g);
    g.reduceClipRegion (face.toNearestInt());
    const float h = face.getHeight(), tm = 0.8f;
    const Point<float> pv (face.getCentreX(), face.getBottom() + h * 0.2f);
    const float Rr = h * 1.05f;
    const Colour ink (0xff231f1a), red (0xffc0392b);

    Path arc1, arc2;
    arc1.addCentredArc (pv.x, pv.y, Rr, Rr, 0.f, -tm, tm * 0.35f, true);
    arc2.addCentredArc (pv.x, pv.y, Rr, Rr, 0.f, tm * 0.35f, tm, true);
    g.setColour (ink); g.strokePath (arc1, PathStrokeType (1.3f));
    g.setColour (red); g.strokePath (arc2, PathStrokeType (3.f));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = -tm + 2.f * tm * (float) i / 10.f, len = (i % 5 == 0) ? 8.f : 4.5f;
        pointer (g, pv.x, pv.y, a, Rr - len, Rr, i >= 7 ? red : ink, 1.2f);
    }
    g.setColour (ink.withAlpha (0.8f));
    g.setFont (Font (FontOptions (9.f, Font::bold)));
    g.drawText ("GAIN REDUCTION", face.withTrimmedTop (h * 0.5f).withTrimmedBottom (h * 0.14f).toNearestInt(), Justification::centredBottom);
    g.setFont (Font (FontOptions (8.5f, Font::bold)));
    g.drawText ("0", Rectangle<int> ((int) (pv.x - Rr * std::sin (tm) - 6), (int) (face.getY() + 3), 14, 10), Justification::centred);
    g.drawText ("20", Rectangle<int> ((int) (pv.x + Rr * std::sin (tm) - 10), (int) (face.getY() + 3), 18, 10), Justification::centred);

    // aiguille
    const float a = -tm + 2.f * tm * pos;
    g.setColour (Colours::black.withAlpha (0.25f));
    g.drawLine (pv.x + 1.5f, pv.y + 1.5f, pv.x + std::sin (a) * (Rr - 3.f) + 1.5f, pv.y - std::cos (a) * (Rr - 3.f) + 1.5f, 1.8f);
    pointer (g, pv.x, pv.y, a, 0.f, Rr - 3.f, Colour (0xff111111), 1.7f);
    g.setColour (Colours::white.withAlpha (0.13f));
    g.fillRect (face.withHeight (h * 0.45f));
}

void ModulePanel::paintDisplay (Graphics& g, Rectangle<float> r)
{
    if (skin.display == 2 || skin.display == 3) { drawVU (g, r, vuPos); return; }
    if (skin.display == 1) { drawLeds (g, r, ledPos, true); return; }
    if (skin.display == 4)
    {
        drawLeds (g, r, ledPos, false);
        return;
    }
    if (skin.display != 5) return;

    // courbe de l'EQ précis
    const auto& inf = info (type);
    float v[kMaxParams];
    for (int j = 0; j < 12; ++j)
        v[j] = inf.params[(size_t) j].toReal (proc.apvts.getParameter (RackProcessor::paramId (slot, j))->getValue());
    const double sr = proc.getSampleRate() > 0 ? proc.getSampleRate() : 44100.0;

    g.setColour (Colour (0xff0a0c0e)); g.fillRoundedRectangle (r, 4.f);
    g.setColour (Colours::black);      g.drawRoundedRectangle (r, 4.f, 1.2f);
    auto area = r.reduced (5.f);
    g.setColour (Colours::white.withAlpha (0.08f));
    for (float f : { 100.f, 1000.f, 10000.f })
        g.drawVerticalLine ((int) (area.getX() + area.getWidth() * (std::log10 (f / 20.f) / 3.f)), area.getY(), area.getBottom());
    for (int d = -2; d <= 2; ++d)
        g.drawHorizontalLine ((int) (area.getCentreY() + (float) d * area.getHeight() * 0.22f), area.getX(), area.getRight());

    auto yFor = [&area] (float db) { return area.getCentreY() - jlimit (-12.f, 12.f, db) / 12.f * area.getHeight() * 0.5f; };
    Path path, fill;
    for (int i = 0; i <= 90; ++i)
    {
        const float f = 20.f * std::pow (1000.f, (float) i / 90.f);
        const float x = area.getX() + area.getWidth() * (float) i / 90.f, y = yFor (proEqMagDb (v, f, sr));
        if (i == 0) { path.startNewSubPath (x, y); fill.startNewSubPath (x, area.getCentreY()); fill.lineTo (x, y); }
        else        { path.lineTo (x, y); fill.lineTo (x, y); }
    }
    fill.lineTo (area.getRight(), area.getCentreY()); fill.closeSubPath();
    g.setColour (skin.accent.withAlpha (0.16f)); g.fillPath (fill);
    g.setColour (skin.accent); g.strokePath (path, PathStrokeType (1.8f));

    for (int b = 0; b < 4; ++b)    // poignées de bandes
    {
        const float f = v[b * 3], gain = v[b * 3 + 1];
        const float x = area.getX() + area.getWidth() * (std::log10 (f / 20.f) / 3.f);
        g.setColour (Colours::black); g.fillEllipse (x - 5.f, yFor (gain) - 5.f, 10.f, 10.f);
        g.setColour (skin.accent);    g.drawEllipse (x - 5.f, yFor (gain) - 5.f, 10.f, 10.f, 1.4f);
        g.setFont (Font (FontOptions (8.f, Font::bold)));
        g.drawText (String (b + 1), Rectangle<float> (x - 5.f, yFor (gain) - 5.f, 10.f, 10.f).toNearestInt(), Justification::centred);
    }
}

//==============================================================================
// TUILE « ADD MODULE »
//==============================================================================
void AddTile::paint (Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced (10.f, 4.f);
    g.setColour (Colours::white.withAlpha (hover ? 0.07f : 0.03f));
    g.fillRoundedRectangle (r, 8.f);
    Path p; p.addRoundedRectangle (r, 8.f);
    PathStrokeType st (2.f);
    float dashes[] = { 8.f, 6.f };
    Path dashed; st.createDashedStroke (dashed, p, dashes, 2);
    g.setColour (hover ? Colour (0xffffb23e) : Colours::white.withAlpha (0.35f));
    g.fillPath (dashed);

    const auto c = r.getCentre().translated (0.f, -30.f);
    g.setColour (hover ? Colour (0xffffb23e) : Colours::white.withAlpha (0.55f));
    g.drawEllipse (c.x - 30.f, c.y - 30.f, 60.f, 60.f, 3.f);
    g.drawLine (c.x - 14.f, c.y, c.x + 14.f, c.y, 4.f);
    g.drawLine (c.x, c.y - 14.f, c.x, c.y + 14.f, 4.f);
    g.setFont (Font (FontOptions (15.f, Font::bold)));
    g.drawText ("ADD MODULE", Rectangle<float> (r.getX(), c.y + 44.f, r.getWidth(), 22.f).toNearestInt(), Justification::centred);
    g.setFont (Font (FontOptions (11.f)));
    g.setColour (Colours::white.withAlpha (0.4f));
    g.drawText ("Preamp, EQ, compressor...", Rectangle<float> (r.getX(), c.y + 66.f, r.getWidth(), 16.f).toNearestInt(), Justification::centred);
}

void AddTile::mouseUp (const MouseEvent& e)
{
    if (! getLocalBounds().contains (e.getPosition())) return;
    PopupMenu m;
    auto add = [&m] (int t) { m.addItem (t + 100, String (info (t).name) + "   -   " + info (t).tag); };
    m.addSectionHeader ("PREAMPS & EQ");        add (Preamp); add (ConsoleEQ); add (ProEQ);
    m.addSectionHeader ("DYNAMICS");            add (Compressor); add (Gate); add (DeEsser); add (Limiter);
    m.addSectionHeader ("COLOUR");              add (Saturation);
    m.addSectionHeader ("FILTERS & STEREO");    add (Filters); add (Stereo);
    m.addSectionHeader ("TIME & SPACE");        add (Delay); add (rack::Reverb);
    m.showMenuAsync (PopupMenu::Options().withTargetComponent (this), [this] (int r)
    {
        if (r >= 100 && onPick) onPick (r - 100);
    });
}

//==============================================================================
// ÉDITEUR
//==============================================================================
RackEditor::RackEditor (RackProcessor& p) : AudioProcessorEditor (p), proc (p)
{
    setLookAndFeel (&lnf);
    addAndMakeVisible (content);

    for (int s = 0; s < kNumSlots; ++s)
    {
        panels[(size_t) s] = std::make_unique<ModulePanel> (proc, s);
        panels[(size_t) s]->onMove = [this] (int slot, int dir)
        {
            proc.moveModule (slot, dir);
            for (auto& pn : panels) pn->refresh();      // resynchronise toutes les faces
            content.repaint();
        };
        panels[(size_t) s]->onRemove = [this] (int slot)
        {
            proc.removeModule (slot);
            for (auto& pn : panels) pn->refresh();
            layoutForCount();
        };
        content.addChildComponent (*panels[(size_t) s]);
    }
    addTile.onPick = [this] (int type)
    {
        if (proc.addModule (type))
        {
            for (auto& pn : panels) pn->refresh();
            layoutForCount();
        }
    };
    content.addChildComponent (addTile);

    auto setupKnob = [this] (Slider& k, Label& l, const String& name, const String& id, std::unique_ptr<SliderParameterAttachment>& att)
    {
        k.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        k.setRotaryParameters (MathConstants<float>::pi * 1.2f, MathConstants<float>::pi * 2.8f, true);
        k.setTextBoxStyle (Slider::TextBoxRight, false, 62, 16);
        att = std::make_unique<SliderParameterAttachment> (*proc.apvts.getParameter (id), k);
        k.textFromValueFunction = [] (double v) { return String (v, 1) + " dB"; };
        k.updateText();
        k.getProperties().set ("style", 3);
        k.getProperties().set ("legend", var ((int64) 0xffd0d0d0));
        l.setText (name, dontSendNotification);
        l.setFont (Font (FontOptions (11.f, Font::bold)));
        content.addAndMakeVisible (k);
        content.addAndMakeVisible (l);
    };
    setupKnob (inKnob, inLabel, "INPUT", "in_gain", inAtt);
    setupKnob (outKnob, outLabel, "OUTPUT", "out_gain", outAtt);

    presetBox.setTextWhenNothingSelected ("Chains...");
    for (int i = 0; i < RackProcessor::numPresets(); ++i) presetBox.addItem (RackProcessor::presetName (i), i + 1);
    presetBox.onChange = [this]
    {
        const int id = presetBox.getSelectedId();
        if (id > 0)
        {
            proc.loadPreset (id - 1);
            presetBox.setSelectedId (0, dontSendNotification);
            for (auto& pn : panels) pn->refresh();
            layoutForCount();
        }
    };
    content.addAndMakeVisible (presetBox);

    bypassAtt = std::make_unique<ButtonParameterAttachment> (*proc.apvts.getParameter ("bypass"), bypassBtn);
    bypassBtn.getProperties().set ("led", var ((int64) 0xffff5a4a));
    bypassBtn.getProperties().set ("textLeft", true);
    content.addAndMakeVisible (bypassBtn);

    countAtt = std::make_unique<ParameterAttachment> (*proc.apvts.getParameter ("count"), [this] (float) { layoutForCount(); });

    setResizable (true, true);
    layoutForCount();
    startTimerHz (24);
}

RackEditor::~RackEditor() { stopTimer(); setLookAndFeel (nullptr); }

void RackEditor::layoutForCount()
{
    const float keepScale = scale;      // setResizeLimits peut redimensionner la fenêtre : on garde l'échelle voulue
    const int n = proc.getNumModules();
    const bool full = n >= kNumSlots;
    for (int s = 0; s < kNumSlots; ++s)
    {
        panels[(size_t) s]->setVisible (s < n);
        panels[(size_t) s]->setBounds (kRail + s * ModulePanel::kWidth, kTop, ModulePanel::kWidth, ModulePanel::kHeight);
    }
    addTile.setVisible (! full);
    addTile.setBounds (kRail + n * ModulePanel::kWidth, kTop, ModulePanel::kWidth, ModulePanel::kHeight);

    contentW = jmax (980, 2 * kRail + ModulePanel::kWidth * (n + (full ? 0 : 1)));
    content.setBounds (0, 0, contentW, kH);

    presetBox.setBounds (190, 14, 130, 26);
    inLabel.setBounds (340, 8, 44, 38);   inKnob.setBounds (384, 8, 104, 38);
    outLabel.setBounds (500, 8, 52, 38);  outKnob.setBounds (552, 8, 104, 38);
    bypassBtn.setBounds (contentW - 150, 12, 120, 30);

    getConstrainer()->setFixedAspectRatio ((double) contentW / (double) kH);
    setResizeLimits ((int) (contentW * 0.5), (int) (kH * 0.5), (int) (contentW * 1.4), (int) (kH * 1.4));
    scale = keepScale;
    setSize (roundToInt ((float) contentW * keepScale), roundToInt ((float) kH * keepScale));
    shownCount = n;
    content.repaint();
}

void RackEditor::resized()
{
    if (getWidth() <= 0 || contentW <= 0) return;
    scale = (float) getWidth() / (float) contentW;
    content.setTransform (AffineTransform::scale (scale));
    content.setBounds (0, 0, contentW, kH);
}

void RackEditor::paint (Graphics& g) { g.fillAll (Colour (0xff101113)); }

void RackEditor::timerCallback()
{
    dispIn  = jmax (proc.inPeak.load(),  dispIn  * 0.88f);
    dispOut = jmax (proc.outPeak.load(), dispOut * 0.88f);
    content.repaint (0, 0, contentW, kTop);
}

void RackEditor::paintContent (Graphics& g)
{
    g.fillAll (Colour (0xff0d0e10));

    // barre du haut
    g.setGradientFill (ColourGradient (Colour (0xff2a2d31), 0, 0, Colour (0xff17191c), 0, (float) kTop, false));
    g.fillRect (0, 0, contentW, kTop);
    g.setColour (Colours::black); g.drawHorizontalLine (kTop - 1, 0.f, (float) contentW);
    g.setColour (Colours::white);
    g.setFont (Font (FontOptions (21.f, Font::bold)));
    g.drawText ("RACK STRIP", 16, 6, 170, 28, Justification::centredLeft);
    g.setColour (Colour (0xffffb23e));
    g.setFont (Font (FontOptions (9.f, Font::bold)));
    g.drawText ("MODULAR CHANNEL CHAIN", 16, 32, 170, 14, Justification::centredLeft);

    auto meter = [&g] (float v, int x, int y)
    {
        const float db = Decibels::gainToDecibels (v, -60.f);
        const float frac = jlimit (0.f, 1.f, (db + 60.f) / 60.f);
        g.setColour (Colour (0xff0b0d0f)); g.fillRect (x, y, 100, 7);
        g.setColour (db > -1.f ? Colour (0xffff4040) : (db > -12.f ? Colour (0xffffb23e) : Colour (0xff4cff7a)));
        g.fillRect (x, y, (int) (100 * frac), 7);
    };
    const int mx = contentW - 290;
    meter (dispIn, mx, 15);
    meter (dispOut, mx, 31);
    g.setColour (Colours::white.withAlpha (0.6f));
    g.setFont (Font (FontOptions (9.f, Font::bold)));
    g.drawText ("IN", mx - 30, 12, 26, 10, Justification::centredRight);
    g.drawText ("OUT", mx - 34, 28, 30, 10, Justification::centredRight);

    // rails du rack, sur toute la hauteur et toute la largeur
    for (int side = 0; side < 2; ++side)
    {
        const int x = side == 0 ? 0 : contentW - kRail;
        g.setGradientFill (ColourGradient (Colour (0xff3d4044), (float) x, 0, Colour (0xff222427), (float) (x + kRail), 0, false));
        g.fillRect (x, kTop, kRail, kH - kTop);
        g.setColour (Colour (0xff060607));
        for (int y = kTop + 22; y < kH - 14; y += 60)
            g.fillRoundedRectangle ((float) x + 9.f, (float) y, 12.f, 20.f, 4.f);
    }

    if (proc.getNumModules() == 0)
    {
        g.setColour (Colours::white.withAlpha (0.28f));
        g.setFont (Font (FontOptions (18.f)));
        g.drawText ("Your rack is empty. Click  +  to build your chain, or pick a ready-made chain above.",
                    kRail + ModulePanel::kWidth + 10, kTop + 250, contentW - 2 * kRail - ModulePanel::kWidth - 20, 60,
                    Justification::centredLeft);
    }
}
