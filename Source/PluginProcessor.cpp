#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>

OverDrive_DeadHouseAudioProcessor::OverDrive_DeadHouseAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameters())
{
}

OverDrive_DeadHouseAudioProcessor::~OverDrive_DeadHouseAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
OverDrive_DeadHouseAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterBool>("BYPASS", "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("FORCE_MONO", "Mono Mode", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DRIVE","Drive",1.0f,20.0f,5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("TONE","Tone",200.0f,8000.0f,2000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("OUTPUT","Output",0.0f,2.0f,1.0f));

    return { params.begin(), params.end() };
}

void OverDrive_DeadHouseAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1; // Each individual filter is mono

    for (int i = 0; i < 2; ++i)
    {
        toneFilter[i].prepare(spec);
		toneFilter[i].reset(); // Clear any internal state of the filters
    }
}

void OverDrive_DeadHouseAudioProcessor::releaseResources() {}

void OverDrive_DeadHouseAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    float driveValue = apvts.getRawParameterValue("DRIVE")->load();
    float toneValue = apvts.getRawParameterValue("TONE")->load();
    float outValue = apvts.getRawParameterValue("OUTPUT")->load();
    bool forceMono = apvts.getRawParameterValue("FORCE_MONO")->load() > 0.5f;
    bool bypassed = apvts.getRawParameterValue("BYPASS")->load() > 0.5f;

	// 1. Clear unused output channels (in case of more outputs than inputs)
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
		buffer.clear(ch, 0, numSamples);

    // 2. MONO LOGIC (DAW-Safe Version)
    if (forceMono && numChannels > 1)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float monoSum = 0.0f;

            for (int ch = 0; ch < numChannels; ++ch)
                monoSum += buffer.getSample(ch, i);

            monoSum /= (float)numChannels;

            for (int ch = 0; ch < numChannels; ++ch)
                buffer.setSample(ch, i, monoSum);
        }
    }

    if (bypassed)
    {
        // Do Nothing
        return;
    }

    // 2. Filter Stability - Clamp the frequency
    float safeTone = juce::jlimit(20.0f, 20000.0f, toneValue);
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), safeTone);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        *toneFilter[ch].coefficients = *coeffs;
    }

    for (int channel = 0; channel < numChannels; ++channel)
    {        
            //*toneFilter[channel].coefficients = *coeffs;
            auto* data = buffer.getWritePointer(channel);

            for (int i = 0; i < numSamples; ++i)
            {
                // Input Gain (Drive)
                float x = data[i] * driveValue;

                // Soft Clipping (The math that prevents hard digital crackle)
                // std::tanh handles the "loud" peaks by rounding them off
                x = std::tanh(x);

                // Filter (This is where the glitching usually happens)
                x = toneFilter[channel].processSample(x);

                // Output Gain
                x *= outValue;

                // 3. HARD LIMITER (The Safety Net)
                // If the math explodes (NaN) or goes too high, this catches it
                if (std::isnan(x) || std::isinf(x)) x = 0.0f;
                data[i] = juce::jlimit(-1.0f, 1.0f, x);
            }       
    }
}


juce::AudioProcessorEditor* OverDrive_DeadHouseAudioProcessor::createEditor()
{
    return new OverDrive_DeadHouseAudioProcessorEditor(*this);
}

void OverDrive_DeadHouseAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OverDrive_DeadHouseAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr)
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OverDrive_DeadHouseAudioProcessor();
}
