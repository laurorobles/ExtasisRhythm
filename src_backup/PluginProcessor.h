#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

class ExtasisRhythmProcessor : public juce::AudioProcessor {
public:
    ExtasisRhythmProcessor();
    ~ExtasisRhythmProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& name) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    
    juce::StringArray getDrumKitNames() const;
    juce::StringArray getVariantsForChannel(int folderIndex, int ch) const;
    void loadSampleForChannel (int ch, int folderIndex, const juce::String& fileName);
    void loadSampleForChannel (int ch, int fileIndex);
    void loadGlobalDrumKit (int kitIndex);
    void triggerChannel (int ch, float vel);
    void resetAllParameters();
    void resetSequencer();
    void changePattern (int newPattern);

    void saveCustomPreset(const juce::File& file);
    void loadCustomPreset(const juce::File& file);

    std::atomic<int> channelSteps[12];
    std::atomic<int> flashCounters[12];
    std::atomic<float> outputLevelL{0.0f}, outputLevelR{0.0f};

    int currentPattern = 0;
    int savedPatterns[8][12][16];
    int savedFills[8][16];

    juce::File samplesFolder;
    juce::Array<juce::File> drumFolders;

    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> sampleBuffers[12];
    int sampleLengths[12];
    std::atomic<double> samplePositions[12];
    std::atomic<float> channelVelocities[12];
    juce::CriticalSection sampleLock;

    double currentSampleRate = 44100.0;
    int currentSamplesPerBlock = 512;
    double stepPhase = 0.0;

    bool subTriggered2 = false;
    bool subTriggered3 = false;
    bool subTriggered4 = false;

    juce::dsp::StateVariableTPTFilter<float> masterHpfL, masterHpfR;
    juce::dsp::StateVariableTPTFilter<float> masterLpfL, masterLpfR;
    juce::dsp::StateVariableTPTFilter<float> springToneFilterL, springToneFilterR;
    juce::dsp::StateVariableTPTFilter<float> channelToneFilters[12];

    std::vector<float> flangerBufferL, flangerBufferR;
    int flangerWritePos = 0;
    float flangerLfoPhase = 0.0f;

    // Modelo de Tanque de 3 Resortes Analógicos
    std::vector<float> springDelayL[3];
    std::vector<float> springDelayR[3];
    size_t springPos[3] = { 0, 0, 0 };
    juce::dsp::FirstOrderTPTFilter<float> springApL[3], springApR[3];

    bool isInitialized = false;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtasisRhythmProcessor)
};
