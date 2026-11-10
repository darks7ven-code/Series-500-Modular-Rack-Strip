#pragma once
#include "PluginEditor.h"
#include <BinaryData.h>

// Artwork coordinates are in the original 1672 x 941 design.
class AtelierEditor final : public juce::AudioProcessorEditor, private juce::Timer {
    using Rect=juce::Rectangle<int>;
    using String=juce::String;
    using Graphics=juce::Graphics;
    RackProcessor& p;
    juce::Image art=juce::ImageFileFormat::loadFrom(BinaryData::Panel_png,BinaryData::Panel_pngSize);
    int page=0, preview=rack::Compressor, focus=2, preset=-1;
    juce::RangedAudioParameter* drag=nullptr;
    float start=0;
    juce::TextEditor search;
    int downY=0;
    std::array<float,80> history {};
    juce::MemoryBlock saved;
    void toggle(const String& id){auto* v=p.apvts.getParameter(id);v->beginChangeGesture();v->setValueNotifyingHost(1-v->getValue());v->endChangeGesture();}

    struct Knob { int x,y,r,index; };
    struct Plate { int type,y,h; std::vector<Knob> knobs; };
    const std::vector<Plate> plates{
        {rack::Preamp,145,166,{{712,228,32,4},{833,228,32,1},{966,228,32,3}}},
        {rack::ProEQ,315,156,{{718,397,30,3},{827,397,30,4},{929,397,30,5}}},
        {rack::Compressor,475,157,{{717,553,31,1},{824,553,30,2},{928,553,30,3},{1031,553,30,4}}},
        {rack::Reverb,636,139,{{717,711,29,0},{833,711,29,1},{951,711,29,2},{1064,711,29,3}}}
    };
    struct Control { Rect bounds; String id; };
    std::vector<Control> controls;
    const int bank[6]={rack::Preamp,rack::ProEQ,rack::Compressor,rack::Reverb,rack::Saturation,rack::Delay};
    const int edges[5]={145,314,474,635,776};
    int num() const {return p.getNumModules();}
    void timerCallback() override {std::rotate(history.begin(),history.begin()+1,history.end());history.back()=p.outPeak.load();page=juce::jlimit(0,juce::jmax(0,(num()-1)/4),page);repaint();}
    void label(Graphics& g,Rect r,String s,juce::Colour bg,juce::Colour fg,int size){
        g.setColour(bg);g.fillRect(r);g.setColour(fg);g.setFont(juce::FontOptions(float(size),juce::Font::bold));g.drawFittedText(s,r.reduced(2),juce::Justification::centred,1);
    }
    void knob(Graphics& g,int x,int y,int radius,int sx,int sy,int sr,const String& id){
        auto* param=p.apvts.getParameter(id); if(!param)return;
        Graphics::ScopedSaveState save(g);
        juce::Path clip;clip.addEllipse(float(x-radius),float(y-radius),float(2*radius),float(2*radius));g.reduceClipRegion(clip);
        g.addTransform(juce::AffineTransform::rotation((param->getValue()-.5f)*4.65f,float(x),float(y)));
        g.drawImage(art,x-radius,y-radius,2*radius,2*radius,sx-sr,sy-sr,2*sr,2*sr);
        controls.push_back({{x-radius-4,y-radius-4,radius*2+8,radius*2+8},id});
    }
    void details(int slot){
        if(slot<0 || (slot>=num() && slot!=rack::kMasterSlot))return;
        auto panel=std::make_unique<ModulePanel>(p,slot,230);
        panel->onMove=[this](int s,int d){p.moveModule(s,d);};
        panel->onRemove=[this](int s){p.removeModule(s);};
        juce::CallOutBox::launchAsynchronously(std::move(panel),{getWidth()*3/4,55,1,1},this);
    }
    void add(int type){if(p.addModule(type)){focus=num()-1;page=focus/4;preview=type;}repaint();}
    void menu(bool chains){
        juce::PopupMenu m;
        if(chains){for(int i=0;i<p.numPresets();++i)m.addItem(i+1,p.presetName(i));}
        else {for(int i=1;i<rack::BusComp;++i)if(String(rack::info(i).name).containsIgnoreCase(search.getText()))m.addItem(i,rack::info(i).name);}
        m.showMenuAsync({},[self=juce::Component::SafePointer<AtelierEditor>(this),chains](int id){
            if(!self || !id)return;
            if(chains){self->preset=id-1;self->p.loadPreset(id-1);self->page=0;self->focus=0;}
            else self->add(id);
        });
    }
public:
    explicit AtelierEditor(RackProcessor& proc):AudioProcessorEditor(proc),p(proc){
        addAndMakeVisible(search);search.setColour(juce::TextEditor::backgroundColourId,juce::Colour(0xffd7d6dc));
        search.setColour(juce::TextEditor::textColourId,juce::Colours::black);search.setTextToShowWhenEmpty("Search modules...",juce::Colours::darkgrey);
        search.onReturnKey=[this]{menu(false);};setResizable(true,true);getConstrainer()->setFixedAspectRatio(1672.0/941.0);setResizeLimits(836,471,1672,941);setSize(1152,648);startTimerHz(20);
    }
    ~AtelierEditor() override {if(drag)drag->endChangeGesture();}
    void resized() override {search.setBounds(185*getWidth()/1672,135*getHeight()/941,268*getWidth()/1672,35*getHeight()/941);}
    void paint(Graphics& g) override {
        g.addTransform(juce::AffineTransform::scale(getWidth()/1672.f,getHeight()/941.f));
        g.drawImageAt(art,0,0);controls.clear();
        auto dark=juce::Colour(0xff29292d),light=juce::Colour(0xffd4d3d8);
        label(g,{632,28,220,37},preset<0?"Atelier 500":p.presetName(preset),light,juce::Colours::black,21);
        label(g,{1054,101,155,34},String(num())+" / 16    "+String(page+1)+"/"+String(juce::jmax(1,(num()+3)/4)),dark,juce::Colours::white,19);
        for(int i=0;i<4;++i){
            int slot=4*page+i,y=edges[i],h=edges[i+1]-y-3;
            if(slot>=num()){label(g,{515,y,716,h},"",dark,light,20);continue;}
            int t=p.getSlotType(slot);
            auto it=std::find_if(plates.begin(),plates.end(),[t](auto& a){return a.type==t;});
            if(it==plates.end()){label(g,{515,y,716,h},rack::info(t).name,dark,light,29);continue;}
            auto& a=*it;
            g.drawImage(art,515,y,716,h,515,a.y,716,a.h);
            for(auto k:a.knobs)knob(g,k.x,y+(k.y-a.y)*h/a.h,k.r,k.x,k.y,k.r,p.paramId(slot,k.index));
            if(t==rack::ProEQ){
                auto box=Rect(1006,y+28,164,95);g.setColour(juce::Colour(0xff102539));g.fillRect(box);
                std::array<float,rack::kMaxParams> v{};auto& pars=rack::info(t).params;
                for(size_t j=0;j<pars.size();++j)v[j]=pars[j].toReal(p.apvts.getParameter(p.paramId(slot,(int)j))->getValue());
                juce::Path curve;
                for(int x=0;x<box.getWidth();x+=2){float db=rack::proEqMagDb(v.data(),20.f*std::pow(1000.f,x/164.f),juce::jmax(44100.,p.getSampleRate()));
                    float yy=box.getCentreY()-juce::jlimit(-20.f,20.f,db)*2; if(x==0)curve.startNewSubPath(box.getX(),yy);else curve.lineTo(float(box.getX()+x),yy);}
                g.setColour(juce::Colours::deepskyblue);g.strokePath(curve,juce::PathStrokeType(2.f));
            }
            if(i!=int(it-plates.begin()) || page)label(g,{531,y+14,44,31},String(slot+1),dark,light,20);
            if(t==rack::Preamp && p.apvts.getParameter(p.paramId(slot,5))->getValue()<.5f){g.setColour(juce::Colours::orange);g.drawEllipse(1023.f,float(y+77),35.f,35.f,3.f);}
            if(t==rack::Reverb){label(g,{678,y+9,77,26},"SIZE",dark,light,17);label(g,{777,y+9,108,26},"DAMPING",dark,light,17);label(g,{908,y+9,86,26},"WIDTH",dark,light,17);}
            if(p.apvts.getRawParameterValue(p.onId(slot))->load()<.5f){g.setColour(juce::Colours::black.withAlpha(.57f));g.fillRect(515,y,716,h);}
        }
        label(g,{849,822,206,25},String(16-num())+" slots available",light,dark,16);
        if(preview==rack::Compressor && focus>=0 && focus<num() && p.getSlotType(focus)==preview){
            for(auto k:std::vector<Knob>{{1399,430,33,1},{1537,430,33,2},{1399,547,33,3},{1537,547,33,4}})
                knob(g,k.x,k.y,k.r,k.x,k.y,k.r,p.paramId(focus,k.index));
        } else if(preview!=rack::Compressor){
            label(g,{1288,147,363,646},rack::info(preview).name,dark,light,30);
            auto it=std::find_if(plates.begin(),plates.end(),[this](auto& a){return a.type==preview;});
            if(it!=plates.end())g.drawImage(art,1295,263,351,91,515,it->y,716,it->h);
        }
        g.setColour(juce::Colour(0xff1c1d21));g.fillRect(527,890,620,45);g.setColour(juce::Colour(0xff138de5));
        for(int i=0;i<80;++i){int h=juce::roundToInt(35*juce::jlimit(0.f,1.f,history[(size_t)i]));g.fillRect(540+7*i,930-h,4,h);}
        for(int i=0;i<2;++i){int x=i?1522:257;g.setColour(dark);g.fillRect(x,909,118,9);g.setColour(juce::Colours::limegreen);g.fillRect(x,909,int(118*juce::jlimit(0.f,1.f,(i?p.outPeak:p.inPeak).load())),9);
            auto id=i?"out_gain":"in_gain";knob(g,i?1411:136,915,18,i?1411:136,915,18,id);
            label(g,{i?1439:169,896,78,38},String(p.apvts.getRawParameterValue(id)->load(),1)+" dB",dark,light,17);}
    }
    void mouseDown(const juce::MouseEvent& e) override{
        int x=e.x*1672/getWidth(),y=e.y*941/getHeight();
        for(auto& c:controls)if(c.bounds.contains(x,y)){drag=p.apvts.getParameter(c.id);start=drag->getValue();downY=e.y;drag->beginChangeGesture();return;}
        if(y<90){if(x>568 && x<916)menu(true);else if(x>=928 && x<983)p.getStateInformation(saved);else if(x>=987 && x<1050 && saved.getSize())p.setStateInformation(saved.getData(),(int)saved.getSize());return;}
        if(x<129){if(y>260 && y<347)menu(true);else if(y>417){juce::PopupMenu m;m.addItem(1,"Master bus");m.addItem(2,"Rack bypass",true,p.apvts.getParameter("bypass")->getValue()>.5f);m.showMenuAsync({},[self=juce::Component::SafePointer<AtelierEditor>(this)](int id){if(self && id==1)self->details(rack::kMasterSlot);if(self && id==2)self->toggle("bypass");});}else menu(false);return;}
        if(x<479){if(y>=294 && y<723){int j=y<378?0:y<455?1:y<531?2:y<597?3:y<655?4:5;preview=bank[j];focus=-1;if(x>406)add(preview);}
            else if(y>808)add(preview);else menu(false);return;}
        if(x<1234){if(y<145){page=(page+1)%juce::jmax(1,(num()+3)/4);return;}if(y>=776){menu(false);return;}
            for(int i=0;i<4;++i)if(y<edges[i+1]){int s=page*4+i;if(s>=num())return;focus=s;preview=p.getSlotType(s);
                if(preview==rack::Preamp && x>1011 && x<1068 && y>edges[i]+67 && y<edges[i]+126){toggle(p.paramId(s,5));return;}
                if(x<603 && y<edges[i]+42){auto* on=p.apvts.getParameter(p.onId(s));on->beginChangeGesture();on->setValueNotifyingHost(1-on->getValue());on->endChangeGesture();}
                else if(e.mods.isPopupMenu() || e.getNumberOfClicks()>1)details(s);return;}
        }else if(y>=798 && y<851)add(preview);else if(y<797)details(focus);
    }
    void mouseDrag(const juce::MouseEvent& e) override{if(drag)drag->setValueNotifyingHost(juce::jlimit(0.f,1.f,start+(downY-e.y)/160.f));}
    void mouseUp(const juce::MouseEvent&) override{if(drag){drag->endChangeGesture();drag=nullptr;}}
};
