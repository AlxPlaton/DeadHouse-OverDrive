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

    float driveValue = apvts.getRawParameterValue("DRIVE")->load();
    float toneValue = apvts.getRawParameterValue("TONE")->load();
    float outValue = apvts.getRawParameterValue("OUTPUT")->load();
    bool forceMono = apvts.getRawParameterValue("FORCE_MONO")->load() > 0.5f;
    bool bypassed = apvts.getRawParameterValue("BYPASS")->load() > 0.5f;

    // 2. MONO LOGIC (DAW-Safe Version)
    if (forceMono && buffer.getNumChannels() >= 2)
    {
        // We simply sum Left and Right to Mono so it works for ANY input
        // and copy that sum to both channels.
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float leftSample = buffer.getSample(0, i);
            float rightSample = buffer.getSample(1, i);

            // Sum them and divide by 2 to prevent clipping
            float monoSum = (leftSample + rightSample) * 0.5f;

            buffer.setSample(0, i, monoSum);
            buffer.setSample(1, i, monoSum);
        }
    }

    if (bypassed) return;

    // 2. Filter Stability - Clamp the frequency
    float safeTone = juce::jlimit(20.0f, 20000.0f, toneValue);
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), safeTone);

    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        if (channel < 2)
        {
            *toneFilter[channel].coefficients = *coeffs;
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
