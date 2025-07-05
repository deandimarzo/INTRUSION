/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
INTRUSIONAudioProcessorEditor::INTRUSIONAudioProcessorEditor (INTRUSIONAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), absoluteGraph(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    
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
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    absoluteAmountSlider.setBounds(20, 20, 100, 100); // Adjust position/size
    absoluteOffsetSlider.setBounds(200, 200, 100, 100); // Adjust position/size
}
