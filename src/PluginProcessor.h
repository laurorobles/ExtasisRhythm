#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include <vector>

class ExtasisRhythmProcessor  : public juce::AudioProcessor
{
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
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::StringArray getDrumKitNames() const;
    juce::StringArray getVariantsForChannel (int folderIndex, int ch) const;
    void loadSampleForChannel (int ch, int folderIndex, const juce::String& fileName);
    void loadSmartSampleForChannel (int i, int kit);
    void loadGlobalDrumKit (int kit);
    void randomizeKit();

    void triggerChannel (int ch, float vel);
    void killAllAudio();
    void resetAllParameters();
    void resetSequencer();
    void changePattern (int newPattern);
    int getCurrentPattern() const { return currentPattern; }

    void saveCustomPreset (const juce::File& file);
    void loadCustomPreset (const juce::File& file);

    juce::AudioProcessorValueTreeState apvts;

    std::atomic<int> channelSteps[12];
    std::atomic<float> channelVelocities[12];
    std::atomic<int> flashCounters[12];
    juce::StringArray currentSampleName;

    std::atomic<float> outputLevelL { 0.0f };
    std::atomic<float> outputLevelR { 0.0f };

    std::atomic<bool> isSyncedToHost { false };
    std::atomic<bool> hostPlaying { false };
    std::atomic<double> hostBpm { 120.0 };

    std::atomic<float> cpuLoad { 0.0f };

    juce::File samplesFolder;
    juce::Array<juce::File> drumFolders;
    
    std::atomic<double> samplePositions[12];
    std::atomic<double> samplePositionsOld[12];
    std::atomic<float>  fadeOld[12];

    double sampleFileRates[12] = { 44100.0, 44100.0, 44100.0, 44100.0, 44100.0, 44100.0, 
                                   44100.0, 44100.0, 44100.0, 44100.0, 44100.0, 44100.0 };

    std::atomic<int> seqModes[12];
    std::atomic<int> seqPingDir[12]; 
    int seqPingPos[12] = {};         

    std::atomic<int> fillSeqMode {0};
    int fillSeqPos = 0;

    int savedGlides[8][12][32];
    int savedNotes[8][12][32]; 
    std::atomic<float> channelStepSemitones[12];

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioFormatManager formatManager;

    double currentSampleRate = 44100.0;
    int currentSamplesPerBlock = 512;
    int transportSampleCounter = 0;
    int globalStepCounter = -1;
    bool isInitialized = false;
    juce::Random random;

    int currentPattern = 0;
    int savedPatterns[8][12][32];
    int savedFills[8][16];

    double internalElapsedBeats = 0.0;
    int lastHostStep = -1;
    int lastSubStep[12];
    int lastFillSubStep = -1;

    juce::AudioBuffer<float> sampleBuffers[12];
    int sampleLengths[12] = {0};
    juce::CriticalSection sampleLock;

    juce::dsp::StateVariableTPTFilter<float> kickHpfL, kickHpfR, otherHpfL, otherHpfR;
    juce::dsp::StateVariableTPTFilter<float> kickLpfL, kickLpfR, otherLpfL, otherLpfR;
    juce::dsp::StateVariableTPTFilter<float> kickRatLpfL, kickRatLpfR, otherRatLpfL, otherRatLpfR;
    
    juce::dsp::StateVariableTPTFilter<float> delayFeedbackLpfL, delayFeedbackLpfR;
    juce::dsp::StateVariableTPTFilter<float> springToneFilterL, springToneFilterR;
    juce::dsp::StateVariableTPTFilter<float> channelToneFilters[12];
    juce::dsp::StateVariableTPTFilter<float> envFilterL[12], envFilterR[12];

    std::vector<float> flangerBufferL, flangerBufferR;
    int flangerWritePos = 0;
    float flangerLfoPhase = 0.0f;

    std::vector<float> chorusBufferL, chorusBufferR;
    int chorusWritePos = 0;
    float chorusLfoPhase = 0.0f;

    std::vector<float> springDelayL[3], springDelayR[3];
    int springPos[3] = {0, 0, 0};
    juce::dsp::FirstOrderTPTFilter<float> springApL[3], springApR[3];

    std::vector<float> delayBufferL, delayBufferR;
    int delayBufferLength = 0;
    int delayWritePos = 0;
    float delayLfoPhase = 0.0f;
    float smoothedDelayTime = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtasisRhythmProcessor)
};