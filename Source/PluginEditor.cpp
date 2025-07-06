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
    setSize (400, 400);
    
    titleLabel.setText("INTRUSION", juce::dontSendNotification);
    titleLabel.setFont(getVCRFont(24.0f));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    
    cronchAmountSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    cronchAmountSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    cronchAmountSlider.setRange(0.0f, 20.0f, 0.01f);
    addAndMakeVisible(cronchAmountSlider);

    cronchAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "cronchAmount", cronchAmountSlider);

    
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
    cronchAmountLabel.setText("CRONCH", juce::dontSendNotification);
    cronchAmountLabel.attachToComponent(&cronchAmountSlider, false);
    addAndMakeVisible(cronchAmountLabel);

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
    
    absolutionToggle.setButtonText("ABSOLUTION");
    addAndMakeVisible(absolutionToggle);
    absolutionToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "absolutionOn", absolutionToggle);
    
    absolutionThresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    absolutionThresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    absolutionThresholdSlider.setRange(0.0f, 1.0f, 0.01f);
    addAndMakeVisible(absolutionThresholdSlider);

    absolutionThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "absolutionThreshold", absolutionThresholdSlider);

    absolutionThresholdLabel.setText("Gate", juce::dontSendNotification);
    absolutionThresholdLabel.attachToComponent(&absolutionThresholdSlider, false);
    absolutionThresholdLabel.setFont(getVCRFont(14.0f));
    addAndMakeVisible(absolutionThresholdLabel);
    
    auto font = getVCRFont(14.0f);

    cronchAmountLabel.setFont(font);
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

    // Graph - expand horizontally, leave space for left/right controls
    int graphLeft = margin + narrowKnobWidth * 2 + spacing * 2;
    int graphRight = getWidth() - (margin + knobSize + spacing);
    int graphWidth = graphRight - graphLeft;
    absoluteGraph.setBounds(graphLeft,
                            50,
                            graphWidth,
                            100);

    // OCHO controls on left
    dryLevelSlider.setBounds(margin, 100, narrowKnobWidth, 120);
    octaveLevelSlider.setBounds(margin + narrowKnobWidth + spacing, 100, narrowKnobWidth, 120);
    ochoLPFSlider.setBounds(margin, 260, knobSize, knobSize);

    // ABSOLUTE controls on right
    cronchAmountSlider.setBounds(getWidth() - margin - knobSize, 100, knobSize, knobSize);
    absoluteOffsetSlider.setBounds(getWidth() - margin - knobSize, 210, knobSize, knobSize);

    // ABSOLUTION controls - move to center below graph
    absolutionToggle.setBounds(getWidth() / 2 - knobSize / 2, 160, knobSize, 20);
    absolutionThresholdSlider.setBounds(getWidth() / 2 - knobSize / 2, 210, knobSize, knobSize);

    // Apply styling
    styleSliderColor(cronchAmountSlider, juce::Colours::blue);
    styleSliderColor(absoluteOffsetSlider, juce::Colours::blue);
    styleSliderColor(dryLevelSlider, juce::Colours::red);
    styleSliderColor(octaveLevelSlider, juce::Colours::red);
    styleSliderColor(ochoLPFSlider, juce::Colours::red);
    styleSliderColor(absolutionThresholdSlider, juce::Colours::yellow);

    crtOverlay.setBounds(getLocalBounds());
}
