
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class OverDrive_DeadHouseAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OverDrive_DeadHouseAudioProcessorEditor (OverDrive_DeadHouseAudioProcessor&);
    ~OverDrive_DeadHouseAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    OverDrive_DeadHouseAudioProcessor& audioProcessor;

    juce::Font creepsterFont;

    juce::Slider driveSlider, toneSlider, outputSlider;
    juce::ToggleButton bypassButton{ "Bypass" };
    juce::ToggleButton monoButton{ "Mono Mode" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> monoAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OverDrive_DeadHouseAudioProcessorEditor)
};
