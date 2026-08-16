#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

class ExtasisRhythmProcessor : public juce::AudioProcessor {
public:
    ExtasisRhythmProcessor();
    ~ExtasisRhythmProcessor() override;
    void prepareToPlay (double, int) override;
    void releaseResources() override;
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout&) const override;
#endif
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int) override;
    const juce::String getProgramName (int) override;
    void changeProgramName (int, const juce::String&) override;
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::StringArray getDrumKitNames() const;
    void loadSampleForChannel (int, int);
    void loadGlobalDrumKit (int);
    void triggerChannel (int, float velocity = 1.0f);
    void saveCustomPreset (const juce::File&);
    void loadCustomPreset (const juce::File&);
    void resetAllParameters();

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<int> flashCounters[10];
    std::atomic<int> channelSteps[10];
    std::atomic<float> outputLevelL { 0.0f };
    std::atomic<float> outputLevelR { 0.0f };

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSources[10];
    float sampleNormGains[10] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> masterHpf, masterLpf;
    juce::Reverb springReverb;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> springToneFilter;
    std::atomic<float> channelVelocities[10];
    std::atomic<double> samplePositions[10];
    juce::Array<juce::File> drumFolders;
    double currentSampleRate = 44100.0;
    int currentSamplesPerBlock = 512;
    std::atomic<bool> isLoadingKits { false };
    double stepPhase = 0.0, flangerLfoPhase = 0.0;
    float pcmHoldSampleL = 0.0f, pcmHoldSampleR = 0.0f;
    int pcmCounter = 0, flangerWritePos = 0;
    std::vector<float> flangerBufferL, flangerBufferR;
    float envFastL = 0.0f, envSlowL = 0.0f, envFastR = 0.0f, envSlowR = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtasisRhythmProcessor)
};
