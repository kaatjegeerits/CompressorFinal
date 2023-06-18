/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


struct ChainSettings
{
    float threshold{}, knee{}, ratio{}, attack{}, release{}, gain{}, mix{};
};




ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class CompressorV2AudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    CompressorV2AudioProcessor();
    ~CompressorV2AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

   
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void setKnee();
    float getRMSValueOutput(const int level);
    float getRMSValueInput(const int level);

    juce::AudioVisualiserComponent waveViewerCompressed; 
    juce::AudioVisualiserComponent waveViewerUncompressed;

private:
    float inputLevel{};
    float slope{};
    float kneeHalf{};
    float overShoot{};
    float state{};

    float rmsLevelOutputLeft, rmsLevelOutputRight;
    float rmsLevelInputLeft,rmsLevelInputRight;

    float* channelData{};
    float* dryData{};
    float* wetData{};

    juce::AudioParameterFloat* knee{nullptr};
    juce::AudioParameterFloat* threshold{nullptr};
    juce::AudioParameterFloat* ratio{nullptr};
    juce::AudioParameterFloat* attack{ nullptr };
    juce::AudioParameterFloat* release{nullptr};
    juce::AudioParameterFloat* gain{nullptr};
    juce::AudioParameterFloat* dryWetMix{nullptr};

    juce::dsp::Compressor<float> compressor;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorV2AudioProcessor)
};
