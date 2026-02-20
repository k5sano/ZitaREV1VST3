#include "PluginEditor.h"

static void setupSlider (juce::Slider& s, juce::Label& l, const juce::String& text,
                         juce::Component& parent)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    parent.addAndMakeVisible (s);

    l.setText (text, juce::dontSendNotification);
    l.setJustificationType (juce::Justification::centred);
    l.setFont (juce::Font (13.0f));
    parent.addAndMakeVisible (l);
}

//==============================================================================
ZitaRev1Editor::ZitaRev1Editor (ZitaRev1Processor& p)
    : AudioProcessorEditor (&p), processorRef (p),
      attDelay (p.apvts, "delay", sliderDelay),
      attRTMid (p.apvts, "rtmid", sliderRTMid),
      attRTLow (p.apvts, "rtlow", sliderRTLow),
      attDamp  (p.apvts, "damp",  sliderDamp),
      attMix   (p.apvts, "mix",   sliderMix)
{
    setupSlider (sliderDelay, labelDelay, "Delay",   *this);
    setupSlider (sliderRTMid, labelRTMid, "RT Mid",  *this);
    setupSlider (sliderRTLow, labelRTLow, "RT Low",  *this);
    setupSlider (sliderDamp,  labelDamp,  "Damping", *this);
    setupSlider (sliderMix,   labelMix,   "Mix",     *this);

    setSize (500, 200);
}

ZitaRev1Editor::~ZitaRev1Editor() {}

//==============================================================================
void ZitaRev1Editor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff2b2b2b));

    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (18.0f, juce::Font::bold));
    g.drawText ("ZitaRev1", getLocalBounds().removeFromTop (30),
                juce::Justification::centred);
}

void ZitaRev1Editor::resized()
{
    auto area = getLocalBounds().reduced (10);
    area.removeFromTop (30);  // title

    const int knobW = area.getWidth() / 5;
    const int labelH = 20;

    auto labelRow  = area.removeFromBottom (labelH);
    auto knobRow   = area;

    struct { juce::Slider* s; juce::Label* l; } items[] = {
        { &sliderDelay, &labelDelay },
        { &sliderRTMid, &labelRTMid },
        { &sliderRTLow, &labelRTLow },
        { &sliderDamp,  &labelDamp  },
        { &sliderMix,   &labelMix   },
    };

    for (auto& item : items)
    {
        item.s->setBounds (knobRow .removeFromLeft (knobW).reduced (4));
        item.l->setBounds (labelRow.removeFromLeft (knobW));
    }
}
