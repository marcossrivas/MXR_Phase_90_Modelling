/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PhaserAudioProcessor::PhaserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, "PARAMETERS", initGUI())
#endif
{
    initDSP();
}

PhaserAudioProcessor::~PhaserAudioProcessor()
{
}

void PhaserAudioProcessor::initDSP()
{
    for (int i = 0; i < getTotalNumInputChannels(); ++i)
    {
        ptrPhaser1[i] = std::unique_ptr<Phaser>(new Phaser());
        ptrPhaser2[i] = std::unique_ptr<Phaser>(new Phaser());
        ptrPhaser3[i] = std::unique_ptr<Phaser>(new Phaser());
        ptrPhaser4[i] = std::unique_ptr<Phaser>(new Phaser());
        ptrMix[i] = std::unique_ptr<Mix>(new Mix());
        ptrLFO[i] = std::unique_ptr<LFO>(new LFO());
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout PhaserAudioProcessor::initGUI()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("FREQ_ID", "lfo rate", 0.1f, 20.0f, 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("MIX_ID", "mix", 0.1f, 1.0f, 1.0f));

    return{ params.begin() , params.end() };
}

//==============================================================================
const juce::String PhaserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PhaserAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PhaserAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PhaserAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PhaserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PhaserAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PhaserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PhaserAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PhaserAudioProcessor::getProgramName (int index)
{
    return {};
}

void PhaserAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PhaserAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    ptrLFO[0]->initLFO(samplesPerBlock, sampleRate);
    ptrLFO[1]->initLFO(samplesPerBlock, sampleRate);
}

void PhaserAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PhaserAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PhaserAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::AudioBuffer<float> phaserBuffer(totalNumInputChannels, buffer.getNumSamples());
    phaserBuffer.clear();

    juce::AudioBuffer<float> LFObuffer(totalNumInputChannels, buffer.getNumSamples());
    LFObuffer.clear();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        ptrLFO[channel]->processLFO(LFObuffer.getWritePointer(channel), *parameters.getRawParameterValue("FREQ_ID"));

        ptrPhaser1[channel]->processPhaser(channelData, phaserBuffer.getWritePointer(channel), buffer.getNumSamples(), getSampleRate(), LFObuffer.getReadPointer(channel));
        ptrPhaser2[channel]->processPhaser(phaserBuffer.getWritePointer(channel), phaserBuffer.getWritePointer(channel), buffer.getNumSamples(), getSampleRate(), LFObuffer.getReadPointer(channel));
        ptrPhaser3[channel]->processPhaser(phaserBuffer.getWritePointer(channel), phaserBuffer.getWritePointer(channel), buffer.getNumSamples(), getSampleRate(), LFObuffer.getReadPointer(channel));
        ptrPhaser4[channel]->processPhaser(phaserBuffer.getWritePointer(channel), phaserBuffer.getWritePointer(channel), buffer.getNumSamples(), getSampleRate(), LFObuffer.getReadPointer(channel));

        ptrMix[channel]->processMix(channelData, phaserBuffer.getWritePointer(channel), channelData, buffer.getNumSamples(), *parameters.getRawParameterValue("MIX_ID"));
    }
}

//==============================================================================
bool PhaserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PhaserAudioProcessor::createEditor()
{
    return new PhaserAudioProcessorEditor (*this);
}

//==============================================================================
void PhaserAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PhaserAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PhaserAudioProcessor();
}
