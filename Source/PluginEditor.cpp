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

    auto neve = [&] { s.top = C (0xff94a8b9); s.bottom = C (0xff6a7f94); s.text = Colours::white; s.legend = Colours::white; s.knobStyle = 0; s.accent = C (0xffff5a4a); };
    auto api  = [&] { s.top = C (0xff232528); s.bottom = C (0xff0f1012); s.text = Colours::white; s.legend = C (0xff7db8ff); s.knobStyle = 2; s.accent = C (0xff5fa8ff); };
    auto ssl  = [&] { s.top = C (0xffa4a7aa); s.bottom = C (0xff82868a); s.text = C (0xff15181a); s.legend = C (0xff15181a); s.knobStyle = 1; s.accent = C (0xffe2362d); };

    switch (type)
    {
        case Preamp:
            s.display = 6; s.strip = 1; s.accent = C (0xff6fe8a8);
            if (model == 1) neve(); else if (model == 2) api(); else ssl();
            break;
        case ConsoleEQ:
            s.display = 5; s.accent = C (0xff6fe8a8);
            if (model == 1) neve(); else if (model == 2) api(); else ssl();
            break;
        case Compressor:
            if (model == 1)        // OPTO : aluminium brossé, bakélite, VU crème
            {
                s.top = C (0xffd6d9db); s.bottom = C (0xffa6aaad); s.text = C (0xff1a1c1e); s.legend = C (0xff1a1c1e);
                s.knobStyle = 3; s.display = 2; s.brushed = true; s.accent = C (0xffc0392b);
            }
            else if (model == 2)   // FET : face noire, dessus argent, VU avec liseré bleu
            {
                s.top = C (0xff1d1e20); s.bottom = C (0xff0c0d0e); s.text = C (0xffeeeeee); s.legend = C (0xffdcdcdc);
                s.knobStyle = 4; s.display = 3; s.hasStripe = true; s.stripe = C (0xff2c5f9e); s.accent = C (0xffff9a3c);
            }
            else                   // VCA : gris console, capuchons colorés, LED
            {
                s.top = C (0xffadada5); s.bottom = C (0xff8d8d86); s.text = C (0xff17191a); s.legend = C (0xff17191a);
                s.knobStyle = 1; s.display = 4; s.accent = C (0xffe2362d);
            }
            break;
        case BusComp:
            if (model == 1) { neve(); s.display = 2; }
            else if (model == 2) { api(); s.display = 3; }
            else { ssl(); s.display = 3; s.top = C (0xffb3b5b2); s.bottom = C (0xff93958f); }
            break;
        case Saturation:
            s.accent = C (0xffffb23e); s.display = 6;
            if (model == 1)        { s.top = C (0xff66442c); s.bottom = C (0xff3b2517); s.text = C (0xfff0e2c4); s.legend = C (0xfff0e2c4); s.knobStyle = 5; }
            else if (model == 2)   { s.top = C (0xff2c3d5a); s.bottom = C (0xff182134); s.knobStyle = 0; }
            else if (model == 3)   { s.top = C (0xff2f2f32); s.bottom = C (0xff18181a); s.legend = C (0xffff9a4a); s.accent = C (0xffff9a4a); s.knobStyle = 4; }
            else                   { s.top = C (0xff44524c); s.bottom = C (0xff29332f); s.text = C (0xffe9e4d0); s.legend = C (0xffe9e4d0); s.knobStyle = 4; }
            break;
        case ProEQ:
            s.top = C (0xff363a3f); s.bottom = C (0xff1b1d20); s.knobStyle = 2; s.display = 5; s.accent = C (0xff4ad6ff); s.legend = C (0xff9fdcff);
            break;
        case Gate:     s.display = 6; s.strip = 2; s.accent = C (0xff7be36f); break;
        case DeEsser:  s.display = 6; s.strip = 2; s.accent = C (0xffd28bff); break;
        case Limiter:  s.display = 6; s.strip = 2; s.accent = C (0xffff8a6a); break;
        case Filters:  s.top = C (0xff585c60); s.bottom = C (0xff34373a); s.knobStyle = 1; s.display = 5; s.accent = C (0xffffd24a); break;
        case Stereo:   s.display = 7; s.accent = C (0xff58ff9c); break;
        case Delay:    s.display = 8; s.accent = C (0xff6ff0ff); break;
        case rack::Reverb: s.display = 6; s.accent = C (0xffb9a8ff); break;
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
        case BusComp:
            if (model == 1) return j == 5 ? neveRed : (j == 2 ? navy : grey);
            if (model == 2) return j == 1 ? blue : (j == 5 ? black : white);
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
// LOOK & FEEL : boutons réalistes
//==============================================================================
static Colour readCol (const Component& c, const char* key, uint32 def)
{
    const var v = c.getProperties().getWithDefault (key, var ((int64) def));
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
    setColour (Slider::textBoxOutlineColourId, Colour (0xff3a3d41));
    setColour (Slider::textBoxBackgroundColourId, Colour (0xff0a0b0c).withAlpha (0.92f));
    setColour (Slider::textBoxHighlightColourId, Colour (0xff3a6ea5));
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
    g.setGradientFill (ColourGradient (Colour (down ? 0xff202226 : 0xff5a5e64), 0, r.getY(), Colour (0xff1c1d20), 0, r.getBottom(), false));
    g.fillRoundedRectangle (r, 3.f);
    g.setColour (Colours::white.withAlpha (0.18f)); g.drawHorizontalLine ((int) r.getY() + 1, r.getX() + 3, r.getRight() - 3);
    g.setColour (hl ? Colour (0xffffb23e) : Colour (0xff050506));
    g.drawRoundedRectangle (r, 3.f, 1.f);
}

void RackLnF::drawScrollbar (Graphics& g, ScrollBar&, int x, int y, int w, int h, bool vertical, int thumbStart, int thumbSize, bool over, bool down)
{
    auto track = Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (2.f, 3.f);
    g.setColour (Colour (0xff060707)); g.fillRoundedRectangle (track, 4.f);
    g.setColour (Colours::white.withAlpha (0.08f)); g.drawRoundedRectangle (track, 4.f, 1.f);
    auto th = vertical ? Rectangle<float> ((float) x + 3.f, (float) (y + thumbStart), (float) w - 6.f, (float) thumbSize)
                       : Rectangle<float> ((float) (x + thumbStart), (float) y + 3.f, (float) thumbSize, (float) h - 6.f);
    g.setGradientFill (ColourGradient (Colour (over || down ? 0xffffc760 : 0xffe0a03a), 0, th.getY(), Colour (0xff8a5a14), 0, th.getBottom(), false));
    g.fillRoundedRectangle (th, 4.f);
    g.setColour (Colours::white.withAlpha (0.35f)); g.drawHorizontalLine ((int) th.getY() + 1, th.getX() + 4, th.getRight() - 4);
}

static void pointer (Graphics& g, float cx, float cy, float ang, float r0, float r1, Colour c, float thick)
{
    g.setColour (c);
    g.drawLine (cx + std::sin (ang) * r0, cy - std::cos (ang) * r0, cx + std::sin (ang) * r1, cy - std::cos (ang) * r1, thick);
}

static void knurl (Graphics& g, float cx, float cy, float r0, float r1, int n, Colour c, float thick = 1.f)
{
    for (int i = 0; i < n; ++i)
        pointer (g, cx, cy, MathConstants<float>::twoPi * (float) i / (float) n, r0, r1, c, thick);
}

static void radialDisc (Graphics& g, float cx, float cy, float r, Colour hi, Colour lo)
{
    g.setGradientFill (ColourGradient (hi, cx - r * 0.3f, cy - r * 0.42f, lo, cx + r * 0.7f, cy + r * 0.45f, true));
    g.fillEllipse (cx - r, cy - r, r * 2, r * 2);
}

// Disque métallique tourné : reflets anisotropes en éventail, comme un vrai bouton en aluminium usiné
static void anisoDisc (Graphics& g, float cx, float cy, float r, Colour lo, Colour hi)
{
    Graphics::ScopedSaveState ss (g);
    Path circ; circ.addEllipse (cx - r, cy - r, r * 2, r * 2);
    g.reduceClipRegion (circ, {});
    const int N = 72;
    for (int k = 0; k < N; ++k)
    {
        const float a = MathConstants<float>::twoPi * (float) k / (float) N, a2 = MathConstants<float>::twoPi * ((float) k + 1.5f) / (float) N;
        const float t = jlimit (0.f, 1.f, 0.5f + 0.34f * std::cos (2.f * (a - 0.7f)) + 0.16f * std::cos (4.f * (a + 0.25f)));
        Path tri;
        tri.startNewSubPath (cx, cy);
        tri.lineTo (cx + std::sin (a) * r * 1.06f, cy - std::cos (a) * r * 1.06f);
        tri.lineTo (cx + std::sin (a2) * r * 1.06f, cy - std::cos (a2) * r * 1.06f);
        tri.closeSubPath();
        g.setColour (lo.interpolatedWith (hi, t));
        g.fillPath (tri);
    }
}

static void gloss (Graphics& g, float cx, float cy, float r, float alpha)
{
    g.setGradientFill (ColourGradient (Colours::white.withAlpha (alpha), cx, cy - r * 0.95f, Colours::white.withAlpha (0.f), cx, cy - r * 0.1f, false));
    g.fillEllipse (cx - r * 0.72f, cy - r * 0.96f, r * 1.44f, r * 0.9f);
}

void RackLnF::drawRotarySlider (Graphics& g, int x, int y, int w, int h, float pos, float a0, float a1, Slider& s)
{
    const int style = (int) s.getProperties().getWithDefault ("style", 5);
    const Colour cap = readCol (s, "cap", 0xffe3d9bb), legend = readCol (s, "legend", 0xffffffff);
    const float R = jmin (w, h) * 0.5f - 1.f;
    const float cx = x + w * 0.5f, cy = y + h * 0.5f;
    const float ang = a0 + pos * (a1 - a0);
    const float kr = R - 9.f;

    // graduations sérigraphiées
    for (int i = 0; i <= 10; ++i)
    {
        const float a = a0 + (a1 - a0) * (float) i / 10.f;
        const float len = (i == 0 || i == 5 || i == 10) ? 6.5f : 3.8f;
        pointer (g, cx, cy, a, R - len, R, legend.withAlpha (0.9f), (i % 5 == 0) ? 1.6f : 1.1f);
    }
    // valeurs mini / maxi imprimées
    const String mn = s.getProperties().getWithDefault ("minTxt", "").toString(), mx = s.getProperties().getWithDefault ("maxTxt", "").toString();
    g.setColour (legend.withAlpha (0.92f));
    g.setFont (Font (FontOptions (8.5f, Font::bold)));
    if (mn.isNotEmpty()) g.drawText (mn, Rectangle<float> (cx - R * 0.72f - 11.f, cy + R * 0.74f, 22.f, 10.f).toNearestInt(), Justification::centred);
    if (mx.isNotEmpty()) g.drawText (mx, Rectangle<float> (cx + R * 0.72f - 11.f, cy + R * 0.74f, 22.f, 10.f).toNearestInt(), Justification::centred);

    // ombre portée douce
    for (int i = 3; i >= 1; --i)
    {
        g.setColour (Colours::black.withAlpha (0.13f));
        g.fillEllipse (cx - kr - (float) i + 1.5f, cy - kr - (float) i + 4.f, (kr + (float) i) * 2, (kr + (float) i) * 2);
    }
    auto rim = [&g, cx, cy] (float r, Colour c, float t = 1.f) { g.setColour (c); g.drawEllipse (cx - r, cy - r, r * 2, r * 2, t); };

    switch (style)
    {
        case 0: // Marconi à jupe, cannelé
        {
            radialDisc (g, cx, cy, kr, cap.darker (0.35f), cap.darker (0.75f));
            rim (kr, Colours::black.withAlpha (0.7f), 1.2f);
            const float cr = kr * 0.8f;
            radialDisc (g, cx, cy, cr, cap.brighter (0.45f), cap.darker (0.3f));
            knurl (g, cx, cy, cr * 0.84f, cr, 36, Colours::black.withAlpha (0.4f), 1.6f);
            knurl (g, cx, cy, cr * 0.84f, cr, 36, Colours::white.withAlpha (0.12f), 0.6f);
            rim (cr, Colours::black.withAlpha (0.55f));
            radialDisc (g, cx, cy, cr * 0.68f, cap.brighter (0.35f), cap.darker (0.12f));
            pointer (g, cx, cy, ang, kr * 0.05f, kr * 0.76f, Colours::black.withAlpha (0.35f), 3.4f);
            pointer (g, cx, cy, ang, kr * 0.05f, kr * 0.76f, Colours::white, 2.4f);
            gloss (g, cx, cy, cr, 0.28f);
            break;
        }
        case 1: // capuchon SSL
        {
            radialDisc (g, cx, cy, kr, Colour (0xff3a3c40), Colour (0xff0a0a0b));
            rim (kr, Colours::black, 1.2f);
            const float cr = kr * 0.9f;
            radialDisc (g, cx, cy, cr, cap.brighter (0.4f), cap.darker (0.5f));
            knurl (g, cx, cy, cr * 0.86f, cr, 44, Colours::black.withAlpha (0.28f), 1.f);
            rim (cr, Colours::black.withAlpha (0.7f));
            pointer (g, cx, cy, ang, kr * 0.18f, kr * 0.86f, Colours::black.withAlpha (0.3f), 3.8f);
            pointer (g, cx, cy, ang, kr * 0.18f, kr * 0.86f, cap.getPerceivedBrightness() > 0.55f ? Colours::black : Colours::white, 2.6f);
            gloss (g, cx, cy, cr, 0.2f);
            break;
        }
        case 2: // aluminium usiné + insert coloré
        {
            anisoDisc (g, cx, cy, kr, Colour (0xff6d7277), Colour (0xfff7f9fa));
            knurl (g, cx, cy, kr * 0.9f, kr, 60, Colours::black.withAlpha (0.3f), 1.f);
            rim (kr, Colours::black.withAlpha (0.8f), 1.3f);
            rim (kr - 1.5f, Colours::white.withAlpha (0.35f), 0.8f);
            const float ir = kr * 0.5f;
            radialDisc (g, cx, cy, ir, cap.brighter (0.45f), cap.darker (0.55f));
            rim (ir, Colours::black.withAlpha (0.75f), 1.2f);
            gloss (g, cx, cy, ir, 0.3f);
            pointer (g, cx, cy, ang, kr * 0.6f, kr * 0.98f, Colours::black, 3.2f);
            break;
        }
        case 3: // bakélite noire laquée
        {
            anisoDisc (g, cx, cy, kr, Colour (0xff7c8084), Colour (0xffe9ecee));
            radialDisc (g, cx, cy, kr * 0.93f, Colour (0xff55565a), Colour (0xff050506));
            rim (kr * 0.93f, Colours::black, 1.2f);
            radialDisc (g, cx, cy, kr * 0.8f, Colour (0xff3a3b3e), Colour (0xff09090a));
            pointer (g, cx, cy, ang, kr * 0.12f, kr * 0.74f, Colours::black.withAlpha (0.4f), 4.f);
            pointer (g, cx, cy, ang, kr * 0.12f, kr * 0.74f, Colours::white, 3.f);
            gloss (g, cx, cy, kr * 0.9f, 0.3f);
            break;
        }
        case 4: // FET : noir moleté, dessus argent
        {
            radialDisc (g, cx, cy, kr, Colour (0xff3b3c40), Colour (0xff060607));
            knurl (g, cx, cy, kr * 0.88f, kr, 56, Colours::white.withAlpha (0.14f), 1.f);
            rim (kr, Colours::black, 1.3f);
            const float ir = kr * 0.66f;
            anisoDisc (g, cx, cy, ir, Colour (0xff80858a), Colour (0xfffafbfc));
            rim (ir, Colours::black.withAlpha (0.65f), 1.2f);
            pointer (g, cx, cy, ang, kr * 0.06f, ir * 0.96f, Colours::black, 2.8f);
            pointer (g, cx, cy, ang, kr * 0.74f, kr * 0.97f, Colours::white, 2.2f);
            break;
        }
        default: // ivoire vintage
        {
            radialDisc (g, cx, cy, kr, cap.brighter (0.2f), cap.darker (0.55f));
            rim (kr, Colours::black.withAlpha (0.65f), 1.2f);
            radialDisc (g, cx, cy, kr * 0.84f, cap.brighter (0.4f), cap.darker (0.2f));
            rim (kr * 0.84f, Colours::black.withAlpha (0.25f));
            pointer (g, cx, cy, ang, kr * 0.12f, kr * 0.8f, Colour (0xff3a2a18), 3.f);
            gloss (g, cx, cy, kr * 0.84f, 0.25f);
            break;
        }
    }
}

void RackLnF::drawToggleButton (Graphics& g, ToggleButton& b, bool, bool)
{
    auto r = b.getLocalBounds().toFloat();
    const bool on = b.getToggleState();
    const Colour led = readCol (b, "led", 0xff4cff7a);

    if (b.getProperties().contains ("push"))    // bouton-poussoir lumineux "IN"
    {
        auto f = r.reduced (3.f);
        g.setColour (Colours::black.withAlpha (0.55f)); g.fillRoundedRectangle (f.translated (0.f, 2.f).expanded (1.f), 4.f);
        g.setGradientFill (ColourGradient (Colour (0xff6d7177), 0, f.getY(), Colour (0xff1c1d20), 0, f.getBottom(), false));
        g.fillRoundedRectangle (f, 4.f);
        auto lens = f.reduced (4.f);
        g.setGradientFill (ColourGradient (on ? led.brighter (0.5f) : led.darker (0.9f), 0, lens.getY(),
                                           on ? led.darker (0.25f) : led.darker (1.2f), 0, lens.getBottom(), false));
        g.fillRoundedRectangle (lens, 3.f);
        if (on) { g.setColour (led.withAlpha (0.28f)); g.fillRoundedRectangle (lens.expanded (4.f), 6.f); }
        g.setColour (Colours::white.withAlpha (on ? 0.4f : 0.12f)); g.fillRoundedRectangle (lens.withHeight (lens.getHeight() * 0.4f).reduced (2.f, 0.f), 2.f);
        g.setColour (Colours::black.withAlpha (0.8f)); g.drawRoundedRectangle (lens, 3.f, 1.f);
        g.setColour (on ? Colour (0xff102014) : Colour (0xffcfd3d6));
        g.setFont (Font (FontOptions (11.f, Font::bold)));
        g.drawText ("IN", lens.toNearestInt(), Justification::centred);
        return;
    }

    const float d = jmin (r.getWidth(), r.getHeight()) - 8.f;
    auto c = r.getCentre();
    if (b.getButtonText().isNotEmpty() && b.getProperties().contains ("textLeft")) c = { r.getX() + d * 0.5f + 4.f, r.getCentreY() };
    g.setColour (Colours::black.withAlpha (0.65f));
    g.fillEllipse (c.x - d / 2 - 2.5f, c.y - d / 2 - 2.5f, d + 5, d + 5);
    radialDisc (g, c.x, c.y, d / 2, on ? led.brighter (0.6f) : led.darker (0.5f), on ? led.darker (0.3f) : led.darker (1.3f));
    gloss (g, c.x, c.y, d / 2, on ? 0.5f : 0.2f);
    if (on) { g.setColour (led.withAlpha (0.22f)); g.fillEllipse (c.x - d, c.y - d, d * 2, d * 2); }
    if (b.getButtonText().isNotEmpty())
    {
        g.setColour (Colours::white);
        g.setFont (Font (FontOptions (11.f, Font::bold)));
        g.drawText (b.getButtonText(), r.withTrimmedLeft (d + 12).toNearestInt(), Justification::centredLeft);
    }
}

//==============================================================================
// ANALYSEUR DE SPECTRE
//==============================================================================
SpectrumAnalyzer::SpectrumAnalyzer() { bands.fill (0.f); }

void SpectrumAnalyzer::finish (double sr)
{
    const float N = 2048.f;
    for (int b = 0; b < kBands; ++b)
    {
        const float f0 = 20.f * std::pow (1000.f, (float) b / (float) kBands), f1 = 20.f * std::pow (1000.f, (float) (b + 1) / (float) kBands);
        int k0 = jmax (1, (int) std::floor (f0 * N / (float) sr)), k1 = (int) std::ceil (f1 * N / (float) sr);
        k1 = jmin (1023, jmax (k0 + 1, k1));
        k0 = jmin (k0, 1022);
        float mag = 0.f;
        for (int k = k0; k < k1; ++k) mag = jmax (mag, work[(size_t) k]);
        const float db = 20.f * std::log10 (mag / 512.f + 1.0e-9f);
        const float fc = std::sqrt (f0 * f1);
        const float tilt = 4.5f * std::log2 (fc / 1000.f);          // compense la pente naturelle du son
        const float norm = jlimit (0.f, 1.f, (db + tilt + 78.f) / 62.f);
        bands[(size_t) b] = jmax (norm, bands[(size_t) b] * 0.82f);
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
        g.setColour (Colours::black.withAlpha (0.5f)); g.fillRoundedRectangle (r.translated (0.f, 1.5f), 3.f);
        if (on)
        {
            g.setGradientFill (ColourGradient (skin.accent.brighter (0.5f), 0, r.getY(), skin.accent.darker (0.35f), 0, r.getBottom(), false));
            g.fillRoundedRectangle (r, 3.f);
            g.setColour (skin.accent.withAlpha (0.3f)); g.drawRoundedRectangle (r.expanded (1.5f), 4.f, 2.f);
        }
        else
        {
            g.setGradientFill (ColourGradient (Colour (0xff5a5d62), 0, r.getY(), Colour (0xff17181a), 0, r.getBottom(), false));
            g.fillRoundedRectangle (r, 3.f);
        }
        g.setColour (Colours::white.withAlpha (on ? 0.45f : 0.2f)); g.drawHorizontalLine ((int) r.getY() + 1, r.getX() + 3.f, r.getRight() - 3.f);
        g.setColour (Colours::black.withAlpha (0.85f)); g.drawRoundedRectangle (r, 3.f, 1.f);
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
void drawScrew (Graphics& g, float x, float y, float seed)
{
    g.setColour (Colours::black.withAlpha (0.45f)); g.fillEllipse (x - 6.f, y - 4.5f, 12.f, 12.f);
    g.setGradientFill (ColourGradient (Colour (0xfff0f2f4), x - 3.f, y - 4.f, Colour (0xff5d6165), x + 4.f, y + 5.f, true));
    g.fillEllipse (x - 5.5f, y - 5.5f, 11.f, 11.f);
    g.setColour (Colours::black.withAlpha (0.55f)); g.drawEllipse (x - 5.5f, y - 5.5f, 11.f, 11.f, 0.9f);
    const float a = seed * 1.7f;
    g.setColour (Colour (0xff2a2c2f));
    g.drawLine (x - std::cos (a) * 3.8f, y - std::sin (a) * 3.8f, x + std::cos (a) * 3.8f, y + std::sin (a) * 3.8f, 1.6f);
    g.drawLine (x - std::sin (a) * 3.8f, y + std::cos (a) * 3.8f, x + std::sin (a) * 3.8f, y - std::cos (a) * 3.8f, 1.6f);
}

static String shortNum (const ParamDef& pd, float v)
{
    String t;
    if (pd.unit != nullptr && String (pd.unit) == "Hz")
        t = v >= 1000.f ? String (v / 1000.f, v >= 10000.f ? 0 : 1) + "k" : String ((int) std::round (v));
    else if (std::abs (v) < 10.f && std::abs (v - std::round (v)) > 0.01f) t = String (v, 1);
    else t = String ((int) std::round (v));
    if (pd.mn < 0.f && v > 0.f) t = "+" + t;
    return t;
}

ModulePanel::ModulePanel (RackProcessor& p, int s, int width) : proc (p), slot (s), isMaster (s == kMasterSlot)
{
    setSize (width, kHeight);

    powerBtn.getProperties().set ("push", true);
    addAndMakeVisible (powerBtn);
    onAtt = std::make_unique<ButtonParameterAttachment> (*proc.apvts.getParameter (RackProcessor::onId (slot)), powerBtn);

    for (auto* b : { &leftBtn, &rightBtn, &removeBtn }) addChildComponent (*b);
    if (! isMaster) { leftBtn.setVisible (true); rightBtn.setVisible (true); removeBtn.setVisible (true); }
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
        c.slider.setTextBoxStyle (Slider::TextBoxBelow, false, 70, 14);
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

std::array<float, kMaxParams> ModulePanel::realValues() const
{
    std::array<float, kMaxParams> v {};
    if (type <= 0) return v;
    const auto& inf = info (type);
    for (size_t j = 0; j < inf.params.size(); ++j)
        v[j] = inf.params[j].toReal (proc.apvts.getParameter (RackProcessor::paramId (slot, (int) j))->getValue());
    return v;
}

int ModulePanel::mainDisplayHeight() const
{
    if (type <= 0) return 0;
    switch (skin.display)
    {
        case 2: case 3: return 108;
        case 4:         return 36;
        case 5: case 6: case 7: case 8: return 96;
        default:        return 0;
    }
}

int ModulePanel::displayHeight() const
{
    const int m = mainDisplayHeight();
    return m == 0 ? 0 : m + (skin.strip > 0 ? 30 : 0);
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
    specIn.clear(); specOut.clear();
    applySkin();
}

void ModulePanel::applySkin()
{
    skin = skinFor (jmax (0, type), model);
    if (type > 0)
    {
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
                c.slider.getProperties().set ("minTxt", shortNum (pd, pd.mn));
                c.slider.getProperties().set ("maxTxt", shortNum (pd, pd.mx));
                c.slider.setColour (Slider::textBoxTextColourId, Colour (0xffe6efe4));
                c.slider.repaint();
            }
        }
    }
    powerBtn.getProperties().set ("led", var ((int64) (skin.display == 0 ? 0xff4cff7a : 0xff4cff7a)));
    resized();
    buildBackground();
    repaint();
}

void ModulePanel::buildBackground()
{
    const int W = getWidth(), H = getHeight();
    if (W <= 0 || H <= 0) return;
    bg = Image (Image::ARGB, W * 2, H * 2, true);
    Graphics g (bg);
    g.addTransform (AffineTransform::scale (2.f));

    g.setGradientFill (ColourGradient (skin.top, 0, 0, skin.bottom, 0, (float) H, false));
    g.fillAll();
    Random rnd (slot * 977 + type * 31 + model * 7 + 5);

    if (skin.brushed)
    {
        for (int k = 0; k < 1600; ++k)
        {
            const float y = rnd.nextFloat() * (float) H, x = rnd.nextFloat() * (float) W, len = 12.f + rnd.nextFloat() * 110.f;
            g.setColour ((rnd.nextBool() ? Colours::white : Colours::black).withAlpha (0.03f + rnd.nextFloat() * 0.06f));
            g.drawLine (x, y, x + len, y, 0.55f);
        }
        ColourGradient sheen (Colours::black.withAlpha (0.10f), 0, 0, Colours::black.withAlpha (0.10f), (float) W, 0, false);
        sheen.addColour (0.28, Colours::white.withAlpha (0.20f));
        sheen.addColour (0.55, Colours::white.withAlpha (0.02f));
        g.setGradientFill (sheen); g.fillAll();
    }
    else
    {
        for (int k = 0; k < 4200; ++k)     // grain de peinture / sérigraphie
        {
            g.setColour ((rnd.nextBool() ? Colours::white : Colours::black).withAlpha (0.02f + rnd.nextFloat() * 0.035f));
            g.fillRect (rnd.nextFloat() * (float) W, rnd.nextFloat() * (float) H, 1.f, 1.f);
        }
        ColourGradient sheen (Colours::black.withAlpha (0.12f), 0, 0, Colours::black.withAlpha (0.12f), (float) W, 0, false);
        sheen.addColour (0.3, Colours::white.withAlpha (0.07f));
        g.setGradientFill (sheen); g.fillAll();
    }

    // biseau de la face avant
    g.setColour (Colours::white.withAlpha (0.38f)); g.drawHorizontalLine (1, 0.f, (float) W);
    g.setColour (Colours::white.withAlpha (0.2f));  g.drawVerticalLine (1, 0.f, (float) H);
    g.setColour (Colours::black.withAlpha (0.55f)); g.drawHorizontalLine (H - 2, 0.f, (float) W);
    g.drawVerticalLine (W - 2, 0.f, (float) H);
    // ombres portées entre les modules
    g.setGradientFill (ColourGradient (Colours::black.withAlpha (0.55f), 0, 0, Colours::transparentBlack, 7.f, 0, false));
    g.fillRect (0.f, 0.f, 7.f, (float) H);
    g.setGradientFill (ColourGradient (Colours::black.withAlpha (0.55f), (float) W, 0, Colours::transparentBlack, (float) W - 7.f, 0, false));
    g.fillRect ((float) W - 7.f, 0.f, 7.f, (float) H);

    for (int i = 0; i < 4; ++i)
        drawScrew (g, (i % 2 == 0) ? 14.f : (float) W - 14.f, (i < 2) ? 12.f : (float) H - 12.f, (float) (i + slot));
}

void ModulePanel::resized()
{
    const int W = getWidth(), H = getHeight();
    leftBtn.setBounds (26, 20, 26, 20);
    removeBtn.setBounds (W / 2 - 13, 20, 26, 20);
    rightBtn.setBounds (W - 52, 20, 26, 20);
    powerBtn.setBounds (W / 2 - 28, H - 62, 56, 34);

    if (type > 0)
    {
        const auto& params = info (type).params;
        const int n = (int) params.size();
        const int dh = displayHeight();
        int top = 96 + (dh > 0 ? dh + 14 : 8);
        const int bottom = H - 66;

        int startIdx = 0;
        if (params[0].kind == Kind::Choice)
        {
            cells[0].choice.setBounds (12, top, W - 24, 30);
            top += 44; startIdx = 1;
        }
        const int count = n - startIdx;
        const int cols = n > 8 ? 3 : 2, rows = (count + cols - 1) / cols;
        const int availH = bottom - top, rowH = jmin (112, availH / jmax (1, rows));
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
    if (bg.getWidth() != W * 2 || bg.getHeight() != H * 2) buildBackground();
}

void ModulePanel::timerCallback()
{
    if (type <= 0) return;
    if (currentModel() != model) { model = currentModel(); applySkin(); }

    const float gr = proc.slotMeter[slot].load();
    const float lvl = Decibels::gainToDecibels (proc.slotLevel[slot].load(), -60.f);
    float targetVu = 0.f, targetLed = 0.f;
    if (skin.display == 2 || skin.display == 3) targetVu = jlimit (0.f, 1.f, gr / 20.f);
    if (skin.strip == 1) targetLed = jlimit (0.f, 1.f, (lvl + 42.f) / 42.f);
    if (skin.strip == 2 || skin.display == 4)
        targetLed = jlimit (0.f, 1.f, gr / (type == Gate ? 40.f : (type == DeEsser ? 12.f : (type == Limiter ? 12.f : 20.f))));

    vuPos  += (targetVu - vuPos) * 0.28f;
    ledPos += (targetLed - ledPos) * 0.45f;
    ++tick;
    if (skin.display >= 5 && (tick % 2 == 0)) updateSpectra();
    if (skin.display != 0) repaint (displayRect().expanded (6, 6));
}

void ModulePanel::updateSpectra()
{
    auto& sc = proc.scopes[slot];
    const int pos = sc.pos.load();
    constexpr int mask = RackProcessor::Scope::N - 1;
    const double sr = proc.getSampleRate() > 0 ? proc.getSampleRate() : 44100.0;

    if (skin.display == 5 || skin.display == 6)
    {
        specOut.analyse ([&] (int k) { const int i = (pos - 2048 + k) & mask; return 0.5f * (sc.outL[(size_t) i] + sc.outR[(size_t) i]); }, sr);
        if (type == Saturation || type == Preamp)
            specIn.analyse ([&] (int k) { return sc.inM[(size_t) ((pos - 2048 + k) & mask)]; }, sr);
    }
    if (skin.display == 7)
    {
        double ll = 1e-9, rr = 1e-9, lr = 0.0; float pl = 0.f, pr = 0.f;
        for (int k = 0; k < 1024; ++k)
        {
            const int i = (pos - 1024 + k) & mask;
            const float l = sc.outL[(size_t) i], r = sc.outR[(size_t) i];
            ll += (double) l * l; rr += (double) r * r; lr += (double) l * r;
            pl = jmax (pl, std::abs (l)); pr = jmax (pr, std::abs (r));
        }
        corr += ((float) (lr / std::sqrt (ll * rr)) - corr) * 0.3f;
        peakL = jmax (pl, peakL * 0.8f); peakR = jmax (pr, peakR * 0.8f);
    }
}

//------------------------------------------------------------------------------
void ModulePanel::paint (Graphics& g)
{
    const int W = getWidth(), H = getHeight();
    g.setImageResamplingQuality (Graphics::highResamplingQuality);
    g.drawImage (bg, 0, 0, W, H, 0, 0, bg.getWidth(), bg.getHeight());
    if (type <= 0) return;

    const auto& inf = info (type);
    const bool darkText = skin.text.getPerceivedBrightness() < 0.5f;

    // plaque de nom gravée
    auto plate = Rectangle<float> (12.f, 46.f, (float) W - 24.f, 40.f);
    g.setGradientFill (ColourGradient (darkText ? Colours::white.withAlpha (0.34f) : Colours::black.withAlpha (0.32f), 0, plate.getY(),
                                       darkText ? Colours::white.withAlpha (0.2f) : Colours::black.withAlpha (0.18f), 0, plate.getBottom(), false));
    g.fillRoundedRectangle (plate, 3.f);
    g.setColour (Colours::black.withAlpha (0.5f)); g.drawRoundedRectangle (plate, 3.f, 1.2f);
    g.setColour (Colours::white.withAlpha (0.18f)); g.drawHorizontalLine ((int) plate.getBottom(), plate.getX() + 2.f, plate.getRight() - 2.f);

    auto engraved = [&g, darkText] (const String& txt, Rectangle<int> r, Font f, Colour col)
    {
        g.setFont (f);
        g.setColour (darkText ? Colours::white.withAlpha (0.55f) : Colours::black.withAlpha (0.7f));
        g.drawText (txt, r.translated (0, 1), Justification::centred);
        g.setColour (col);
        g.drawText (txt, r, Justification::centred);
    };
    engraved (inf.name, { 12, 48, W - 24, 22 }, Font (FontOptions (17.f, Font::bold).withKerningFactor (0.1f)), skin.text);
    String sub = inf.tag;
    if (! inf.params.empty() && inf.params[0].kind == Kind::Choice)
        sub = (isMaster ? String ("MASTER  -  ") : String()) + inf.params[0].format ((float) model).toUpperCase() + " STYLE";
    engraved (sub, { 12, 69, W - 24, 14 }, Font (FontOptions (9.f, Font::bold).withKerningFactor (0.12f)), skin.legend.withAlpha (0.9f));

    if (displayHeight() > 0) paintDisplay (g, displayRect().toFloat());

    g.setColour (skin.legend.withAlpha (0.75f));
    g.setFont (Font (FontOptions (9.f, Font::bold).withKerningFactor (0.1f)));
    g.drawText (isMaster ? "MASTER BUS" : String (slot + 1), 0, H - 22, W, 12, Justification::centred);
}

void ModulePanel::drawScreen (Graphics& g, Rectangle<float> r)
{
    // cadre chromé + écran sombre légèrement teinté (phosphore), avec lignes de balayage
    g.setGradientFill (ColourGradient (Colour (0xffa9adb1), 0, r.getY(), Colour (0xff3f4246), 0, r.getBottom(), false));
    g.fillRoundedRectangle (r, 5.f);
    auto s = r.reduced (2.5f);
    g.setGradientFill (ColourGradient (Colour (0xff0b1210).interpolatedWith (skin.accent, 0.08f), 0, s.getY(), Colour (0xff030505), 0, s.getBottom(), false));
    g.fillRoundedRectangle (s, 3.5f);
    g.setColour (Colours::white.withAlpha (0.025f));
    for (float y = s.getY(); y < s.getBottom(); y += 3.f) g.drawHorizontalLine ((int) y, s.getX(), s.getRight());
    g.setColour (Colours::black.withAlpha (0.8f)); g.drawRoundedRectangle (s, 3.5f, 1.4f);
}

void ModulePanel::drawLeds (Graphics& g, Rectangle<float> r, float pos, bool levelColours)
{
    g.setGradientFill (ColourGradient (Colour (0xff9a9ea2), 0, r.getY(), Colour (0xff3b3e42), 0, r.getBottom(), false));
    g.fillRoundedRectangle (r, 4.f);
    auto s = r.reduced (2.f);
    g.setColour (Colour (0xff070809)); g.fillRoundedRectangle (s, 3.f);
    g.setColour (Colours::black); g.drawRoundedRectangle (s, 3.f, 1.f);
    const int n = 12;
    auto in = s.reduced (6.f, 5.f);
    const float sw = in.getWidth() / (float) n;
    const int lit = (int) std::ceil (pos * (float) n - 0.001f);
    for (int i = 0; i < n; ++i)
    {
        Colour c = skin.accent;
        if (levelColours) c = i < 8 ? Colour (0xff3be36a) : (i < 11 ? Colour (0xffffc23e) : Colour (0xffff4338));
        auto seg = Rectangle<float> (in.getX() + (float) i * sw + 1.f, in.getY(), sw - 2.f, in.getHeight());
        const bool on = i < lit;
        if (on) { g.setColour (c.withAlpha (0.25f)); g.fillRoundedRectangle (seg.expanded (2.f), 3.f); }
        g.setGradientFill (ColourGradient (on ? c.brighter (0.4f) : c.darker (0.85f).withAlpha (0.6f), 0, seg.getY(),
                                           on ? c.darker (0.2f) : c.darker (1.2f).withAlpha (0.6f), 0, seg.getBottom(), false));
        g.fillRoundedRectangle (seg, 1.5f);
    }
}

void ModulePanel::drawVU (Graphics& g, Rectangle<float> r, float pos)
{
    auto outer = r;
    if (skin.hasStripe) { g.setColour (skin.stripe); g.fillRoundedRectangle (r, 7.f); outer = r.reduced (6.f); }
    g.setGradientFill (ColourGradient (Colour (0xfff1f3f4), 0, outer.getY(), Colour (0xff4c5054), 0, outer.getBottom(), false));
    g.fillRoundedRectangle (outer, 5.f);
    auto inner = outer.reduced (3.f);
    g.setColour (Colour (0xff070707)); g.fillRoundedRectangle (inner, 4.f);
    auto face = inner.reduced (2.5f);

    const bool cream = (skin.display == 2);
    const Colour c0 = cream ? Colour (0xfffdf3d2) : Colour (0xfffcfaf2), c1 = cream ? Colour (0xffc2ae6e) : Colour (0xffc4bfaa);
    g.setGradientFill (ColourGradient (c0, face.getCentreX(), face.getY() + face.getHeight() * 0.3f, c1, face.getX(), face.getBottom(), true));
    g.fillRoundedRectangle (face, 3.f);

    Graphics::ScopedSaveState ss (g);
    g.reduceClipRegion (face.toNearestInt());
    const float h = face.getHeight(), tm = 0.8f;
    const Point<float> pv (face.getCentreX(), face.getBottom() + h * 0.22f);
    const float Rr = h * 1.08f;
    const Colour ink (0xff221e19), red (0xffbf3626);

    Path arc1, arc2;
    arc1.addCentredArc (pv.x, pv.y, Rr, Rr, 0.f, -tm, tm * 0.4f, true);
    arc2.addCentredArc (pv.x, pv.y, Rr, Rr, 0.f, tm * 0.4f, tm, true);
    g.setColour (ink); g.strokePath (arc1, PathStrokeType (1.3f));
    g.setColour (red); g.strokePath (arc2, PathStrokeType (3.4f));
    g.setFont (Font (FontOptions (8.f, Font::bold)));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = -tm + 2.f * tm * (float) i / 10.f, len = (i % 2 == 0) ? 8.f : 4.5f;
        pointer (g, pv.x, pv.y, a, Rr - len, Rr, i >= 8 ? red : ink, (i % 2 == 0) ? 1.4f : 1.f);
        if (i % 2 == 0)
        {
            g.setColour (i >= 8 ? red : ink);
            g.drawText (String (i * 2), Rectangle<float> (pv.x + std::sin (a) * (Rr - 17.f) - 8.f, pv.y - std::cos (a) * (Rr - 17.f) - 5.f, 16.f, 10.f).toNearestInt(), Justification::centred);
        }
    }
    g.setColour (ink.withAlpha (0.85f));
    g.setFont (Font (FontOptions (9.f, Font::bold).withKerningFactor (0.12f)));
    g.drawText (cream ? "GAIN REDUCTION" : "GR  dB", face.withTrimmedTop (h * 0.52f).withTrimmedBottom (h * 0.12f).toNearestInt(), Justification::centredBottom);

    // aiguille avec ombre
    const float a = -tm + 2.f * tm * pos;
    g.setColour (Colours::black.withAlpha (0.28f));
    g.drawLine (pv.x + 2.f, pv.y + 2.f, pv.x + std::sin (a) * (Rr - 3.f) + 2.f, pv.y - std::cos (a) * (Rr - 3.f) + 2.f, 2.2f);
    pointer (g, pv.x, pv.y, a, 0.f, Rr - 3.f, Colour (0xff121212), 1.8f);
    // vitre
    g.setGradientFill (ColourGradient (Colours::white.withAlpha (0.30f), 0, face.getY(), Colours::white.withAlpha (0.0f), 0, face.getY() + h * 0.55f, false));
    g.fillRect (face.withHeight (h * 0.55f));
    g.setColour (Colours::black.withAlpha (0.35f)); g.drawRoundedRectangle (face, 3.f, 1.5f);
}

void ModulePanel::paintDisplay (Graphics& g, Rectangle<float> r)
{
    const int mh = mainDisplayHeight();
    const auto main = r.withHeight ((float) mh);
    switch (skin.display)
    {
        case 2: case 3: drawVU (g, main, vuPos); break;
        case 4:         drawLeds (g, main, ledPos, false); break;
        case 5:         drawCurve (g, main); break;
        case 6:         drawSpectrum (g, main, type == Saturation || type == Preamp,
                                      type == DeEsser ? realValues()[0] : 0.f, 0.5f); break;
        case 7:         drawScope (g, main); break;
        case 8:         drawTaps (g, main); break;
        default: break;
    }
    if (skin.strip > 0)
        drawLeds (g, r.withTrimmedTop ((float) mh + 4.f).withHeight (26.f), ledPos, skin.strip == 1);
}

void ModulePanel::drawSpectrum (Graphics& g, Rectangle<float> r, bool showInput, float markerHz, float fillAlpha)
{
    drawScreen (g, r);
    auto a = r.reduced (7.f, 8.f).withTrimmedBottom (8.f);
    g.setColour (Colours::white.withAlpha (0.07f));
    for (float f : { 100.f, 1000.f, 10000.f })
        g.drawVerticalLine ((int) (a.getX() + a.getWidth() * std::log10 (f / 20.f) / 3.f), a.getY(), a.getBottom());
    for (int i = 1; i < 4; ++i) g.drawHorizontalLine ((int) (a.getY() + a.getHeight() * (float) i / 4.f), a.getX(), a.getRight());

    auto build = [&a] (const std::array<float, SpectrumAnalyzer::kBands>& b, Path& line, Path& fill)
    {
        for (int i = 0; i < SpectrumAnalyzer::kBands; ++i)
        {
            const float x = a.getX() + a.getWidth() * ((float) i + 0.5f) / (float) SpectrumAnalyzer::kBands;
            const float y = a.getBottom() - b[(size_t) i] * a.getHeight();
            if (i == 0) { line.startNewSubPath (x, y); fill.startNewSubPath (x, a.getBottom()); fill.lineTo (x, y); }
            else        { line.lineTo (x, y); fill.lineTo (x, y); }
        }
        fill.lineTo (a.getRight(), a.getBottom()); fill.closeSubPath();
    };
    if (showInput)
    {
        Path l, f; build (specIn.bands, l, f);
        g.setColour (Colours::white.withAlpha (0.5f)); g.strokePath (l, PathStrokeType (1.1f));
    }
    Path line, fill; build (specOut.bands, line, fill);
    g.setGradientFill (ColourGradient (skin.accent.withAlpha (fillAlpha), 0, a.getY(), skin.accent.withAlpha (0.04f), 0, a.getBottom(), false));
    g.fillPath (fill);
    g.setColour (skin.accent); g.strokePath (line, PathStrokeType (1.5f));

    if (markerHz > 0.f)
    {
        const float x = a.getX() + a.getWidth() * std::log10 (markerHz / 20.f) / 3.f;
        g.setColour (Colour (0xffffd24a).withAlpha (0.9f));
        g.drawVerticalLine ((int) x, a.getY(), a.getBottom());
        g.fillRect (x - 3.f, a.getY(), 7.f, 3.f);
    }
    g.setColour (Colours::white.withAlpha (0.4f));
    g.setFont (Font (FontOptions (7.5f, Font::bold)));
    int k = 0;
    for (auto lab : { "100", "1k", "10k" })
    {
        const float f = k == 0 ? 100.f : (k == 1 ? 1000.f : 10000.f);
        g.drawText (lab, Rectangle<float> (a.getX() + a.getWidth() * std::log10 (f / 20.f) / 3.f - 12.f, r.getBottom() - 12.f, 24.f, 9.f).toNearestInt(), Justification::centred);
        ++k;
    }
}

void ModulePanel::drawCurve (Graphics& g, Rectangle<float> r)
{
    drawScreen (g, r);
    auto a = r.reduced (7.f, 8.f).withTrimmedBottom (8.f);
    const auto v = realValues();
    const double sr = proc.getSampleRate() > 0 ? proc.getSampleRate() : 44100.0;
    const bool isFilter = (type == Filters);
    const float lo = isFilter ? -36.f : -18.f, hi = isFilter ? 6.f : 18.f;
    auto yFor = [&] (float db) { return a.getBottom() - (jlimit (lo, hi, db) - lo) / (hi - lo) * a.getHeight(); };

    g.setColour (Colours::white.withAlpha (0.07f));
    for (float f : { 100.f, 1000.f, 10000.f })
        g.drawVerticalLine ((int) (a.getX() + a.getWidth() * std::log10 (f / 20.f) / 3.f), a.getY(), a.getBottom());
    for (float d = isFilter ? -30.f : -12.f; d <= hi; d += isFilter ? 12.f : 12.f)
        g.drawHorizontalLine ((int) yFor (d), a.getX(), a.getRight());
    g.setColour (Colours::white.withAlpha (0.16f));
    g.drawHorizontalLine ((int) yFor (0.f), a.getX(), a.getRight());

    // spectre du signal en filigrane
    Path sp;
    for (int i = 0; i < SpectrumAnalyzer::kBands; ++i)
    {
        const float x = a.getX() + a.getWidth() * ((float) i + 0.5f) / (float) SpectrumAnalyzer::kBands, y = a.getBottom() - specOut.bands[(size_t) i] * a.getHeight() * 0.8f;
        if (i == 0) { sp.startNewSubPath (x, a.getBottom()); sp.lineTo (x, y); } else sp.lineTo (x, y);
    }
    sp.lineTo (a.getRight(), a.getBottom()); sp.closeSubPath();
    g.setColour (Colours::white.withAlpha (0.10f)); g.fillPath (sp);

    Path path, fill;
    for (int i = 0; i <= 100; ++i)
    {
        const float f = 20.f * std::pow (1000.f, (float) i / 100.f);
        const float db = type == ProEQ ? proEqMagDb (v.data(), f, sr) : (type == ConsoleEQ ? consoleEqMagDb (v.data(), f, sr) : filtersMagDb (v.data(), f, sr));
        const float x = a.getX() + a.getWidth() * (float) i / 100.f, y = yFor (db);
        if (i == 0) { path.startNewSubPath (x, y); fill.startNewSubPath (x, yFor (0.f)); fill.lineTo (x, y); }
        else        { path.lineTo (x, y); fill.lineTo (x, y); }
    }
    fill.lineTo (a.getRight(), yFor (0.f)); fill.closeSubPath();
    g.setColour (skin.accent.withAlpha (0.2f)); g.fillPath (fill);
    g.setColour (skin.accent.withAlpha (0.35f)); g.strokePath (path, PathStrokeType (4.f));
    g.setColour (skin.accent.brighter (0.3f)); g.strokePath (path, PathStrokeType (1.7f));

    auto xF = [&a] (float f) { return a.getX() + a.getWidth() * std::log10 (f / 20.f) / 3.f; };
    if (type == ProEQ)
        for (int b = 0; b < 4; ++b)
        {
            const float x = xF (v[(size_t) b * 3]), y = yFor (v[(size_t) b * 3 + 1]);
            g.setColour (Colours::black); g.fillEllipse (x - 5.5f, y - 5.5f, 11.f, 11.f);
            g.setColour (skin.accent);    g.drawEllipse (x - 5.5f, y - 5.5f, 11.f, 11.f, 1.4f);
            g.setFont (Font (FontOptions (8.f, Font::bold)));
            g.drawText (String (b + 1), Rectangle<float> (x - 5.5f, y - 5.5f, 11.f, 11.f).toNearestInt(), Justification::centred);
        }
    if (isFilter)
    {
        g.setColour (skin.accent.withAlpha (0.8f));
        if (v[0] > 21.f) g.drawVerticalLine ((int) xF (v[0]), a.getY(), a.getBottom());
        if (v[1] < 19500.f) g.drawVerticalLine ((int) xF (v[1]), a.getY(), a.getBottom());
    }
    g.setColour (Colours::white.withAlpha (0.4f));
    g.setFont (Font (FontOptions (7.5f, Font::bold)));
    g.drawText ("100", Rectangle<float> (xF (100.f) - 12.f, r.getBottom() - 12.f, 24.f, 9.f).toNearestInt(), Justification::centred);
    g.drawText ("1k", Rectangle<float> (xF (1000.f) - 12.f, r.getBottom() - 12.f, 24.f, 9.f).toNearestInt(), Justification::centred);
    g.drawText ("10k", Rectangle<float> (xF (10000.f) - 12.f, r.getBottom() - 12.f, 24.f, 9.f).toNearestInt(), Justification::centred);
}

void ModulePanel::drawScope (Graphics& g, Rectangle<float> r)
{
    const float side = r.getHeight();
    auto sc = r.withWidth (side);
    drawScreen (g, sc);
    auto a = sc.reduced (7.f);
    const auto c = a.getCentre();
    g.setColour (Colours::white.withAlpha (0.1f));
    g.drawLine (a.getX(), a.getBottom(), a.getRight(), a.getY(), 1.f);       // axes L / R
    g.drawLine (a.getX(), a.getY(), a.getRight(), a.getBottom(), 1.f);
    g.drawVerticalLine ((int) c.x, a.getY(), a.getBottom());
    g.drawHorizontalLine ((int) c.y, a.getX(), a.getRight());
    g.setColour (Colours::white.withAlpha (0.45f));
    g.setFont (Font (FontOptions (8.f, Font::bold)));
    g.drawText ("L", Rectangle<float> (a.getX(), a.getY() - 1.f, 12.f, 10.f).toNearestInt(), Justification::centred);
    g.drawText ("R", Rectangle<float> (a.getRight() - 12.f, a.getY() - 1.f, 12.f, 10.f).toNearestInt(), Justification::centred);

    auto& s = proc.scopes[slot];
    const int pos = s.pos.load();
    constexpr int mask = RackProcessor::Scope::N - 1;
    const float k = a.getWidth() * 0.5f * 1.25f;
    g.setColour (skin.accent.withAlpha (0.55f));
    for (int i = 0; i < 700; ++i)
    {
        const int idx = (pos - 700 + i) & mask;
        const float l = s.outL[(size_t) idx], rr = s.outR[(size_t) idx];
        const float x = c.x + (rr - l) * 0.7071f * k * 1.6f, y = c.y - (l + rr) * 0.7071f * k * 1.6f;
        if (a.contains (x, y)) g.fillRect (x - 0.7f, y - 0.7f, 1.6f, 1.6f);
    }

    // niveaux L / R et corrélation
    auto st = r.withTrimmedLeft (side + 6.f);
    auto bar = [&g] (Rectangle<float> b, float v)
    {
        g.setColour (Colour (0xff070809)); g.fillRoundedRectangle (b, 2.f);
        const float db = Decibels::gainToDecibels (v, -60.f), f = jlimit (0.f, 1.f, (db + 60.f) / 60.f);
        g.setColour (db > -3.f ? Colour (0xffff4338) : (db > -14.f ? Colour (0xffffc23e) : Colour (0xff3be36a)));
        g.fillRoundedRectangle (b.withTop (b.getBottom() - b.getHeight() * f), 2.f);
    };
    const float bh = st.getHeight() - 36.f;
    bar (Rectangle<float> (st.getX() + 4.f, st.getY() + 2.f, 12.f, bh), peakL);
    bar (Rectangle<float> (st.getX() + 22.f, st.getY() + 2.f, 12.f, bh), peakR);
    g.setColour (Colours::white.withAlpha (0.75f));
    g.setFont (Font (FontOptions (8.f, Font::bold)));
    g.drawText ("L", Rectangle<float> (st.getX() + 4.f, st.getY() + bh + 3.f, 12.f, 9.f).toNearestInt(), Justification::centred);
    g.drawText ("R", Rectangle<float> (st.getX() + 22.f, st.getY() + bh + 3.f, 12.f, 9.f).toNearestInt(), Justification::centred);
    auto cb = Rectangle<float> (st.getX() + 2.f, st.getBottom() - 16.f, st.getWidth() - 4.f, 8.f);
    g.setColour (Colour (0xff070809)); g.fillRoundedRectangle (cb, 2.f);
    g.setColour (Colours::white.withAlpha (0.25f)); g.drawVerticalLine ((int) cb.getCentreX(), cb.getY(), cb.getBottom());
    g.setColour (corr < 0.f ? Colour (0xffff4338) : Colour (0xff3be36a));
    const float cx0 = cb.getCentreX(), cx1 = cx0 + corr * cb.getWidth() * 0.5f;
    g.fillRect (jmin (cx0, cx1), cb.getY() + 1.f, std::abs (cx1 - cx0), cb.getHeight() - 2.f);
    g.setColour (Colours::white.withAlpha (0.75f));
    g.drawText ("PHASE", Rectangle<float> (st.getX(), st.getBottom() - 9.f, st.getWidth(), 9.f).toNearestInt(), Justification::centred);
}

void ModulePanel::drawTaps (Graphics& g, Rectangle<float> r)
{
    drawScreen (g, r);
    auto a = r.reduced (9.f, 8.f);
    const auto v = realValues();
    const float fb = v[1] / 100.f, mix = jmax (0.2f, v[3] / 100.f);
    const bool pp = (int) v[4] == 1;
    const int taps = 8;
    const float base = a.getBottom() - 14.f, maxH = a.getHeight() - 22.f;
    g.setColour (Colours::white.withAlpha (0.18f)); g.drawHorizontalLine ((int) base, a.getX(), a.getRight());
    for (int k = 0; k < taps; ++k)
    {
        const float x = a.getX() + 4.f + (a.getWidth() - 14.f) * (float) k / (float) (taps - 1);
        const float amp = k == 0 ? 1.f : mix * std::pow (fb, (float) (k - 1));
        const float h = jmax (2.f, amp * maxH);
        const Colour c = k == 0 ? Colours::white : ((pp && k % 2 == 0) ? Colour (0xffffa04a) : skin.accent);
        auto bar = Rectangle<float> (x - 3.5f, base - h, 7.f, h);
        g.setColour (c.withAlpha (0.25f)); g.fillRoundedRectangle (bar.expanded (2.f), 3.f);
        g.setGradientFill (ColourGradient (c.brighter (0.3f), 0, bar.getY(), c.darker (0.3f), 0, bar.getBottom(), false));
        g.fillRoundedRectangle (bar, 2.f);
    }
    g.setColour (Colours::white.withAlpha (0.7f));
    g.setFont (Font (FontOptions (8.5f, Font::bold)));
    g.drawText (String ((int) v[0]) + " ms", Rectangle<float> (a.getX(), a.getBottom() - 11.f, a.getWidth(), 10.f).toNearestInt(), Justification::centredLeft);
    g.drawText (pp ? "PING-PONG" : "STEREO", Rectangle<float> (a.getX(), a.getBottom() - 11.f, a.getWidth(), 10.f).toNearestInt(), Justification::centredRight);
}

//==============================================================================
// TUILE « ADD MODULE »
//==============================================================================
void AddTile::paint (Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced (12.f, 8.f);
    g.setColour (Colours::white.withAlpha (hover ? 0.08f : 0.03f));
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
    buildStaticImages();

    viewport.setViewedComponent (&holder, false);
    viewport.setScrollBarsShown (false, true);
    viewport.setScrollBarThickness (kScroll);
    content.addAndMakeVisible (viewport);

    for (int s = 0; s < kNumSlots; ++s)
    {
        panels[(size_t) s] = std::make_unique<ModulePanel> (proc, s, kPanelW);
        panels[(size_t) s]->onMove = [this] (int slot, int dir)
        {
            proc.moveModule (slot, dir);
            for (auto& pn : panels) pn->refresh();      // resynchronise toutes les faces
            holder.repaint();
        };
        panels[(size_t) s]->onRemove = [this] (int slot)
        {
            proc.removeModule (slot);
            for (auto& pn : panels) pn->refresh();
            layoutForCount();
        };
        holder.addChildComponent (*panels[(size_t) s]);
    }
    addTile.onPick = [this] (int type)
    {
        if (proc.addModule (type))
        {
            for (auto& pn : panels) pn->refresh();
            layoutForCount();
        }
    };
    holder.addChildComponent (addTile);

    master = std::make_unique<ModulePanel> (proc, kMasterSlot, kMasterW);
    content.addAndMakeVisible (*master);

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
        l.setFont (Font (FontOptions (11.f, Font::bold).withKerningFactor (0.1f)));
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

    content.setBounds (0, 0, kContentW, kH);
    presetBox.setBounds (190, 14, 130, 26);
    inLabel.setBounds (340, 8, 44, 38);   inKnob.setBounds (384, 8, 104, 38);
    outLabel.setBounds (500, 8, 52, 38);  outKnob.setBounds (552, 8, 104, 38);
    bypassBtn.setBounds (kContentW - 150, 12, 120, 30);
    viewport.setBounds (kRail, kTop, kViewW, kViewH);
    master->setBounds (kRail + kViewW + kGap, kTop, kMasterW, ModulePanel::kHeight);

    const float initialScale = 0.8f;       // setResizeLimits peut redimensionner la fenêtre : on mémorise l'échelle voulue
    setResizable (true, true);
    setResizeLimits ((int) (kContentW * 0.5), (int) (kH * 0.5), (int) (kContentW * 1.5), (int) (kH * 1.5));
    if (auto* c = getConstrainer()) c->setFixedAspectRatio ((double) kContentW / (double) kH);
    setSize (roundToInt ((float) kContentW * initialScale), roundToInt ((float) kH * initialScale));
    layoutForCount();
    startTimerHz (24);
}

RackEditor::~RackEditor() { stopTimer(); setLookAndFeel (nullptr); }

void RackEditor::buildStaticImages()
{
    // côtés en bois
    {
        const int w = 28, h = kH;
        woodImg = Image (Image::ARGB, w * 2, h * 2, true);
        Graphics g (woodImg);
        g.addTransform (AffineTransform::scale (2.f));
        ColourGradient base (Colour (0xff3b1c0d), 0, 0, Colour (0xff3b1c0d), (float) w, 0, false);
        base.addColour (0.35, Colour (0xff7b4524));
        base.addColour (0.7, Colour (0xff5d3118));
        g.setGradientFill (base); g.fillAll();
        Random rnd (11);
        for (int k = 0; k < 130; ++k)
        {
            Path p;
            const float x0 = rnd.nextFloat() * (float) w, ph = rnd.nextFloat() * 6.f, am = 0.4f + rnd.nextFloat() * 1.6f;
            for (int y = 0; y <= h; y += 14)
            {
                const float x = x0 + std::sin ((float) y * 0.011f + ph) * am + std::sin ((float) y * 0.05f + ph * 2.f) * 0.3f;
                if (y == 0) p.startNewSubPath (x, 0.f); else p.lineTo (x, (float) y);
            }
            g.setColour ((rnd.nextInt (3) == 0 ? Colour (0xffc58a50) : Colour (0xff1c0b04)).withAlpha (0.07f + rnd.nextFloat() * 0.1f));
            g.strokePath (p, PathStrokeType (0.5f + rnd.nextFloat() * 1.2f));
        }
        g.setGradientFill (ColourGradient (Colours::white.withAlpha (0.12f), 0, 0, Colours::transparentWhite, 7.f, 0, false));
        g.fillRect (0.f, 0.f, 7.f, (float) h);
        g.setGradientFill (ColourGradient (Colours::black.withAlpha (0.55f), (float) w, 0, Colours::transparentBlack, (float) w - 9.f, 0, false));
        g.fillRect ((float) w - 9.f, 0.f, 9.f, (float) h);
    }
    // barre du haut : métal brossé sombre
    {
        topImg = Image (Image::ARGB, kContentW * 2, kTop * 2, true);
        Graphics g (topImg);
        g.addTransform (AffineTransform::scale (2.f));
        g.setGradientFill (ColourGradient (Colour (0xff35383c), 0, 0, Colour (0xff141517), 0, (float) kTop, false));
        g.fillAll();
        Random rnd (3);
        for (int k = 0; k < 1500; ++k)
        {
            const float y = rnd.nextFloat() * (float) kTop, x = rnd.nextFloat() * (float) kContentW;
            g.setColour ((rnd.nextBool() ? Colours::white : Colours::black).withAlpha (0.02f + rnd.nextFloat() * 0.045f));
            g.drawLine (x, y, x + 10.f + rnd.nextFloat() * 90.f, y, 0.55f);
        }
        g.setColour (Colours::white.withAlpha (0.2f)); g.drawHorizontalLine (1, 0.f, (float) kContentW);
    }
}

void RackEditor::layoutForCount()
{
    const int n = proc.getNumModules();
    const bool full = n >= kNumSlots;
    for (int s = 0; s < kNumSlots; ++s)
    {
        panels[(size_t) s]->setVisible (s < n);
        panels[(size_t) s]->setBounds (s * kPanelW, 0, kPanelW, ModulePanel::kHeight);
    }
    addTile.setVisible (! full);
    addTile.setBounds (n * kPanelW, 0, kPanelW, ModulePanel::kHeight);

    const int cells = jmax (kVisible, n + (full ? 0 : 1));
    holder.setSize (cells * kPanelW, ModulePanel::kHeight);
    if (n > shownCount && shownCount >= 0)      // un module vient d'être ajouté : on fait défiler jusqu'à lui
        viewport.setViewPosition (jmax (0, (n + (full ? 0 : 1)) * kPanelW - kViewW), 0);
    shownCount = n;
    holder.repaint();
    content.repaint();
}

void RackEditor::resized()
{
    if (getWidth() <= 0) return;
    scale = (float) getWidth() / (float) kContentW;
    content.setTransform (AffineTransform::scale (scale));
    content.setBounds (0, 0, kContentW, kH);
}

void RackEditor::paint (Graphics& g) { g.fillAll (Colour (0xff0b0b0c)); }

void RackEditor::timerCallback()
{
    dispIn  = jmax (proc.inPeak.load(),  dispIn  * 0.88f);
    dispOut = jmax (proc.outPeak.load(), dispOut * 0.88f);
    content.repaint (0, 0, kContentW, kTop);
}

void RackEditor::paintHolder (Graphics& g)
{
    g.fillAll (Colour (0xff0a0a0b));
    const int n = proc.getNumModules();
    // plaques de fermeture vierges, comme dans un vrai rack
    for (int c = n + 1; c < holder.getWidth() / kPanelW; ++c)
    {
        auto r = Rectangle<float> ((float) (c * kPanelW) + 2.f, 0.f, (float) kPanelW - 4.f, (float) ModulePanel::kHeight);
        g.setGradientFill (ColourGradient (Colour (0xff2c2e31), 0, 0, Colour (0xff181a1c), 0, r.getBottom(), false));
        g.fillRect (r);
        Random rnd (c * 13);
        for (int k = 0; k < 500; ++k)
        {
            g.setColour ((rnd.nextBool() ? Colours::white : Colours::black).withAlpha (0.02f + rnd.nextFloat() * 0.04f));
            const float y = rnd.nextFloat() * r.getHeight(), x = r.getX() + rnd.nextFloat() * r.getWidth();
            g.drawLine (x, y, x + 12.f + rnd.nextFloat() * 60.f, y, 0.5f);
        }
        g.setColour (Colours::white.withAlpha (0.2f)); g.drawHorizontalLine (1, r.getX(), r.getRight());
        g.setColour (Colours::black.withAlpha (0.7f)); g.drawRect (r, 1.5f);
        for (int i = 0; i < 4; ++i)
            drawScrew (g, r.getX() + ((i % 2 == 0) ? 14.f : r.getWidth() - 14.f), (i < 2) ? 12.f : r.getBottom() - 12.f, (float) (i + c));
    }
    if (n == 0)
    {
        g.setColour (Colours::white.withAlpha (0.3f));
        g.setFont (Font (FontOptions (16.f)));
        g.drawFittedText ("Your rack is empty.\n\nClick  +  to add a module, or pick a ready-made chain at the top.",
                          kPanelW + 20, 230, kPanelW * 3 - 40, 120, Justification::centredLeft, 4);
    }
}

void RackEditor::paintContent (Graphics& g)
{
    g.fillAll (Colour (0xff0b0b0c));

    // barre du haut
    g.setImageResamplingQuality (Graphics::highResamplingQuality);
    g.drawImage (topImg, 0, 0, kContentW, kTop, 0, 0, topImg.getWidth(), topImg.getHeight());
    g.setColour (Colours::black); g.drawHorizontalLine (kTop - 1, 0.f, (float) kContentW);
    auto engrave = [&g] (const String& t, Rectangle<int> r, Font f, Colour c, Justification j)
    {
        g.setFont (f);
        g.setColour (Colours::black.withAlpha (0.85f)); g.drawText (t, r.translated (0, 1), j);
        g.setColour (c); g.drawText (t, r, j);
    };
    engrave ("RACK STRIP", { 18, 5, 170, 28 }, Font (FontOptions (22.f, Font::bold).withKerningFactor (0.14f)), Colour (0xfff0e6cf), Justification::centredLeft);
    engrave ("MODULAR CHANNEL CHAIN", { 18, 32, 170, 14 }, Font (FontOptions (8.5f, Font::bold).withKerningFactor (0.16f)), Colour (0xffe0a03a), Justification::centredLeft);

    auto meter = [&g] (float v, int x, int y)
    {
        const float db = Decibels::gainToDecibels (v, -60.f);
        const float frac = jlimit (0.f, 1.f, (db + 60.f) / 60.f);
        g.setColour (Colour (0xff060708)); g.fillRoundedRectangle ((float) x, (float) y, 100.f, 8.f, 2.f);
        g.setColour (db > -1.f ? Colour (0xffff4040) : (db > -12.f ? Colour (0xffffb23e) : Colour (0xff4cff7a)));
        g.fillRoundedRectangle ((float) x, (float) y, 100.f * frac, 8.f, 2.f);
        g.setColour (Colours::white.withAlpha (0.15f)); g.drawRoundedRectangle ((float) x, (float) y, 100.f, 8.f, 2.f, 1.f);
    };
    const int mx = kContentW - 290;
    meter (dispIn, mx, 14);
    meter (dispOut, mx, 31);
    g.setColour (Colours::white.withAlpha (0.6f));
    g.setFont (Font (FontOptions (9.f, Font::bold)));
    g.drawText ("IN", mx - 30, 12, 26, 10, Justification::centredRight);
    g.drawText ("OUT", mx - 34, 29, 30, 10, Justification::centredRight);

    // côtés en bois + rails de rack percés
    g.drawImage (woodImg, 0, kTop, 28, kH - kTop, 0, 0, woodImg.getWidth(), woodImg.getHeight());
    g.drawImage (woodImg, kContentW - 28, kTop, 28, kH - kTop, 0, 0, woodImg.getWidth(), woodImg.getHeight());
    for (int side = 0; side < 2; ++side)
    {
        const int x = side == 0 ? 28 : kContentW - kRail;
        g.setGradientFill (ColourGradient (Colour (0xff4a4d52), (float) x, 0, Colour (0xff25272a), (float) (x + 22), 0, false));
        g.fillRect (x, kTop, 22, kH - kTop);
        g.setColour (Colours::white.withAlpha (0.15f)); g.drawVerticalLine (x + 1, (float) kTop, (float) kH);
        g.setColour (Colour (0xff050506));
        for (int y = kTop + 28; y < kH - 14; y += 62)
            g.fillRoundedRectangle ((float) x + 5.f, (float) y, 12.f, 20.f, 4.f);
    }
    // base du rack
    g.setGradientFill (ColourGradient (Colour (0xff3b1c0d), 0, (float) kH - kBottom, Colour (0xff1c0d06), 0, (float) kH, false));
    g.fillRect (0, kH - kBottom, kContentW, kBottom);
    // séparation entre le rack défilant et le bus maître
    g.setColour (Colour (0xff050506)); g.fillRect (kRail + kViewW, kTop, kGap, kViewH);
}

