#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
static juce::String pID_Delay  = "delay";
static juce::String pID_RTMid  = "rtmid";
static juce::String pID_RTLow  = "rtlow";
static juce::String pID_Damp   = "damp";
static juce::String pID_Mix    = "mix";

//==============================================================================
ZitaRev1Processor::ZitaRev1Processor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    apvts.addParameterListener (pID_Delay, this);
    apvts.addParameterListener (pID_RTMid, this);
    apvts.addParameterListener (pID_RTLow, this);
    apvts.addParameterListener (pID_Damp,  this);
    apvts.addParameterListener (pID_Mix,   this);
}

ZitaRev1Processor::~ZitaRev1Processor()
{
    apvts.removeParameterListener (pID_Delay, this);
    apvts.removeParameterListener (pID_RTMid, this);
    apvts.removeParameterListener (pID_RTLow, this);
    apvts.removeParameterListener (pID_Damp,  this);
    apvts.removeParameterListener (pID_Mix,   this);

    if (_reverbReady)
        _reverb.fini();
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
ZitaRev1Processor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { pID_Delay, 1 },
        "Delay",
        juce::NormalisableRange<float> (0.02f, 0.1f, 0.001f),
        0.04f,
        juce::AudioParameterFloatAttributes().withLabel ("s")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { pID_RTMid, 1 },
        "RT Mid",
        juce::NormalisableRange<float> (0.1f, 8.0f, 0.01f),
        2.0f,
        juce::AudioParameterFloatAttributes().withLabel ("s")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { pID_RTLow, 1 },
        "RT Low",
        juce::NormalisableRange<float> (0.1f, 8.0f, 0.01f),
        3.0f,
        juce::AudioParameterFloatAttributes().withLabel ("s")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { pID_Damp, 1 },
        "Damping",
        juce::NormalisableRange<float> (1000.0f, 20000.0f, 1.0f, 0.4f),
        6000.0f,
        juce::AudioParameterFloatAttributes().withLabel ("Hz")));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { pID_Mix, 1 },
        "Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
        0.8f));  // 0.5f は wet が小さすぎるため 0.8f に設定

    return layout;
}

//==============================================================================
void ZitaRev1Processor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    if (_reverbReady)
        _reverb.fini();

    _reverb.init (static_cast<float> (sampleRate), /*ambis=*/false);
    _reverbReady = true;

    // パラメータ初期値を明示的にセット（APVTS の現在値を反映）
    syncAllParams();

    // ★ prepare() を必ず呼ぶ（_g0/_g1 を初期化するため）
    _reverb.prepare (samplesPerBlock);

    // dry バッファのサイズを確保
    _dryBuffer.setSize (2, samplesPerBlock);
}

void ZitaRev1Processor::releaseResources()
{
    if (_reverbReady)
    {
        _reverb.fini();
        _reverbReady = false;
    }
}

//==============================================================================
void ZitaRev1Processor::processBlock (juce::AudioBuffer<float>& buffer,
                                      juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    if (!_reverbReady)
        return;

    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0 || numChannels == 0)
        return;

    // モノ入力対策：チャンネル数が 1 の場合は右に複製
    if (numChannels == 1)
        buffer.copyFrom (1, 0, buffer, 0, 0, numSamples);

    // ① dry バッファに入力をコピー（in-place 処理を避けるため）
    _dryBuffer.makeCopyOf (buffer);

    // ② 入力・出力ポインタ配列（4 要素必須）
    float* inp[4] = {
        _dryBuffer.getWritePointer (0),
        _dryBuffer.getWritePointer (1),
        nullptr,
        nullptr
    };
    float* out[4] = {
        buffer.getWritePointer (0),
        buffer.getWritePointer (1),
        nullptr,
        nullptr
    };

    // ③ パラメータ更新を反映（毎ブロック必須）
    _reverb.prepare (numSamples);

    // ④ リバーブ処理本体
    _reverb.process (numSamples, inp, out);

    // ⑤ 出力ゲイン補正（wet が元々小さいので +6 dB 補正）
    buffer.applyGain (2.0f);
}

//==============================================================================
void ZitaRev1Processor::parameterChanged (const juce::String& paramID, float newValue)
{
    if (!_reverbReady)
        return;

    if      (paramID == pID_Delay) _reverb.set_delay (newValue);
    else if (paramID == pID_RTMid) _reverb.set_rtmid (newValue);
    else if (paramID == pID_RTLow) _reverb.set_rtlow (newValue);
    else if (paramID == pID_Damp)  _reverb.set_fdamp (newValue);
    else if (paramID == pID_Mix)   _reverb.set_opmix (newValue);
}

void ZitaRev1Processor::syncAllParams()
{
    _reverb.set_delay (apvts.getRawParameterValue (pID_Delay)->load());
    _reverb.set_rtmid (apvts.getRawParameterValue (pID_RTMid)->load());
    _reverb.set_rtlow (apvts.getRawParameterValue (pID_RTLow)->load());
    _reverb.set_fdamp (apvts.getRawParameterValue (pID_Damp)->load());
    _reverb.set_opmix (apvts.getRawParameterValue (pID_Mix)->load());
    // Fixed crossover frequency between rtlow/rtmid bands
    _reverb.set_xover (200.0f);
}

//==============================================================================
juce::AudioProcessorEditor* ZitaRev1Processor::createEditor()
{
    return new ZitaRev1Editor (*this);
}

//==============================================================================
void ZitaRev1Processor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, dest);
}

void ZitaRev1Processor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ZitaRev1Processor();
}
