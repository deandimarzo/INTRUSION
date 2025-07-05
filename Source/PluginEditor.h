/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class INTRUSIONAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    INTRUSIONAudioProcessorEditor (INTRUSIONAudioProcessor&);
    ~INTRUSIONAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    juce::Slider absoluteAmountSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> absoluteAmountAttachment;
    
    juce::Slider absoluteOffsetSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> absoluteOffsetAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    INTRUSIONAudioProcessor& audioProcessor;
    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (INTRUSIONAudioProcessorEditor)
};
