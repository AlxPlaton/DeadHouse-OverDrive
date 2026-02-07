
#pragma once

#include <JuceHeader.h>

class OverDrive_DeadHouseAudioProcessor  : public juce::AudioProcessor
{
public:
    OverDrive_DeadHouseAudioProcessor();
    ~OverDrive_DeadHouseAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "OverDrive_DeadHouseAudio"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }


    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    //juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
    // Two filters: [0] is Left, [1] is Right
    juce::dsp::IIR::Filter<float> toneFilter[16]; // This is correct! L + R
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OverDrive_DeadHouseAudioProcessor)
};
