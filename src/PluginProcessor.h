#include "SampleBuffer.h"
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

    void scanSampleFolders();
    void setSamplesFolder (const juce::File& folder);
    static juce::File getConfigFile();

    void updateLicenseStatus();
    std::atomic<bool> isLicensedCached { false };
    std::atomic<int64_t> demoSamplesElapsed { 0 };
    std::atomic<bool> demoExpired { false };

    static int getChannelForMidiNote (int noteNum);
    static int getMidiNoteForChannel (int ch);
    static juce::String getMidiNoteNameForChannel (int ch);

    int activeMidiNotes[12] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

    void triggerChannel (int ch, float vel);
    void killAllAudio();
    void resetAllParameters();
    void resetSequencer();
    void changePattern (int newPattern);
    void copyToNextPattern();
    int getCurrentPattern() const { return currentPattern; }

    void saveCustomPreset (const juce::File& file);
    void loadCustomPreset (const juce::File& file);

    juce::AudioProcessorValueTreeState apvts;

    std::atomic<int> channelSteps[12];
    std::atomic<float> channelVelocities[12];
    std::atomic<int> flashCounters[12];
    juce::StringArray currentSampleName;
    juce::StringArray channelTags[12];
    void loadTagsFromJson();
    int levenshteinDistance(const juce::String& s1, const juce::String& s2) const;
    int analyzeAudioFile(const juce::File& file);

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



    std::atomic<int> seqModes[12];
    std::atomic<int> seqPingDir[12]; 
    int seqPingPos[12] = {};         

    std::atomic<int> fillSeqMode {0};
    int fillSeqPos = 0;

    int savedGlides[8][12][32];
    int savedNotes[8][12][32]; 
    std::atomic<float> channelStepSemitones[12];

    std::atomic<double> lastFiredBeat[12];
    std::atomic<float> lastFiredSemitone[12];

private:
    uint64_t samplesProcessed = 0;

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

    juce::LinearSmoothedValue<float> volSmoother[12];
    juce::LinearSmoothedValue<float> panSmoother[12];
    juce::LinearSmoothedValue<float> pitchSmoother[12];
    juce::LinearSmoothedValue<float> cutSmoother[12];


     
    
     
    SampleBuffer::Ptr sampleBuffers[12];
    juce::SpinLock pointerLock;
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

    // Fast DSP Saturator (Padé Rational Approximation ~5x faster than std::tanh)
    static inline float fastTanh (float x) noexcept
    {
        if (x > 3.0f)  return 1.0f;
        if (x < -3.0f) return -1.0f;
        float x2 = x * x;
        return x * (27.0f + x2) / (27.0f + 9.0f * x2);
    }

    // Cached raw parameter pointers to eliminate map lookups in realtime processBlock
    struct CachedParameters {
        std::atomic<float>* stepParams[12][32] = {};
        std::atomic<float>* lengthParams[12] = {};
        std::atomic<float>* fillStepParams[16] = {};
        std::atomic<float>* fillLength = nullptr;
        std::atomic<float>* tripletFill = nullptr;
        std::atomic<float>* fillFit = nullptr;
        
        std::atomic<float>* isPlaying = nullptr;
        std::atomic<float>* bpm = nullptr;
        std::atomic<float>* globalKitChoice = nullptr;
        
        std::atomic<float>* masterHpf = nullptr;
        std::atomic<float>* masterHpfRes = nullptr;
        std::atomic<float>* masterLpf = nullptr;
        std::atomic<float>* masterLpfRes = nullptr;
        std::atomic<float>* pcmBits = nullptr;
        std::atomic<float>* pcmRate = nullptr;
        std::atomic<float>* masterAnalog = nullptr;
        std::atomic<float>* masterVinyl = nullptr;
        std::atomic<float>* pumpOn = nullptr;
        std::atomic<float>* masterAnti = nullptr;
        std::atomic<float>* masterLimiter = nullptr;
        std::atomic<float>* flangerOn = nullptr;
        std::atomic<float>* flangerRate = nullptr;
        std::atomic<float>* flangerFeedback = nullptr;
        std::atomic<float>* chorusOn = nullptr;
        std::atomic<float>* chorusRate = nullptr;
        std::atomic<float>* chorusDepth = nullptr;
        std::atomic<float>* envFilterCut = nullptr;
        std::atomic<float>* envFilterRes = nullptr;
        std::atomic<float>* pumpThr = nullptr;
        std::atomic<float>* pumpAmt = nullptr;
        std::atomic<float>* transientAttack = nullptr;
        std::atomic<float>* transientSustain = nullptr;
        std::atomic<float>* driveDist = nullptr;
        std::atomic<float>* driveFilter = nullptr;
        std::atomic<float>* driveVol = nullptr;
        std::atomic<float>* springDecay = nullptr;
        std::atomic<float>* springTone = nullptr;
        std::atomic<float>* delayTime = nullptr;
        std::atomic<float>* delaySync = nullptr;
        std::atomic<float>* delayFb = nullptr;
        std::atomic<float>* delayModRate = nullptr;
        std::atomic<float>* delayModDepth = nullptr;
        std::atomic<float>* masterVolume = nullptr;
        std::atomic<float>* masterClipper = nullptr;
        
        std::atomic<float>* chanGain[12] = {};
        std::atomic<float>* chanPan[12] = {};
        std::atomic<float>* chanPitch[12] = {};
        std::atomic<float>* chanSSend[12] = {};
        std::atomic<float>* chanDSend[12] = {};
        std::atomic<float>* chanAttack[12] = {};
        std::atomic<float>* chanDecay[12] = {};
        std::atomic<float>* chanMute[12] = {};
        std::atomic<float>* chanSolo[12] = {};
        std::atomic<float>* chanEnv[12] = {};
        std::atomic<float>* chanTriplet[12] = {};
        std::atomic<float>* chanFit[12] = {};
        std::atomic<float>* chanTone[12] = {};
        std::atomic<float>* sampleSource[12] = {};
    } cachedParams;

    void initializeParameterPointers();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtasisRhythmProcessor)
};