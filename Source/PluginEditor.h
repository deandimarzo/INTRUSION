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

class GraphComponent : public juce::Component, private juce::Timer
{
public:
    GraphComponent(INTRUSIONAudioProcessor& p) : processor(p)
    {
        startTimerHz(30); // Refresh 30 times per second
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::lime);
        
        auto area = getLocalBounds().toFloat();
        auto width = area.getWidth();
        auto height = area.getHeight();

        juce::Path waveform;

        float amount = processor.parameters.getRawParameterValue("absoluteAmount")->load();
        float dcOffset = processor.parameters.getRawParameterValue("absoluteOffset")->load();

        int numPoints = (int)width;

        for (int i = 0; i < numPoints; ++i)
        {
            float normX = (float)i / (numPoints - 1);
            float input = std::sin(normX * juce::MathConstants<float>::twoPi);

            float y = applyAbsoluteToSample(input, amount, dcOffset);
            float pixelY = juce::jmap(y, -1.0f, 1.0f, height, 0.0f); // flip Y

            if (i == 0)
                waveform.startNewSubPath((float)i, pixelY);
            else
                waveform.lineTo((float)i, pixelY);
        }

        g.strokePath(waveform, juce::PathStrokeType(2.0f));
    }

private:
    INTRUSIONAudioProcessor& processor;

    void timerCallback() override { repaint(); }

    float applyAbsoluteToSample(float x, float amount, float dcOffset)
    {
        amount = juce::jlimit(0.01f, 100.0f, amount);
        float shaped = std::copysignf(1.0f - std::expf(-std::abs(x) * amount), x + dcOffset);
        return juce::jlimit(-1.0f, 1.0f, shaped);
    }
};


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
    
    GraphComponent absoluteGraph;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    INTRUSIONAudioProcessor& audioProcessor;
    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (INTRUSIONAudioProcessorEditor)
};
