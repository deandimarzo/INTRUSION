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
        g.setColour(juce::Colours::red);

        auto area = getLocalBounds().toFloat();
        auto width = area.getWidth();
        auto height = area.getHeight();

        juce::Path waveform;

        float amount = processor.parameters.getRawParameterValue("absoluteAmount")->load();
        float dcOffset = processor.parameters.getRawParameterValue("absoluteOffset")->load();
        float dryLevel = processor.parameters.getRawParameterValue("dryLevel")->load();
        float octaveLevel = processor.parameters.getRawParameterValue("octaveLevel")->load();

        int numPoints = (int)width;
        float lastInput = 0.0f;
        float flipMultiplier = 1.0f;

        for (int i = 0; i < numPoints; ++i)
        {
            float normX = (float)i / (numPoints - 1);
            float input = std::sin(normX * juce::MathConstants<float>::twoPi * 2.0f);
            float adjustedInput = input + dcOffset;

            // Simulate positive-going zero crossing
            if (lastInput < 0.0f && adjustedInput >= 0.0f)
                flipMultiplier = -flipMultiplier;

            lastInput = adjustedInput;

            float ocho = adjustedInput * flipMultiplier;
            float mixed = (input * dryLevel) + (ocho * octaveLevel);
            float shaped = applyAbsoluteToSample(mixed, amount, dcOffset);
            float pixelY = juce::jmap(shaped, -1.0f, 1.0f, height, 0.0f);

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
    
    juce::Label absoluteAmountLabel;
    juce::Label absoluteOffsetLabel;
    juce::Label dryLevelLabel;
    juce::Label octaveLevelLabel;
    
    juce::Slider absoluteAmountSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> absoluteAmountAttachment;
    
    juce::Slider absoluteOffsetSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> absoluteOffsetAttachment;
    
    juce::Slider dryLevelSlider;
    juce::Slider octaveLevelSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> octaveLevelAttachment;
    
    juce::Slider ochoLPFSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ochoLPFAttachment;
    juce::Label ochoLPFLabel;
    
    juce::Label titleLabel;
    
    class CRTOscillationOverlay : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override
        {
            int barHeight = 1;
            int gapHeight = 3;
            int totalHeight = getHeight();
            int totalWidth = getWidth();

            g.setColour(juce::Colours::black.withAlpha(0.2f));

            for (int y = 0; y < totalHeight; y += (barHeight + gapHeight))
                g.fillRect(0, y, totalWidth, barHeight);
        }
    };

    CRTOscillationOverlay crtOverlay;
    
    GraphComponent absoluteGraph;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    INTRUSIONAudioProcessor& audioProcessor;
    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (INTRUSIONAudioProcessorEditor)
};
