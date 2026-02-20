#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class ZitaRev1Editor : public juce::AudioProcessorEditor
{
public:
    explicit ZitaRev1Editor (ZitaRev1Processor& p);
    ~ZitaRev1Editor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ZitaRev1Processor& processorRef;

    // Sliders
    juce::Slider sliderDelay, sliderRTMid, sliderRTLow, sliderDamp, sliderMix;
    juce::Label  labelDelay,  labelRTMid,  labelRTLow,  labelDamp,  labelMix;

    using APVTS = juce::AudioProcessorValueTreeState;
    APVTS::SliderAttachment attDelay, attRTMid, attRTLow, attDamp, attMix;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZitaRev1Editor)
};
