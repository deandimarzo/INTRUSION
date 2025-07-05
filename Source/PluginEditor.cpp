/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
INTRUSIONAudioProcessorEditor::INTRUSIONAudioProcessorEditor (INTRUSIONAudioProcessor& p)
: AudioProcessorEditor (&p), absoluteGraph(p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 360);
    
    
    absoluteAmountSlider.setSliderStyle(juce::Slider::Rotary);
    absoluteAmountSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(absoluteAmountSlider);

    absoluteAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "absoluteAmount", absoluteAmountSlider);
    
    absoluteOffsetSlider.setSliderStyle(juce::Slider::Rotary);
    absoluteOffsetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(absoluteOffsetSlider);

    absoluteOffsetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "absoluteOffset", absoluteOffsetSlider);
    
    addAndMakeVisible(absoluteGraph);
    
    // Dry Level Slider
    dryLevelSlider.setSliderStyle(juce::Slider::Rotary);
    dryLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(dryLevelSlider);
    dryLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "dryLevel", dryLevelSlider);

    // Octave Level Slider
    octaveLevelSlider.setSliderStyle(juce::Slider::Rotary);
    octaveLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(octaveLevelSlider);
    octaveLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "octaveLevel", octaveLevelSlider);
    
    // Labels
    absoluteAmountLabel.setText("ABS Amount", juce::dontSendNotification);
    absoluteAmountLabel.attachToComponent(&absoluteAmountSlider, false);
    addAndMakeVisible(absoluteAmountLabel);

    absoluteOffsetLabel.setText("DC Offset", juce::dontSendNotification);
    absoluteOffsetLabel.attachToComponent(&absoluteOffsetSlider, false);
    addAndMakeVisible(absoluteOffsetLabel);

    dryLevelLabel.setText("Dry Level", juce::dontSendNotification);
    dryLevelLabel.attachToComponent(&dryLevelSlider, false);
    addAndMakeVisible(dryLevelLabel);

    octaveLevelLabel.setText("Octave Level", juce::dontSendNotification);
    octaveLevelLabel.attachToComponent(&octaveLevelSlider, false);
    addAndMakeVisible(octaveLevelLabel);
    
    dryLevelSlider.setSliderStyle(juce::Slider::LinearVertical);
    octaveLevelSlider.setSliderStyle(juce::Slider::LinearVertical);
    
    ochoLPFSlider.setSliderStyle(juce::Slider::Rotary);
    ochoLPFSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(ochoLPFSlider);
    ochoLPFAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "ochoLPFCutoff", ochoLPFSlider);
    ochoLPFLabel.setText("Ocho LPF", juce::dontSendNotification);
    ochoLPFLabel.attachToComponent(&ochoLPFSlider, false);
    addAndMakeVisible(ochoLPFLabel);
}

INTRUSIONAudioProcessorEditor::~INTRUSIONAudioProcessorEditor()
{
}

//==============================================================================
void INTRUSIONAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void INTRUSIONAudioProcessorEditor::resized()
{
    const int margin = 20;
    const int knobSize = 80;
    const int narrowKnobWidth = 40;

    // Graph
    absoluteGraph.setBounds(120, margin, getWidth() - 240, 100);

    // ABSOLUTE controls on right
    absoluteAmountSlider.setBounds(getWidth() - margin - knobSize, 140, knobSize, knobSize);
    absoluteOffsetSlider.setBounds(getWidth() - margin - knobSize, 240, knobSize, knobSize);

    // OCHO controls on left
    dryLevelSlider.setBounds(margin, 140, narrowKnobWidth, 160);
    octaveLevelSlider.setBounds(margin + narrowKnobWidth + 10, 140, narrowKnobWidth, 160);

    // Ocho LPF center
    ochoLPFSlider.setBounds((getWidth() / 2) - (knobSize / 2), 260, knobSize, knobSize);
}
