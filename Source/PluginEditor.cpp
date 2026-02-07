
#include "PluginProcessor.h"
#include "PluginEditor.h"

OverDrive_DeadHouseAudioProcessorEditor::OverDrive_DeadHouseAudioProcessorEditor (OverDrive_DeadHouseAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 300);

    // Load Creepster from BinaryData
    auto fontData = BinaryData::CreepsterRegular_ttf; // Note: Projucer removes hyphens
    auto fontSize = BinaryData::CreepsterRegular_ttfSize;

    creepsterFont = juce::Font(juce::Typeface::createSystemTypefaceFor(fontData, fontSize));

    auto setupSlider = [](juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::Rotary);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    };

    setupSlider(driveSlider);
    setupSlider(toneSlider);
    setupSlider(outputSlider);

    addAndMakeVisible(bypassButton);
    addAndMakeVisible(monoButton);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(toneSlider);
    addAndMakeVisible(outputSlider);

    bypassButton.onClick = [this]() { repaint(); };

    // Link to APVTS
    bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "BYPASS", bypassButton);
    monoAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "FORCE_MONO", monoButton);
    driveAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"DRIVE",driveSlider);
    toneAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"TONE",toneSlider);
    outputAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"OUTPUT",outputSlider);
}

OverDrive_DeadHouseAudioProcessorEditor::~OverDrive_DeadHouseAudioProcessorEditor() {}

void OverDrive_DeadHouseAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    // --- TITLE ---
    g.setColour(juce::Colours::darkred);
    g.setFont(creepsterFont.withHeight(40.0f)); // Big font for title
    g.drawText("DeadHouse Drive", 0, 10, getWidth(), 50, juce::Justification::centred);

    // --- SUB-LABELS ---
    g.setFont(creepsterFont.withHeight(25.0f)); // Smaller for knobs

    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(70); // Space for the title
    auto labelArea = area.removeFromTop(30); // Row for knob names

    int sliderWidth = labelArea.getWidth() / 3;

    g.drawText("BLOOD", labelArea.removeFromLeft(sliderWidth), juce::Justification::centred);
    g.drawText("BONE", labelArea.removeFromLeft(sliderWidth), juce::Justification::centred);
    g.drawText("DEATH", labelArea, juce::Justification::centred);

    // --- LED ---
    bool isActive = !audioProcessor.apvts.getRawParameterValue("BYPASS")->load();
    g.setColour(isActive ? juce::Colours::red : juce::Colours::grey);
    g.fillEllipse(getWidth() / 2 - 5, 65, 10, 10);
}

void OverDrive_DeadHouseAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    // 1. Reserve 60 pixels for "DeadHouse Drive" title
    area.removeFromTop(60);

    // 2. Reserve 40 pixels for the "BLOOD/BONE/DEATH" labels
    area.removeFromTop(40);

    // 3. Reserve 40 pixels at the bottom for buttons
    auto buttonArea = area.removeFromBottom(40);
    bypassButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(5));
    monoButton.setBounds(buttonArea.reduced(5));

    // 4. Whatever is left in 'area' is for the sliders
    auto width = area.getWidth() / 3;
    driveSlider.setBounds(area.removeFromLeft(width));
    toneSlider.setBounds(area.removeFromLeft(width));
    outputSlider.setBounds(area);
}
