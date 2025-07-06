/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
INTRUSIONAudioProcessor::INTRUSIONAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, juce::Identifier("Parameters"), {
         std::make_unique<juce::AudioParameterFloat>("absoluteAmount", "ABSOLUTE Amount", 0.0f, 100.0f, 8.0f),
         std::make_unique<juce::AudioParameterFloat>("absoluteOffset", "ABSOLUTE Offset", -1.0f, 1.0f, 0.0f),
         std::make_unique<juce::AudioParameterFloat>("dryLevel", "Dry Level", 0.0f, 1.0f, 1.0f),
         std::make_unique<juce::AudioParameterFloat>("octaveLevel", "Octave Level", 0.0f, 1.0f, 1.0f),
         std::make_unique<juce::AudioParameterFloat>("ochoLPFCutoff", "Ocho LPF Cutoff", 50.0f, 8000.0f, 1000.0f)
     })
#endif
{
}

INTRUSIONAudioProcessor::~INTRUSIONAudioProcessor()
{
}

//==============================================================================
const juce::String INTRUSIONAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool INTRUSIONAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool INTRUSIONAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool INTRUSIONAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double INTRUSIONAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int INTRUSIONAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int INTRUSIONAudioProcessor::getCurrentProgram()
{
    return 0;
}

void INTRUSIONAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String INTRUSIONAudioProcessor::getProgramName (int index)
{
    return {};
}

void INTRUSIONAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void INTRUSIONAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    lastInputStates.resize(getTotalNumInputChannels(), 0.0f);
    flipFlopStates.resize(getTotalNumInputChannels(), 1.0f);
    
    ochoFilters.resize(getTotalNumInputChannels());
    for (auto& filter : ochoFilters)
    {
        filter.reset();
        filter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 1000.0f);
    }
}

void INTRUSIONAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool INTRUSIONAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// ABSOLUTE distortion shaping function
inline float applyAbsoluteToSample(float x, float amount, float dcOffset)
{
    // Clamp amount to avoid zero/negative values
    amount = juce::jlimit(0.01f, 100.0f, amount);
    float absolutedSample = std::copysignf(1.0f - std::expf(-std::abs(x) * amount), x + dcOffset);
    absolutedSample = juce::jlimit(-1.0f, 1.0f, absolutedSample);
    
    return absolutedSample;
}

inline float processOcho(float input, float& lastInput, float& flipMultiplier, float dcOffset = 0.0f)
{
    // float adjustedInput = input + dcOffset;
    float adjustedInput = input; // trying out only applying DC to ABSOLUTE, not octave

    // Flip only on positive-going zero crossings
    if (lastInput < 0.0f && adjustedInput >= 0.0f)
        flipMultiplier = -flipMultiplier;

    lastInput = adjustedInput;

    return flipMultiplier;
}

void INTRUSIONAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    
    float lpfCutoff = parameters.getRawParameterValue("ochoLPFCutoff")->load();

    // In case we have more outputs than inputs, this code clears any empty outputs
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // MAIN AUDIO PROCESSING
    float amount = parameters.getRawParameterValue("absoluteAmount")->load();
    float dcOffset = parameters.getRawParameterValue("absoluteOffset")->load();
    float dryLevel = parameters.getRawParameterValue("dryLevel")->load();
    float octaveLevel = parameters.getRawParameterValue("octaveLevel")->load();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        ochoFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), lpfCutoff);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];

            // Apply Ocho (octave down flip-flop)
            float filtered = ochoFilters[channel].processSample(inputSample); // LPF pre-Ocho
            float ochoSample = filtered * processOcho(filtered, lastInputStates[channel], flipFlopStates[channel], dcOffset);
            // Apply ABSOLUTE to the Ocho output
            float mixed = (inputSample * dryLevel) + (ochoSample * octaveLevel);
            float output = applyAbsoluteToSample(mixed, amount, dcOffset);

            channelData[sample] = output;
        }
    }
}

//==============================================================================
bool INTRUSIONAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* INTRUSIONAudioProcessor::createEditor()
{
    return new INTRUSIONAudioProcessorEditor (*this);
}

//==============================================================================
void INTRUSIONAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    parameters.state.writeToStream(stream);
}

void INTRUSIONAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ValueTree tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
    
    if (tree.isValid())
        parameters.state = tree;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new INTRUSIONAudioProcessor();
}
