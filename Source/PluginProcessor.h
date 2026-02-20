#pragma once

#include <JuceHeader.h>
#include "zita/reverb.h"

class ZitaRev1Processor : public juce::AudioProcessor,
                          public juce::AudioProcessorValueTreeState::Listener
{
public:
    ZitaRev1Processor();
    ~ZitaRev1Processor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "ZitaRev1"; }
    bool   acceptsMidi() const override { return false; }
    bool   producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 8.0; }

    //==============================================================================
    int  getNumPrograms() override { return 1; }
    int  getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return "Default"; }
    void changeProgramName (int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& dest) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void parameterChanged (const juce::String& paramID, float newValue) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void syncAllParams();

    ::Reverb _reverb;
    bool     _reverbReady { false };

    // dry 信号保持バッファ（in-place 禁止のため入力を別バッファにコピー）
    juce::AudioBuffer<float> _dryBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZitaRev1Processor)
};
