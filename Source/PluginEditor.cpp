/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

static juce::Font getVCRFont(float size)
{
    static juce::Typeface::Ptr vcrTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::VCR_OSD_MONO_ttf, BinaryData::VCR_OSD_MONO_ttfSize);
    return juce::Font(vcrTypeface).withHeight(size);
}

void styleSliderColor(juce::Slider& slider, juce::Colour color)
{
    slider.setColour(juce::Slider::rotarySliderFillColourId, color);
    slider.setColour(juce::Slider::thumbColourId, color);
    slider.setColour(juce::Slider::trackColourId, color);
    slider.setColour(juce::Slider::textBoxTextColourId, color);
    auto dark = color.darker(2.5f);
    slider.setColour(juce::Slider::backgroundColourId, dark);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, dark);
}

//==============================================================================
INTRUSIONAudioProcessorEditor::INTRUSIONAudioProcessorEditor (INTRUSIONAudioProcessor& p)
: AudioProcessorEditor (&p), absoluteGraph(p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 360);
    
    titleLabel.setText("INTRUSION", juce::dontSendNotification);
    titleLabel.setFont(getVCRFont(24.0f));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    
    absoluteAmountSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    absoluteAmountSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    absoluteAmountSlider.setRange(0.0f, 20.0f, 0.01f);
    addAndMakeVisible(absoluteAmountSlider);

    absoluteAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "absoluteAmount", absoluteAmountSlider);

    
    absoluteOffsetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    absoluteOffsetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(absoluteOffsetSlider);

    absoluteOffsetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "absoluteOffset", absoluteOffsetSlider);
    
    addAndMakeVisible(absoluteGraph);
    
    // Dry Level Slider
    dryLevelSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dryLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(dryLevelSlider);
    dryLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "dryLevel", dryLevelSlider);

    // Octave Level Slider
    octaveLevelSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    octaveLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(octaveLevelSlider);
    octaveLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "octaveLevel", octaveLevelSlider);
    
    // Labels
    absoluteAmountLabel.setText("ABSOLUTION", juce::dontSendNotification);
    absoluteAmountLabel.attachToComponent(&absoluteAmountSlider, false);
    addAndMakeVisible(absoluteAmountLabel);

    absoluteOffsetLabel.setText("DC FUCK", juce::dontSendNotification);
    absoluteOffsetLabel.attachToComponent(&absoluteOffsetSlider, false);
    addAndMakeVisible(absoluteOffsetLabel);

    dryLevelLabel.setText("0", juce::dontSendNotification);
    dryLevelLabel.attachToComponent(&dryLevelSlider, false);
    addAndMakeVisible(dryLevelLabel);

    octaveLevelLabel.setText("-8", juce::dontSendNotification);
    octaveLevelLabel.attachToComponent(&octaveLevelSlider, false);
    addAndMakeVisible(octaveLevelLabel);
    
    dryLevelSlider.setSliderStyle(juce::Slider::LinearVertical);
    octaveLevelSlider.setSliderStyle(juce::Slider::LinearVertical);
    
    ochoLPFSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ochoLPFSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(ochoLPFSlider);
    ochoLPFAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "ochoLPFCutoff", ochoLPFSlider);
    ochoLPFLabel.setText("Pre-Filter", juce::dontSendNotification);
    ochoLPFLabel.attachToComponent(&ochoLPFSlider, false);
    addAndMakeVisible(ochoLPFLabel);
    
    auto font = getVCRFont(14.0f);

    absoluteAmountLabel.setFont(font);
    absoluteOffsetLabel.setFont(font);
    dryLevelLabel.setFont(font);
    octaveLevelLabel.setFont(font);
    ochoLPFLabel.setFont(font);

    addAndMakeVisible(crtOverlay);
    crtOverlay.setInterceptsMouseClicks(false, false); // Let clicks pass through
    
    
    
    
}

INTRUSIONAudioProcessorEditor::~INTRUSIONAudioProcessorEditor()
{
}

//==============================================================================
void INTRUSIONAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(0, 0, 0)); // dark background

}

void INTRUSIONAudioProcessorEditor::resized()
{
    const int margin = 20;
    const int knobSize = 80;
    const int narrowKnobWidth = 40;
    const int spacing = 10;

    // Title
    titleLabel.setBounds(0, 10, getWidth(), 30);

    // Graph
    absoluteGraph.setBounds(margin + narrowKnobWidth * 2 + spacing * 2,
                            50,
                            getWidth() - (margin + narrowKnobWidth * 2 + spacing * 2) * 2,
                            100);

    // OCHO controls on left
    dryLevelSlider.setBounds(margin, 100, narrowKnobWidth, 120);
    octaveLevelSlider.setBounds(margin + narrowKnobWidth + spacing, 100, narrowKnobWidth, 120);
    ochoLPFSlider.setBounds(margin, 260, knobSize, knobSize);

    // ABSOLUTE controls on right
    absoluteAmountSlider.setBounds(getWidth() - margin - knobSize, 100, knobSize, knobSize);

    
    absoluteOffsetSlider.setBounds(getWidth() - margin - knobSize, 200, knobSize, knobSize);

    // Apply styling
    styleSliderColor(absoluteAmountSlider, juce::Colours::yellow);
    styleSliderColor(absoluteOffsetSlider, juce::Colours::yellow);
    styleSliderColor(dryLevelSlider, juce::Colours::red);
    styleSliderColor(octaveLevelSlider, juce::Colours::red);
    styleSliderColor(ochoLPFSlider, juce::Colours::red);

    crtOverlay.setBounds(getLocalBounds());
}
