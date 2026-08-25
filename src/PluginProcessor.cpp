#include "LicenseManager.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout ExtasisRhythmProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("isPlaying", 1), "Play", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("bpm", 1), "BPM", juce::NormalisableRange<float>(40.0f, 240.0f, 0.1f), 120.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterVolume", 1), "Master", 0.0f, 2.0f, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterClipper", 1), "Clipper", 0.1f, 1.0f, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("globalKitChoice", 1), "Kit", 0, 30, 0));
    
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("masterAnalog", 1), "Analog", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("masterVinyl", 1), "Vinyl", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("pumpOn", 1), "PumpOn", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("masterAnti", 1), "AntiAlias", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("masterLimiter", 1), "Limiter", true));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterHpf", 1), "HPF", juce::NormalisableRange<float>(20.0f, 10000.0f, 1.0f, 0.3f), 20.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterHpfRes", 1), "HRes", 0.1f, 10.0f, 0.71f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterLpf", 1), "LPF", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20000.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterLpfRes", 1), "LRes", 0.1f, 10.0f, 0.71f));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pcmBits", 1), "Bits", 4.0f, 16.0f, 16.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pcmRate", 1), "Rate", 6.25f, 100.0f, 25.0f));
    
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("flangerOn", 1), "FlangOn", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("flangerRate", 1), "FRate", juce::NormalisableRange<float>(0.05f, 15.0f, 0.01f, 0.5f), 0.40f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("flangerFeedback", 1), "FFB", -0.95f, 0.95f, 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("chorusOn", 1), "ChorusOn", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("chorusRate", 1), "CRate", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f), 1.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("chorusDepth", 1), "CDepth", 0.0f, 1.0f, 0.5f));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("transientAttack", 1), "TAtt", -1.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("transientSustain", 1), "TSus", -1.0f, 1.0f, 0.0f));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("driveDist", 1), "Dist", 0.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("driveFilter", 1), "Filter", 0.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("driveVol", 1), "Vol", 0.0f, 2.0f, 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("springDecay", 1), "SDec", 0.1f, 1.0f, 0.40f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("springTone", 1), "STon", juce::NormalisableRange<float>(200.0f, 12000.0f, 1.0f, 0.3f), 4000.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pumpThr", 1), "PThr", -60.0f, 0.0f, -20.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pumpAmt", 1), "PAmt", 0.0f, 1.0f, 0.5f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("envFilterCut", 1), "EnvCut", juce::NormalisableRange<float>(50.0f, 15000.0f, 1.0f, 0.3f), 3000.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("envFilterRes", 1), "EnvRes", 0.1f, 10.0f, 0.71f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("delayTime", 1), "DTime", juce::NormalisableRange<float>(1.0f, 2000.0f, 1.0f, 0.4f), 300.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("delayFb", 1), "DFb", 0.0f, 1.2f, 0.30f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("delayModRate", 1), "DModR", juce::NormalisableRange<float>(0.05f, 10.0f, 0.01f, 0.5f), 1.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("delayModDepth", 1), "DModD", 0.0f, 1.0f, 0.2f));
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("delaySync", 1), "DelaySync", false));

    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("tripletFill", 1), "TripFill", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("fillFit", 1), "FillFit", false));
    params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("fillLength", 1), "FillLen", 1, 16, 16));

    for (int step = 0; step < 16; ++step)
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("fill_step_" + juce::String(step), 1), "Fill", 0, 2, 0));

    for (int i = 0; i < 12; ++i) {
        juce::String chStr = juce::String (i);
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("sampleSource_" + chStr, 1), "Source", 0, 30, 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("gain" + chStr, 1), "Gain", 0.0f, 1.0f, 0.85f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pan" + chStr, 1), "Pan", -1.0f, 1.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pitch" + chStr, 1), "Pitch", -12.0f, 12.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("tone" + chStr, 1), "Tone", 200.0f, 15000.0f, 7600.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("attack" + chStr, 1), "Attack", 0.0005f, 0.05f, 0.0005f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("decay" + chStr, 1), "Decay", 0.02f, 3.0f, 0.40f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("springSend" + chStr, 1), "SSend", 0.0f, 1.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("delaySend" + chStr, 1), "DSend", 0.0f, 1.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("mute" + chStr, 1), "Mute", false));
        params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("solo" + chStr, 1), "Solo", false));
        params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("envChan_" + chStr, 1), "EnvChan", false));
        params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("triplet" + chStr, 1), "Triplet", false));
        params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("fit" + chStr, 1), "Fit", false));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("length" + chStr, 1), "Length", 1, 32, 16));
        for (int step = 0; step < 32; ++step)
            params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("step_" + chStr + "_" + juce::String(step), 1), "Step", 0, 3, 0));
    }
    return { params.begin(), params.end() };
}

ExtasisRhythmProcessor::ExtasisRhythmProcessor()
    : AudioProcessor (BusesProperties()
        .withOutput ("Master", juce::AudioChannelSet::stereo(), true)
        .withOutput ("Kick", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Snare", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Closed Hat", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Open Hat", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Clap", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Rimshot", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Hi Perc", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Mid Perc", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Low Perc", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Cowbell", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Crash", juce::AudioChannelSet::stereo(), false)
        .withOutput ("Ride", juce::AudioChannelSet::stereo(), false)),
      apvts (*this, nullptr, "APVTS", createParameterLayout()) {
    
    formatManager.registerBasicFormats();
    scanSampleFolders();
    
    for (int p=0; p<8; ++p) {
        for (int i=0; i<12; ++i) {
            for (int s=0; s<32; ++s) {
                savedPatterns[p][i][s] = 0;
                savedGlides[p][i][s] = 0;
                savedNotes[p][i][s] = 0;
            }
        }
        for (int s=0; s<16; ++s) savedFills[p][s] = 0;
    }

    for (int i=0; i<12; ++i) { 
        samplePositions[i] = -1.0; 
        samplePositionsOld[i] = -1.0;
        fadeOld[i] = 0.0f;
        channelVelocities[i] = 1.0f; 
        channelSteps[i] = 0;
        channelStepSemitones[i] = 0.0f;
        lastFiredBeat[i] = -1.0;
        lastFiredSemitone[i] = 0.0f;
        currentSampleName.add("");
        lastSubStep[i] = -1;
        seqModes[i] = 0;
        seqPingDir[i] = 1;
        seqPingPos[i] = 0;
        
    }
    
    int defaultKitIdx = 0;
    for (int k = 0; k < drumFolders.size(); ++k) {
        if (drumFolders[k].getFileName().toLowerCase().contains("sequential")) {
            defaultKitIdx = k;
            break;
        }
    }
    if (auto* kitParam = apvts.getParameter("globalKitChoice")) {
        kitParam->beginChangeGesture();
        kitParam->setValueNotifyingHost(kitParam->convertTo0to1((float)defaultKitIdx));
        kitParam->endChangeGesture();
    }

    loadTagsFromJson();
    if (!drumFolders.isEmpty()) loadGlobalDrumKit(defaultKitIdx);
    initializeParameterPointers();

    isInitialized = true;
}

ExtasisRhythmProcessor::~ExtasisRhythmProcessor() {}

juce::File ExtasisRhythmProcessor::getConfigFile()
{
#if JUCE_MAC
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
        .getChildFile ("Application Support")
        .getChildFile ("ExtasisRhythm")
        .getChildFile ("settings.xml");
#else
    return juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
        .getChildFile ("ExtasisRhythm")
        .getChildFile ("settings.xml");
#endif
}

void ExtasisRhythmProcessor::scanSampleFolders()
{
    drumFolders.clear();

    // 1. Check persistent custom folder
    auto cfg = getConfigFile();
    if (cfg.existsAsFile())
    {
        if (auto xml = juce::parseXML (cfg))
        {
            auto customPath = xml->getStringAttribute ("samplesFolderPath");
            if (customPath.isNotEmpty())
            {
                juce::File customDir (customPath);
                if (customDir.isDirectory())
                    samplesFolder = customDir;
            }
        }
    }

    // 2. Default search in Documents if not custom
    if (!samplesFolder.isDirectory())
    {
        auto docs = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory);
        auto f1 = docs.getChildFile ("ExtasisRhythm_Samples");
        auto f2 = docs.getChildFile ("ExtasisRhythm_samples");
        auto f3 = docs.getChildFile ("ExtasisRhythm Samples");
        if (f1.isDirectory()) samplesFolder = f1;
        else if (f2.isDirectory()) samplesFolder = f2;
        else if (f3.isDirectory()) samplesFolder = f3;
        else samplesFolder = f1;
    }

    if (samplesFolder.isDirectory())
    {
        samplesFolder.findChildFiles (drumFolders, juce::File::findDirectories, false);
        drumFolders.sort();
    }
}

void ExtasisRhythmProcessor::setSamplesFolder (const juce::File& folder)
{
    if (folder.isDirectory())
    {
        samplesFolder = folder;
        auto cfg = getConfigFile();
        cfg.getParentDirectory().createDirectory();

        juce::XmlElement xml ("SETTINGS");
        xml.setAttribute ("samplesFolderPath", folder.getFullPathName());
        xml.writeTo (cfg);

        scanSampleFolders();
        if (!drumFolders.isEmpty())
            loadGlobalDrumKit (0);
    }
}

void ExtasisRhythmProcessor::initializeParameterPointers()
{
    cachedParams.isPlaying = apvts.getRawParameterValue("isPlaying");
    cachedParams.bpm = apvts.getRawParameterValue("bpm");
    cachedParams.globalKitChoice = apvts.getRawParameterValue("globalKitChoice");

    cachedParams.masterHpf = apvts.getRawParameterValue("masterHpf");
    cachedParams.masterHpfRes = apvts.getRawParameterValue("masterHpfRes");
    cachedParams.masterLpf = apvts.getRawParameterValue("masterLpf");
    cachedParams.masterLpfRes = apvts.getRawParameterValue("masterLpfRes");
    cachedParams.pcmBits = apvts.getRawParameterValue("pcmBits");
    cachedParams.pcmRate = apvts.getRawParameterValue("pcmRate");
    cachedParams.masterAnalog = apvts.getRawParameterValue("masterAnalog");
    cachedParams.masterVinyl = apvts.getRawParameterValue("masterVinyl");
    cachedParams.pumpOn = apvts.getRawParameterValue("pumpOn");
    cachedParams.masterAnti = apvts.getRawParameterValue("masterAnti");
    cachedParams.masterLimiter = apvts.getRawParameterValue("masterLimiter");
    cachedParams.flangerOn = apvts.getRawParameterValue("flangerOn");
    cachedParams.flangerRate = apvts.getRawParameterValue("flangerRate");
    cachedParams.flangerFeedback = apvts.getRawParameterValue("flangerFeedback");
    cachedParams.chorusOn = apvts.getRawParameterValue("chorusOn");
    cachedParams.chorusRate = apvts.getRawParameterValue("chorusRate");
    cachedParams.chorusDepth = apvts.getRawParameterValue("chorusDepth");
    cachedParams.envFilterCut = apvts.getRawParameterValue("envFilterCut");
    cachedParams.envFilterRes = apvts.getRawParameterValue("envFilterRes");
    cachedParams.pumpThr = apvts.getRawParameterValue("pumpThr");
    cachedParams.pumpAmt = apvts.getRawParameterValue("pumpAmt");
    cachedParams.transientAttack = apvts.getRawParameterValue("transientAttack");
    cachedParams.transientSustain = apvts.getRawParameterValue("transientSustain");
    cachedParams.driveDist = apvts.getRawParameterValue("driveDist");
    cachedParams.driveFilter = apvts.getRawParameterValue("driveFilter");
    cachedParams.driveVol = apvts.getRawParameterValue("driveVol");
    cachedParams.springDecay = apvts.getRawParameterValue("springDecay");
    cachedParams.springTone = apvts.getRawParameterValue("springTone");
    cachedParams.delayTime = apvts.getRawParameterValue("delayTime");
    cachedParams.delaySync = apvts.getRawParameterValue("delaySync");
    cachedParams.delayFb = apvts.getRawParameterValue("delayFb");
    cachedParams.delayModRate = apvts.getRawParameterValue("delayModRate");
    cachedParams.delayModDepth = apvts.getRawParameterValue("delayModDepth");
    cachedParams.masterVolume = apvts.getRawParameterValue("masterVolume");
    cachedParams.masterClipper = apvts.getRawParameterValue("masterClipper");

    for (int i = 0; i < 12; ++i)
    {
        juce::String ch = juce::String(i);
        cachedParams.lengthParams[i] = apvts.getRawParameterValue ("length" + ch);
        cachedParams.chanGain[i] = apvts.getRawParameterValue ("gain" + ch);
        cachedParams.chanPan[i] = apvts.getRawParameterValue ("pan" + ch);
        cachedParams.chanPitch[i] = apvts.getRawParameterValue ("pitch" + ch);
        cachedParams.chanSSend[i] = apvts.getRawParameterValue ("springSend" + ch);
        cachedParams.chanDSend[i] = apvts.getRawParameterValue ("delaySend" + ch);
        cachedParams.chanAttack[i] = apvts.getRawParameterValue ("attack" + ch);
        cachedParams.chanDecay[i] = apvts.getRawParameterValue ("decay" + ch);
        cachedParams.chanMute[i] = apvts.getRawParameterValue ("mute" + ch);
        cachedParams.chanSolo[i] = apvts.getRawParameterValue ("solo" + ch);
        cachedParams.chanEnv[i] = apvts.getRawParameterValue ("envChan_" + ch);
        cachedParams.chanTriplet[i] = apvts.getRawParameterValue ("triplet" + ch);
        cachedParams.chanFit[i] = apvts.getRawParameterValue ("fit" + ch);
        cachedParams.chanTone[i] = apvts.getRawParameterValue ("tone" + ch);
        cachedParams.sampleSource[i] = apvts.getRawParameterValue ("sampleSource_" + ch);
        
        for (int s = 0; s < 32; ++s)
        {
            cachedParams.stepParams[i][s] = apvts.getRawParameterValue ("step_" + ch + "_" + juce::String(s));
        }
    }
    for (int s = 0; s < 16; ++s)
    {
        cachedParams.fillStepParams[s] = apvts.getRawParameterValue ("fill_step_" + juce::String (s));
    }
    cachedParams.fillLength = apvts.getRawParameterValue ("fillLength");
    cachedParams.tripletFill = apvts.getRawParameterValue ("tripletFill");
    cachedParams.fillFit = apvts.getRawParameterValue ("fillFit");
}

const juce::String ExtasisRhythmProcessor::getName() const { return JucePlugin_Name; }
bool ExtasisRhythmProcessor::acceptsMidi() const { return true; }
bool ExtasisRhythmProcessor::producesMidi() const { return true; }
bool ExtasisRhythmProcessor::isMidiEffect() const { return false; }
double ExtasisRhythmProcessor::getTailLengthSeconds() const { return 0.0; }
int ExtasisRhythmProcessor::getNumPrograms() { return 1; }
int ExtasisRhythmProcessor::getCurrentProgram() { return 0; }
void ExtasisRhythmProcessor::setCurrentProgram (int) {}
const juce::String ExtasisRhythmProcessor::getProgramName (int) { return {}; }
void ExtasisRhythmProcessor::changeProgramName (int index, const juce::String& newName) {}

bool ExtasisRhythmProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const 
{
    // Synth has no inputs
    if (!layouts.getMainInputChannelSet().isDisabled())
        return false;

    // Master output must be stereo or mono
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono())
        return false;

    // All individual stem output buses (1 to 12) must be either disabled, mono, or stereo
    for (int b = 1; b < layouts.outputBuses.size(); ++b)
    {
        const auto& bus = layouts.outputBuses.getReference (b);
        if (!bus.isDisabled() 
            && bus != juce::AudioChannelSet::stereo() 
            && bus != juce::AudioChannelSet::mono())
            return false;
    }

    return true;
}

int ExtasisRhythmProcessor::getChannelForMidiNote (int noteNum)
{
    // Primary Octave: C3 (60) to B3 (71)
    // Ch 0 (Kick): C3 (60), Ch 1 (Snare): C#3 (61), Ch 2 (Closed Hat): D3 (62), etc.
    if (noteNum >= 60 && noteNum <= 71)
        return noteNum - 60;

    // Octave Fallbacks:
    // C2 (48) to B2 (59)
    if (noteNum >= 48 && noteNum <= 59)
        return noteNum - 48;

    // C1 (36) to B1 (47)
    if (noteNum >= 36 && noteNum <= 47)
        return noteNum - 36;

    // General MIDI Drum Map legacy fallback
    switch (noteNum)
    {
        case 35: return 0;  // Acoustic Bass Drum
        case 40: return 1;  // Electric Snare
        case 44: return 2;  // Pedal Hi-Hat
        case 57: return 10; // Crash 2
        case 59: return 11; // Ride 2
    }

    return -1;
}

int ExtasisRhythmProcessor::getMidiNoteForChannel (int ch)
{
    // 12 channels chromatically starting at C3 (MIDI note 60)
    if (ch >= 0 && ch < 12) return 60 + ch;
    return 60;
}

juce::String ExtasisRhythmProcessor::getMidiNoteNameForChannel (int ch)
{
    static const char* noteNames[12] = { "C3 (60)", "C#3 (61)", "D3 (62)", "D#3 (63)", 
                                         "E3 (64)", "F3 (65)", "F#3 (66)", "G3 (67)", 
                                         "G#3 (68)", "A3 (69)", "A#3 (70)", "B3 (71)" };
    if (ch >= 0 && ch < 12) return noteNames[ch];
    return "C3";
}

void ExtasisRhythmProcessor::prepareToPlay(double sr, int bs) {
      currentSamplesPerBlock = bs;
    lastHostStep = -1; internalElapsedBeats = 0.0; lastFillSubStep = -1;
    for (int i = 0; i < 12; ++i) { lastSubStep[i] = -1; fadeOld[i] = 0.0f; channelStepSemitones[i] = 0.0f; }
    
    updateLicenseStatus();
    demoSamplesElapsed.store (0);
    for (int i = 0; i < 12; ++i) {
        volSmoother[i].reset(sr, 0.01);
        panSmoother[i].reset(sr, 0.01);
        pitchSmoother[i].reset(sr, 0.01);
        cutSmoother[i].reset(sr, 0.01);
    }

    initializeParameterPointers();
    for (int i = 0; i < 12; ++i) {
        volSmoother[i].reset(sr, 0.01);
        panSmoother[i].reset(sr, 0.01);
        pitchSmoother[i].reset(sr, 0.01);
        cutSmoother[i].reset(sr, 0.01);
    }

    killAllAudio();

    juce::dsp::ProcessSpec spec { sr, (uint32_t)bs, 2 };
    
    kickHpfL.prepare(spec); kickHpfR.prepare(spec); kickHpfL.setType(juce::dsp::StateVariableTPTFilterType::highpass); kickHpfR.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    otherHpfL.prepare(spec); otherHpfR.prepare(spec); otherHpfL.setType(juce::dsp::StateVariableTPTFilterType::highpass); otherHpfR.setType(juce::dsp::StateVariableTPTFilterType::highpass);

    kickLpfL.prepare(spec); kickLpfR.prepare(spec); kickLpfL.setType(juce::dsp::StateVariableTPTFilterType::lowpass); kickLpfR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    otherLpfL.prepare(spec); otherLpfR.prepare(spec); otherLpfL.setType(juce::dsp::StateVariableTPTFilterType::lowpass); otherLpfR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    kickRatLpfL.prepare(spec); kickRatLpfR.prepare(spec); kickRatLpfL.setType(juce::dsp::StateVariableTPTFilterType::lowpass); kickRatLpfR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    otherRatLpfL.prepare(spec); otherRatLpfR.prepare(spec); otherRatLpfL.setType(juce::dsp::StateVariableTPTFilterType::lowpass); otherRatLpfR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    delayFeedbackLpfL.prepare(spec); delayFeedbackLpfR.prepare(spec); delayFeedbackLpfL.setType(juce::dsp::StateVariableTPTFilterType::lowpass); delayFeedbackLpfR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    delayFeedbackLpfL.setCutoffFrequency(8000.0f); delayFeedbackLpfR.setCutoffFrequency(8000.0f);
    springToneFilterL.prepare(spec); springToneFilterR.prepare(spec); springToneFilterL.setType(juce::dsp::StateVariableTPTFilterType::lowpass); springToneFilterR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    for (int i = 0; i < 12; ++i) {
        channelToneFilters[i].prepare(spec);
        channelToneFilters[i].setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        envFilterL[i].prepare(spec); envFilterL[i].setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        envFilterR[i].prepare(spec); envFilterR[i].setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    }

    flangerBufferL.resize((size_t)(sr * 0.1), 0.0f); flangerBufferR.resize((size_t)(sr * 0.1), 0.0f);
    chorusBufferL.resize((size_t)(sr * 0.05), 0.0f); chorusBufferR.resize((size_t)(sr * 0.05), 0.0f);
    
    springDelayL[0].resize((size_t)(sr * 0.034f), 0.0f); springDelayR[0].resize((size_t)(sr * 0.037f), 0.0f); 
    springDelayL[1].resize((size_t)(sr * 0.045f), 0.0f); springDelayR[1].resize((size_t)(sr * 0.048f), 0.0f);
    springDelayL[2].resize((size_t)(sr * 0.058f), 0.0f); springDelayR[2].resize((size_t)(sr * 0.061f), 0.0f);

    for (int i = 0; i < 3; ++i) {
        springApL[i].prepare(spec); springApR[i].prepare(spec);
        springApL[i].setType(juce::dsp::FirstOrderTPTFilterType::allpass); springApR[i].setType(juce::dsp::FirstOrderTPTFilterType::allpass);
        springApL[i].setCutoffFrequency(1000.0f + (float)i * 500.0f); springApR[i].setCutoffFrequency(1100.0f + (float)i * 500.0f);
        springPos[i] = 0;
    }

    delayBufferLength = (int)(sr * 2.0); delayBufferL.assign(delayBufferLength, 0.0f); delayBufferR.assign(delayBufferLength, 0.0f);
    delayWritePos = 0; delayLfoPhase = 0.0f; flangerWritePos = 0; flangerLfoPhase = 0.0f; chorusWritePos = 0; chorusLfoPhase = 0.0f;
    
    float initialDelMs = cachedParams.delayTime->load();
    smoothedDelayTime = (initialDelMs / 1000.0f) * (float)sr;
}

void ExtasisRhythmProcessor::releaseResources() {
    
    for (int i=0; i<12; ++i) { juce::SpinLock::ScopedLockType sl(pointerLock); sampleBuffers[i] = nullptr; }
}

bool ExtasisRhythmProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* ExtasisRhythmProcessor::createEditor() { return new ExtasisRhythmEditor(*this); }

juce::StringArray ExtasisRhythmProcessor::getDrumKitNames() const {
    juce::StringArray names; 
    for (auto& f : drumFolders) names.add(f.getFileName()); 
    return names;
}

juce::StringArray ExtasisRhythmProcessor::getVariantsForChannel(int folderIndex, int ch) const {
    juce::ignoreUnused(ch);
    juce::StringArray results;
    if (folderIndex < 0 || folderIndex >= drumFolders.size()) return results;
    juce::File kitDir = drumFolders[folderIndex]; 
    juce::Array<juce::File> allFiles;
    kitDir.findChildFiles(allFiles, juce::File::findFiles, false);
    
    for (auto& f : allFiles) {
        if (f.getFileExtension().toLowerCase() == ".wav") results.add(f.getFileName());
    }
    results.sort(true);
    return results;
}

void ExtasisRhythmProcessor::loadSampleForChannel(int ch, int folderIndex, const juce::String& fileName) {
    if (ch < 0 || ch >= 12 || drumFolders.isEmpty() || folderIndex >= drumFolders.size()) return;
    juce::File sample = drumFolders[folderIndex].getChildFile(fileName);
    if (sample.existsAsFile()) {
        auto* reader = formatManager.createReaderFor(sample);
        if (reader) {
            int numSamps = (int) reader->lengthInSamples;
            double fileSr = reader->sampleRate; // Leemos la resolución y velocidad original del sample
            juce::AudioBuffer<float> tempBuffer ((int) reader->numChannels, numSamps);
            reader->read(&tempBuffer, 0, numSamps, 0, true, true);
            delete reader;
            
            
            auto newBuf = new SampleBuffer(std::move(tempBuffer), fileSr > 0.0 ? fileSr : 44100.0);
            {
                juce::SpinLock::ScopedLockType sl(pointerLock);
                sampleBuffers[ch] = newBuf;
            }
            
            
            currentSampleName.set(ch, fileName);
        }
    }
}

void ExtasisRhythmProcessor::loadSmartSampleForChannel(int i, int kit) {
    if (kit < 0 || kit >= drumFolders.size()) return;
    juce::File kitDir = drumFolders[kit];
    juce::Array<juce::File> allFiles;
    kitDir.findChildFiles(allFiles, juce::File::findFiles, false);
    juce::StringArray allVariants = getVariantsForChannel(kit, i);
    if (allVariants.isEmpty()) return;

    juce::StringArray matchedVars;
    juce::StringArray fuzzyMatchedVars;

    for (auto& var : allVariants) {
        juce::String nameLower = var.toLowerCase();
        juce::StringArray fileTokens; fileTokens.addTokens(nameLower, "_-. ", "");
        
        bool exactMatch = false;
        bool fuzzyMatch = false;

        for (auto& tok : channelTags[i]) {
            if (fileTokens.contains(tok)) {
                exactMatch = true;
                break;
            }
            // Nivel 2: Fuzzy Matching
            for (auto& fTok : fileTokens) {
                if (std::abs(fTok.length() - tok.length()) <= 2) {
                    int dist = levenshteinDistance(fTok, tok);
                    if (dist <= 1 || (tok.length() > 4 && dist <= 2)) {
                        fuzzyMatch = true;
                    }
                }
            }
        }
        
        if (exactMatch) {
            matchedVars.add(var);
        } else if (fuzzyMatch) {
            fuzzyMatchedVars.add(var);
        }
    }

    if (!matchedVars.isEmpty()) {
        // Tier 1: Exact Match
        int rIdx = juce::Random::getSystemRandom().nextInt(matchedVars.size());
        loadSampleForChannel(i, kit, matchedVars[rIdx]);
    } else if (!fuzzyMatchedVars.isEmpty()) {
        // Tier 2: Fuzzy Match
        int rIdx = juce::Random::getSystemRandom().nextInt(fuzzyMatchedVars.size());
        loadSampleForChannel(i, kit, fuzzyMatchedVars[rIdx]);
    } else {
        // Tier 3: DSP Auto-Tagging
        bool dspFound = false;
        int maxAttempts = juce::jmin(20, allVariants.size());
        juce::Array<int> checkedIndices;
        
        for (int attempts = 0; attempts < maxAttempts; ++attempts) {
            int rIdx = juce::Random::getSystemRandom().nextInt(allVariants.size());
            if (checkedIndices.contains(rIdx)) continue;
            checkedIndices.add(rIdx);
            
            juce::File f = kitDir.getChildFile(allVariants[rIdx]);
            int guessedChan = analyzeAudioFile(f);
            
            if (guessedChan == i) {
                loadSampleForChannel(i, kit, allVariants[rIdx]);
                dspFound = true;
                break;
            }
        }
        
        if (!dspFound) {
            // FALLBACK
            int rIdx = juce::Random::getSystemRandom().nextInt(allVariants.size());
            loadSampleForChannel(i, kit, allVariants[rIdx]);
        }
    }
}

void ExtasisRhythmProcessor::loadGlobalDrumKit(int kit) {
    for (int i = 0; i < 12; ++i) loadSmartSampleForChannel(i, kit);
}

void ExtasisRhythmProcessor::randomizeKit() {
    if (drumFolders.isEmpty()) return;

    juce::StringArray kickTokens, snareTokens;
    kickTokens.addTokens("bd kik kick kck bombo sub bassdrum bassd kickdrum drum_kik", " ", "");
    snareTokens.addTokens("sd sn snare tarola caja rim snar s snaredrum drum_snr", " ", "");

    for (int i = 0; i < 12; ++i) {
        // Try up to 10 random folders to find a matching sample
        for (int attempts = 0; attempts < 10; ++attempts) {
            int randomFolderIdx = juce::Random::getSystemRandom().nextInt(drumFolders.size());
            juce::StringArray variants = getVariantsForChannel(randomFolderIdx, i);
            if (variants.isEmpty()) continue;

            juce::String chosenSample;

            if (i == 0) {
                juce::StringArray matchKicks;
                for (auto& var : variants) {
                    juce::String nameLower = var.toLowerCase();
                    juce::StringArray fileTokens; fileTokens.addTokens(nameLower, "_-. ", "");
                    bool matched = false;
                    for (auto& tok : kickTokens) { if (fileTokens.contains(tok)) { matched = true; break; } }
                    if (matched) matchKicks.add(var);
                }
                if (!matchKicks.isEmpty()) {
                    int rIdx = juce::Random::getSystemRandom().nextInt(matchKicks.size());
                    chosenSample = matchKicks[rIdx];
                } else if (attempts == 9) { // Fallback on last attempt
                    int rIdx = juce::Random::getSystemRandom().nextInt(variants.size());
                    chosenSample = variants[rIdx];
                }
            } else if (i == 1) {
                juce::StringArray matchSnares;
                for (auto& var : variants) {
                    juce::String nameLower = var.toLowerCase();
                    juce::StringArray fileTokens; fileTokens.addTokens(nameLower, "_-. ", "");
                    bool matched = false;
                    for (auto& tok : snareTokens) { if (fileTokens.contains(tok)) { matched = true; break; } }
                    if (matched) matchSnares.add(var);
                }
                if (!matchSnares.isEmpty()) {
                    int rIdx = juce::Random::getSystemRandom().nextInt(matchSnares.size());
                    chosenSample = matchSnares[rIdx];
                } else if (attempts == 9) { // Fallback on last attempt
                    int rIdx = juce::Random::getSystemRandom().nextInt(variants.size());
                    chosenSample = variants[rIdx];
                }
            } else {
                int rIdx = juce::Random::getSystemRandom().nextInt(variants.size());
                chosenSample = variants[rIdx];
            }

            if (chosenSample.isNotEmpty()) {
                loadSampleForChannel(i, randomFolderIdx, chosenSample);
                
                // Update APVTS so GUI dropdown 1 updates automatically!
                if (auto* param = apvts.getParameter("sampleSource_" + juce::String(i))) {
                    param->beginChangeGesture();
                    param->setValueNotifyingHost(param->convertTo0to1((float)randomFolderIdx));
                    param->endChangeGesture();
                }
                break; // Move to next channel
            }
        }
    }
}

void ExtasisRhythmProcessor::triggerChannel(int ch, float vel) {
    if (ch >= 0 && ch < 12 && sampleBuffers[ch] != nullptr) {
        if (samplePositions[ch].load() >= 0.0) {
            samplePositionsOld[ch] = samplePositions[ch].load();
            fadeOld[ch] = 1.0f;
        }
        samplePositions[ch] = 0.0; 
        channelVelocities[ch] = vel; 
        flashCounters[ch] = 15;
    }
}

void ExtasisRhythmProcessor::killAllAudio() {
    
    if (true) {
        for (int i = 0; i < 12; ++i) {
            samplePositions[i] = -1.0;
            samplePositionsOld[i] = -1.0;
            fadeOld[i] = 0.0f;
            channelSteps[i] = 0;
            flashCounters[i] = 0;
            channelStepSemitones[i] = 0.0f;
        }
    }
    internalElapsedBeats = 0.0;
    outputLevelL = 0.0f;
    outputLevelR = 0.0f;
}

void ExtasisRhythmProcessor::resetAllParameters() {
    auto setParam = [this](const juce::String& id, float val) {
        if (auto* p = apvts.getParameter (id)) {
            p->beginChangeGesture(); p->setValueNotifyingHost (p->convertTo0to1 (val)); p->endChangeGesture();
        }
    };
    setParam ("isPlaying", 0.0f); setParam ("bpm", 120.0f); setParam ("masterVolume", 1.0f); setParam ("masterClipper", 1.0f);
    setParam ("masterAnalog", 0.0f); setParam ("masterVinyl", 0.0f); setParam ("pumpOn", 0.0f); setParam ("masterAnti", 0.0f); setParam ("masterLimiter", 1.0f);
    setParam ("masterHpf", 20.0f); setParam ("masterHpfRes", 0.71f);
    setParam ("masterLpf", 20000.0f); setParam ("masterLpfRes", 0.71f); setParam ("pcmBits", 16.0f); setParam ("pcmRate", 25.0f);
    setParam ("flangerOn", 0.0f); setParam ("flangerRate", 0.40f); setParam ("flangerFeedback", 0.0f);
    setParam ("chorusOn", 0.0f); setParam ("chorusRate", 1.5f); setParam ("chorusDepth", 0.5f);
    setParam ("transientAttack", 0.0f); setParam ("transientSustain", 0.0f);
    setParam ("driveDist", 0.0f); setParam ("driveFilter", 0.0f); setParam ("driveVol", 1.0f);
    setParam ("springDecay", 0.40f); setParam ("springTone", 4000.0f);
    setParam ("pumpThr", -20.0f); setParam ("pumpAmt", 0.5f);
    setParam ("envFilterCut", 3000.0f); setParam ("envFilterRes", 0.71f);
    setParam ("delayTime", 300.0f); setParam ("delayFb", 0.30f); setParam ("delayModRate", 1.5f); setParam ("delayModDepth", 0.2f);
    setParam ("delaySync", 0.0f); setParam ("tripletFill", 0.0f); setParam ("fillFit", 0.0f); setParam ("fillLength", 16.0f);

    for (int i = 0; i < 12; ++i) {
        juce::String ch = juce::String(i);
        setParam ("gain" + ch, 0.85f); setParam ("pan" + ch, 0.0f); setParam ("pitch" + ch, 0.0f); setParam ("tone" + ch, 7600.0f);
        setParam ("attack" + ch, 0.0005f); setParam ("decay" + ch, 0.40f); setParam ("springSend" + ch, 0.0f); setParam ("delaySend" + ch, 0.0f);
        setParam ("mute" + ch, 0.0f); setParam ("solo" + ch, 0.0f); setParam ("envChan_" + ch, 0.0f); setParam ("triplet" + ch, 0.0f); setParam ("fit" + ch, 0.0f); setParam ("length" + ch, 16.0f);
    }
}

void ExtasisRhythmProcessor::resetSequencer() {
    for (int i = 0; i < 12; ++i) {
        channelSteps[i] = 0; lastSubStep[i] = -1; channelStepSemitones[i] = 0.0f;
        for (int step = 0; step < 32; ++step) {
            savedGlides[currentPattern][i][step] = 0;
            savedNotes[currentPattern][i][step] = 0;
            if (auto* p = apvts.getParameter ("step_" + juce::String(i) + "_" + juce::String(step))) {
                p->beginChangeGesture(); p->setValueNotifyingHost (0.0f); p->endChangeGesture();
            }
        }
    }
    lastFillSubStep = -1;
    fillSeqPos = 0;
    if (auto* flParam = apvts.getParameter ("fillLength")) {
        flParam->beginChangeGesture(); flParam->setValueNotifyingHost (flParam->convertTo0to1(16.0f)); flParam->endChangeGesture();
    }
    if (auto* ffParam = apvts.getParameter ("fillFit")) {
        ffParam->beginChangeGesture(); ffParam->setValueNotifyingHost (0.0f); ffParam->endChangeGesture();
    }
    for (int step = 0; step < 16; ++step) {
        if (auto* p = apvts.getParameter ("fill_step_" + juce::String(step))) {
            p->beginChangeGesture(); p->setValueNotifyingHost (0.0f); p->endChangeGesture();
        }
    }
}

void ExtasisRhythmProcessor::changePattern (int newPattern) {
    if (!isInitialized || newPattern < 0 || newPattern > 7) return;

    // 1. Save active pattern values from APVTS if switching patterns
    if (newPattern != currentPattern) {
        for (int i = 0; i < 12; ++i) {
            for (int s = 0; s < 32; ++s) {
                if (auto* param = cachedParams.stepParams[i][s])
                    savedPatterns[currentPattern][i][s] = (int)(param->load() + 0.5f);
            }
        }
        for (int s = 0; s < 16; ++s) {
            if (auto* param = cachedParams.fillStepParams[s])
                savedFills[currentPattern][s] = (int)(param->load() + 0.5f);
        }
        currentPattern = newPattern;
    }

    // 2. Load target pattern values into APVTS parameters
    for (int i = 0; i < 12; ++i) {
        for (int s = 0; s < 32; ++s) {
            if (auto* p = apvts.getParameter ("step_" + juce::String(i) + "_" + juce::String(s))) {
                if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(p)) {
                    rp->setValueNotifyingHost (rp->convertTo0to1((float)savedPatterns[currentPattern][i][s]));
                }
            }
        }
    }
    for (int s = 0; s < 16; ++s) {
        if (auto* p = apvts.getParameter ("fill_step_" + juce::String(s))) {
            if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(p)) {
                rp->setValueNotifyingHost (rp->convertTo0to1((float)savedFills[currentPattern][s]));
            }
        }
    }
}

void ExtasisRhythmProcessor::copyToNextPattern() {
    if (!isInitialized) return;

    // 1. Ensure current pattern steps are saved from APVTS
    for (int i = 0; i < 12; ++i) {
        for (int s = 0; s < 32; ++s) {
            if (auto* param = cachedParams.stepParams[i][s])
                savedPatterns[currentPattern][i][s] = (int)(param->load() + 0.5f);
        }
    }
    for (int s = 0; s < 16; ++s) {
        if (auto* param = cachedParams.fillStepParams[s])
            savedFills[currentPattern][s] = (int)(param->load() + 0.5f);
    }

    int nextPattern = (currentPattern + 1) % 8;

    // 2. Deep copy current pattern data to next pattern
    for (int i = 0; i < 12; ++i) {
        for (int s = 0; s < 32; ++s) {
            savedPatterns[nextPattern][i][s] = savedPatterns[currentPattern][i][s];
            savedGlides[nextPattern][i][s]   = savedGlides[currentPattern][i][s];
            savedNotes[nextPattern][i][s]    = savedNotes[currentPattern][i][s];
        }
    }
    for (int s = 0; s < 16; ++s) {
        savedFills[nextPattern][s] = savedFills[currentPattern][s];
    }

    // 3. Switch to next pattern
    changePattern (nextPattern);
}

void ExtasisRhythmProcessor::getStateInformation (juce::MemoryBlock& destData) {
    if (isInitialized && apvts.getParameter("step_0_0") != nullptr) {
        for (int i = 0; i < 12; ++i) {
            for (int s = 0; s < 32; ++s) {
                if (auto* param = cachedParams.stepParams[i][s])
                    savedPatterns[currentPattern][i][s] = (int)(param->load() + 0.5f);
            }
        }
        for (int s = 0; s < 16; ++s) {
            if (auto* param = cachedParams.fillStepParams[s])
                savedFills[currentPattern][s] = (int)(param->load() + 0.5f);
        }
    }
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    
    // Save Sequencer Patterns, Glides, Note Locks, and Playback Modes
    auto patternsXml = new juce::XmlElement("PATTERNS");
    patternsXml->setAttribute("CURRENT_PATTERN", currentPattern);
    for (int p = 0; p < 8; ++p) {
        for (int i = 0; i < 12; ++i) {
            juce::String stepData, glideData, noteData;
            for (int s = 0; s < 32; ++s) {
                stepData += juce::String(savedPatterns[p][i][s]) + ",";
                glideData += juce::String(savedGlides[p][i][s]) + ",";
                noteData += juce::String(savedNotes[p][i][s]) + ",";
            }
            patternsXml->setAttribute("P" + juce::String(p) + "_CH" + juce::String(i), stepData);
            patternsXml->setAttribute("P" + juce::String(p) + "_CH" + juce::String(i) + "_GLIDE", glideData);
            patternsXml->setAttribute("P" + juce::String(p) + "_CH" + juce::String(i) + "_NOTE", noteData);
        }
        juce::String fillData;
        for (int s = 0; s < 16; ++s) fillData += juce::String(savedFills[p][s]) + ",";
        patternsXml->setAttribute("P" + juce::String(p) + "_FILLS", fillData);
    }
    for (int i = 0; i < 12; ++i) {
        patternsXml->setAttribute("SEQ_MODE_CH" + juce::String(i), seqModes[i].load());
    }
    patternsXml->setAttribute("FILL_SEQ_MODE", fillSeqMode.load());
    xml->addChildElement(patternsXml);

    // Save Exact Sample Kit and Sample Variant for each channel
    auto samplesXml = new juce::XmlElement("SAMPLES");
    for (int i = 0; i < 12; ++i) {
        int chKit = (int)cachedParams.sampleSource[i]->load();
        samplesXml->setAttribute("CH" + juce::String(i) + "_SAMPLE", currentSampleName[i]);
        samplesXml->setAttribute("CH" + juce::String(i) + "_KIT_IDX", chKit);
        if (chKit >= 0 && chKit < drumFolders.size()) {
            samplesXml->setAttribute("CH" + juce::String(i) + "_KIT_NAME", drumFolders[chKit].getFileName());
        }
    }
    xml->addChildElement(samplesXml);

    copyXmlToBinary (*xml, destData);
}

void ExtasisRhythmProcessor::setStateInformation (const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr) {
        if (xmlState->hasTagName (apvts.state.getType())) {
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
        }

        if (auto* patternsXml = xmlState->getChildByName("PATTERNS")) {
            currentPattern = patternsXml->getIntAttribute ("CURRENT_PATTERN", 0);
            for (int p = 0; p < 8; ++p) {
                for (int i = 0; i < 12; ++i) {
                    juce::StringArray steps;
                    steps.addTokens(patternsXml->getStringAttribute("P" + juce::String(p) + "_CH" + juce::String(i)), ",", "");
                    for (int s = 0; s < 32 && s < steps.size(); ++s) savedPatterns[p][i][s] = steps[s].getIntValue();

                    juce::StringArray glides;
                    glides.addTokens(patternsXml->getStringAttribute("P" + juce::String(p) + "_CH" + juce::String(i) + "_GLIDE"), ",", "");
                    for (int s = 0; s < 32 && s < glides.size(); ++s) savedGlides[p][i][s] = glides[s].getIntValue();

                    juce::StringArray notes;
                    notes.addTokens(patternsXml->getStringAttribute("P" + juce::String(p) + "_CH" + juce::String(i) + "_NOTE"), ",", "");
                    for (int s = 0; s < 32 && s < notes.size(); ++s) savedNotes[p][i][s] = notes[s].getIntValue();
                }
                juce::StringArray fills;
                fills.addTokens(patternsXml->getStringAttribute("P" + juce::String(p) + "_FILLS"), ",", "");
                for (int s = 0; s < 16 && s < fills.size(); ++s) savedFills[p][s] = fills[s].getIntValue();
            }
            for (int i = 0; i < 12; ++i) {
                if (patternsXml->hasAttribute("SEQ_MODE_CH" + juce::String(i)))
                    seqModes[i].store (patternsXml->getIntAttribute("SEQ_MODE_CH" + juce::String(i), 0));
            }
            if (patternsXml->hasAttribute("FILL_SEQ_MODE"))
                fillSeqMode.store (patternsXml->getIntAttribute("FILL_SEQ_MODE", 0));
        }

        if (auto* samplesXml = xmlState->getChildByName("SAMPLES")) {
            for (int i = 0; i < 12; ++i) {
                juce::String sName = samplesXml->getStringAttribute("CH" + juce::String(i) + "_SAMPLE");
                if (sName.isEmpty()) sName = samplesXml->getStringAttribute("CH" + juce::String(i)); // backward compatibility

                int kitIdx = samplesXml->getIntAttribute("CH" + juce::String(i) + "_KIT_IDX", -1);
                if (kitIdx < 0) {
                    kitIdx = (int)cachedParams.sampleSource[i]->load();
                }
                
                // Match by kit folder name if available
                if (samplesXml->hasAttribute("CH" + juce::String(i) + "_KIT_NAME")) {
                    juce::String kitName = samplesXml->getStringAttribute("CH" + juce::String(i) + "_KIT_NAME");
                    for (int k = 0; k < drumFolders.size(); ++k) {
                        if (drumFolders[k].getFileName().equalsIgnoreCase (kitName)) {
                            kitIdx = k;
                            break;
                        }
                    }
                }

                if (kitIdx >= 0 && kitIdx < drumFolders.size() && sName.isNotEmpty()) {
                    loadSampleForChannel (i, kitIdx, sName);
                } else if (drumFolders.size() > 0) {
                    loadSmartSampleForChannel (i, 0);
                }
            }
        }

        // Apply current pattern parameters
        for (int i = 0; i < 12; ++i) {
            for (int s = 0; s < 32; ++s) {
                if (auto* p = apvts.getParameter ("step_" + juce::String(i) + "_" + juce::String(s))) {
                    if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(p)) {
                        rp->beginChangeGesture();
                        rp->setValueNotifyingHost (rp->convertTo0to1((float)savedPatterns[currentPattern][i][s]));
                        rp->endChangeGesture();
                    }
                }
            }
        }
        for (int s = 0; s < 16; ++s) {
            if (auto* p = apvts.getParameter ("fill_step_" + juce::String(s))) {
                if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(p)) {
                    rp->beginChangeGesture();
                    rp->setValueNotifyingHost (rp->convertTo0to1((float)savedFills[currentPattern][s]));
                    rp->endChangeGesture();
                }
            }
        }
    }
}

void ExtasisRhythmProcessor::saveCustomPreset(const juce::File& file) {
    juce::MemoryBlock mb; getStateInformation(mb); file.replaceWithData(mb.getData(), mb.getSize());
}

void ExtasisRhythmProcessor::loadCustomPreset(const juce::File& file) {
    juce::MemoryBlock mb; if (file.loadFileAsData(mb)) setStateInformation(mb.getData(), (int)mb.getSize());
}

void ExtasisRhythmProcessor::updateLicenseStatus()
{
    isLicensedCached.store (LicenseManager::isLicensed());
    if (isLicensedCached.load())
    {
        demoExpired.store (false);
    }
}

void ExtasisRhythmProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
    auto startTime = juce::Time::getHighResolutionTicks(); 

    juce::ScopedNoDenormals noDenormals; 
    
    juce::MidiBuffer incomingMidi;
    incomingMidi.addEvents (midi, 0, buffer.getNumSamples(), 0);
    midi.clear();
    SampleBuffer::Ptr localSamples[12];
    {
        juce::SpinLock::ScopedLockType sl(pointerLock);
        for (int i=0; i<12; ++i) localSamples[i] = sampleBuffers[i];
    }

    buffer.clear(); 
    if (getSampleRate() <= 0.0) return;

    if (!isLicensedCached.load()) {
        int64_t currentElapsed = demoSamplesElapsed.load();
        int64_t maxDemoSamples = (int64_t)(getSampleRate() * 600.0); // 10 minutes (600s)
        if (currentElapsed >= maxDemoSamples) {
            demoExpired.store (true);
            return;
        } else {
            demoSamplesElapsed.store (currentElapsed + buffer.getNumSamples());
        }
    }
    
    // Multi-Out Audio: Cache stem output bus buffers (Buses 1 to 12)
    juce::AudioBuffer<float> stemBuses[12];
    bool stemBusEnabled[12] = { false };
    int totalOutputBuses = getBusCount (false);
    for (int b = 0; b < 12; ++b) {
        if ((b + 1) < totalOutputBuses) {
            auto* bus = getBus (false, b + 1);
            if (bus != nullptr && bus->isEnabled()) {
                stemBuses[b] = getBusBuffer (buffer, false, b + 1);
                stemBusEnabled[b] = (stemBuses[b].getNumChannels() > 0);
            }
        }
    }
    auto masterBus = getBusBuffer (buffer, false, 0);
    
    for (int i = 0; i < 12; ++i) { int c = flashCounters[i].load(); if (c > 0) flashCounters[i] = c - 1; }
    
    for (const auto meta : incomingMidi) { 
        auto msg = meta.getMessage(); 
        int ch = getChannelForMidiNote (msg.getNoteNumber());
        if (ch >= 0 && ch < 12) {
            if (msg.isNoteOn()) {
                triggerChannel (ch, msg.getFloatVelocity());
                juce::MidiMessage outMsg = msg;
                outMsg.setChannel (ch + 1);
                midi.addEvent (outMsg, meta.samplePosition);
            } else if (msg.isNoteOff()) {
                juce::MidiMessage outMsg = msg;
                outMsg.setChannel (ch + 1);
                midi.addEvent (outMsg, meta.samplePosition);
            }
        }
    }

    bool currentHostPlaying = false; double currentHostBpm = 120.0; double ppqPosition = 0.0; bool hasHostTime = false;
    if (auto* playHead = getPlayHead()) {
        if (auto pos = playHead->getPosition()) {
            currentHostPlaying = pos->getIsPlaying();
            if (pos->getBpm().hasValue()) currentHostBpm = *pos->getBpm();
            if (pos->getPpqPosition().hasValue() && currentHostPlaying) { 
                ppqPosition = *pos->getPpqPosition(); 
                hasHostTime = true; 
            }
        }
    }
    hostPlaying = currentHostPlaying; hostBpm = currentHostBpm; isSyncedToHost = hasHostTime;

    bool playing = hasHostTime ? currentHostPlaying : (cachedParams.isPlaying->load() > 0.5f);
    double bpm = hasHostTime ? currentHostBpm : cachedParams.bpm->load();

    float hpfFreq = cachedParams.masterHpf->load(); float hpfRes = cachedParams.masterHpfRes->load();
    float lpfFreq = cachedParams.masterLpf->load(); float lpfRes = cachedParams.masterLpfRes->load();
    
    kickHpfL.setCutoffFrequency(hpfFreq); kickHpfL.setResonance(hpfRes); kickHpfR.setCutoffFrequency(hpfFreq); kickHpfR.setResonance(hpfRes);
    otherHpfL.setCutoffFrequency(hpfFreq); otherHpfL.setResonance(hpfRes); otherHpfR.setCutoffFrequency(hpfFreq); otherHpfR.setResonance(hpfRes);

    kickLpfL.setCutoffFrequency(lpfFreq); kickLpfL.setResonance(lpfRes); kickLpfR.setCutoffFrequency(lpfFreq); kickLpfR.setResonance(lpfRes);
    otherLpfL.setCutoffFrequency(lpfFreq); otherLpfL.setResonance(lpfRes); otherLpfR.setCutoffFrequency(lpfFreq); otherLpfR.setResonance(lpfRes);

    float pcmBits = cachedParams.pcmBits->load(); float pcmRateVal = cachedParams.pcmRate->load(); float pcmClockRatio = pcmRateVal / 25.0f;
    
    bool masterAnalog = cachedParams.masterAnalog->load() > 0.5f;
    bool masterVinyl = cachedParams.masterVinyl->load() > 0.5f;
    bool pumpOn = cachedParams.pumpOn->load() > 0.5f;
    bool masterAnti = cachedParams.masterAnti->load() > 0.5f;
    bool masterLimiter = cachedParams.masterLimiter->load() > 0.5f;

    bool flangerOn = cachedParams.flangerOn->load() > 0.5f; float flangerRate = cachedParams.flangerRate->load(); float flangerFb = cachedParams.flangerFeedback->load();
    bool chorusOn = cachedParams.chorusOn->load() > 0.5f; float chorusRate = cachedParams.chorusRate->load(); float chorusDepth = cachedParams.chorusDepth->load();
    
    float envCut = cachedParams.envFilterCut->load(); float envRes = cachedParams.envFilterRes->load();
    float pumpThr = cachedParams.pumpThr->load(); float pumpAmt = cachedParams.pumpAmt->load();

    float transAtt = cachedParams.transientAttack->load(); float transSus = cachedParams.transientSustain->load();
    float ratDist = cachedParams.driveDist->load(); float ratFilter = cachedParams.driveFilter->load(); float ratVol = cachedParams.driveVol->load();
    float ratCutoff = 500.0f + (1.0f - ratFilter) * 19500.0f; 
    
    kickRatLpfL.setCutoffFrequency(ratCutoff); kickRatLpfR.setCutoffFrequency(ratCutoff);
    otherRatLpfL.setCutoffFrequency(ratCutoff); otherRatLpfR.setCutoffFrequency(ratCutoff);

    float springDec = cachedParams.springDecay->load(); float springTon = cachedParams.springTone->load();
    springToneFilterL.setCutoffFrequency(springTon); springToneFilterL.setResonance(0.71f); springToneFilterR.setCutoffFrequency(springTon); springToneFilterR.setResonance(0.71f);
    
    float targetDelMs = cachedParams.delayTime->load();
    bool delaySync = cachedParams.delaySync->load() > 0.5f;
    if (delaySync && bpm > 20.0) {
        float beatMs = (60.0f / (float)bpm) * 1000.0f;
        float beatFraction = 0.5f; 
        if (targetDelMs < 150.0f) beatFraction = 0.25f; 
        else if (targetDelMs < 250.0f) beatFraction = 0.3333f; 
        else if (targetDelMs < 400.0f) beatFraction = 0.5f; 
        else if (targetDelMs < 600.0f) beatFraction = 0.75f; 
        else if (targetDelMs < 900.0f) beatFraction = 1.0f; 
        else beatFraction = 1.5f; 
        targetDelMs = beatMs * beatFraction;
    }
    float targetDelSamples = (targetDelMs / 1000.0f) * (float)getSampleRate();
    float delFb = cachedParams.delayFb->load(); 
    float delModRate = cachedParams.delayModRate->load(); 
    float delModDepth = cachedParams.delayModDepth->load();
    
    float mVol = cachedParams.masterVolume->load(); 
    float mClip = cachedParams.masterClipper->load();

    float chanVol[12], chanPan[12], chanPitch[12], chanSSend[12], chanDSend[12], chanAtt[12], chanDec[12];
    bool chanMute[12], chanSolo[12], chanEnv[12], chanTriplet[12], chanFit[12];
    bool anySolo = false;

    for (int i = 0; i < 12; ++i) {
        juce::String ch = juce::String(i);
        volSmoother[i].setTargetValue(cachedParams.chanGain[i]->load());
        panSmoother[i].setTargetValue(cachedParams.chanPan[i]->load());
        pitchSmoother[i].setTargetValue(cachedParams.chanPitch[i]->load() + channelStepSemitones[i].load());
        
        // We still keep the arrays for anything that isn't per-sample, but we will overwrite them in the sample loop
        double srRatio = localSamples[i]->sampleRate / getSampleRate();

        chanSSend[i] = cachedParams.chanSSend[i]->load(); chanDSend[i] = cachedParams.chanDSend[i]->load();
        chanAtt[i] = cachedParams.chanAttack[i]->load(); chanDec[i] = cachedParams.chanDecay[i]->load();
        chanMute[i] = cachedParams.chanMute[i]->load() > 0.5f; chanSolo[i] = cachedParams.chanSolo[i]->load() > 0.5f;
        chanEnv[i]  = cachedParams.chanEnv[i]->load() > 0.5f;
        chanTriplet[i] = cachedParams.chanTriplet[i]->load() > 0.5f;
        chanFit[i] = cachedParams.chanFit[i]->load() > 0.5f;
        if (chanSolo[i]) anySolo = true;
        channelToneFilters[i].setCutoffFrequency(cachedParams.chanTone[i]->load());
        
        envFilterL[i].setCutoffFrequency(envCut); envFilterL[i].setResonance(envRes);
        envFilterR[i].setCutoffFrequency(envCut); envFilterR[i].setResonance(envRes);
    }

    if (!playing) {
        internalElapsedBeats = 0.0;
        for (int i = 0; i < 12; ++i) { 
            lastSubStep[i] = -1; 
            channelStepSemitones[i] = 0.0f;
            if (activeMidiNotes[i] >= 0) {
                midi.addEvent (juce::MidiMessage::noteOff (i + 1, activeMidiNotes[i]), 0);
                activeMidiNotes[i] = -1;
            }
        }
        lastFillSubStep = -1;
        fillSeqPos = 0;
    }

    

    auto getSampleHermite = [&](int chIdx, double pos, int length, int audioChan) -> float {
        int idx1 = (int)pos; float frac = (float)(pos - (double)idx1);
        int idx0 = juce::jmax(0, idx1 - 1); int idx2 = juce::jmin(length - 1, idx1 + 1); int idx3 = juce::jmin(length - 1, idx1 + 2);
        float y0 = localSamples[chIdx]->buffer.getSample(audioChan, idx0); float y1 = localSamples[chIdx]->buffer.getSample(audioChan, idx1);
        float y2 = localSamples[chIdx]->buffer.getSample(audioChan, idx2); float y3 = localSamples[chIdx]->buffer.getSample(audioChan, idx3);
        float c0 = y1; float c1 = 0.5f * (y2 - y0); float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3; float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
        return c0 + frac * (c1 + frac * (c2 + frac * c3));
    };

    for (int s = 0; s < buffer.getNumSamples(); ++s) {
        smoothedDelayTime = smoothedDelayTime * 0.995f + targetDelSamples * 0.005f;

        for (int i = 0; i < 12; ++i) {
            chanVol[i] = volSmoother[i].getNextValue();
            chanPan[i] = panSmoother[i].getNextValue();
            float semitones = pitchSmoother[i].getNextValue();
            chanPitch[i] = (float)std::pow(2.0, (double)semitones / 12.0) * ((localSamples[i] && localSamples[i]->sampleRate > 0) ? (localSamples[i]->sampleRate / getSampleRate()) : 1.0);
        }

        double beatIncPerSample = (bpm / 60.0) / getSampleRate();
        if (playing && !hasHostTime) internalElapsedBeats += beatIncPerSample;
        double exactBeats = hasHostTime ? (ppqPosition + ((double)s * beatIncPerSample)) : internalElapsedBeats;

        if (playing) {
            for (int ch = 0; ch < 12; ++ch) {
                int maxLen = (int) (cachedParams.lengthParams[ch] != nullptr ? cachedParams.lengthParams[ch]->load() : 16.0f);
                int numSteps = chanFit[ch] ? juce::jlimit(1, 32, maxLen > 0 ? maxLen : 16) : (chanTriplet[ch] ? juce::jlimit(1, 24, maxLen > 0 ? maxLen : 12) : juce::jlimit(1, 32, maxLen > 0 ? maxLen : 16));

                double mult = chanFit[ch] ? ((double)numSteps / 4.0) : (chanTriplet[ch] ? 3.0 : 4.0);
                double chSubStepD = exactBeats * mult;
                int chSubStep = (int)chSubStepD;

                if (chSubStep != lastSubStep[ch]) {
                    lastSubStep[ch] = chSubStep;

                    int mappedStep = 0;
                    int mode = seqModes[ch].load();
                    if (numSteps <= 1) {
                        mappedStep = 0;
                    } else if (mode == 1) { 
                        mappedStep = (numSteps - 1) - (chSubStep % numSteps);
                        if (mappedStep < 0) mappedStep += numSteps;
                    } else if (mode == 2) { 
                        mappedStep = random.nextInt (numSteps);
                    } else if (mode == 3) { 
                        int cycleLen = (numSteps - 1) * 2;
                        if (cycleLen > 0) {
                            int phase = chSubStep % cycleLen;
                            if (phase < 0) phase += cycleLen;
                            mappedStep = (phase < numSteps) ? phase : cycleLen - phase;
                        } else mappedStep = 0;
                    } else { 
                        mappedStep = chSubStep % numSteps;
                        if (mappedStep < 0) mappedStep += numSteps;
                    }
                    channelSteps[ch] = mappedStep;

                    int pat = getCurrentPattern();
                    channelStepSemitones[ch] = (float)savedNotes[pat][ch][mappedStep];

                    auto* stepParam = cachedParams.stepParams[ch][mappedStep];
                    int stepV = (stepParam != nullptr) ? (int)(stepParam->load() + 0.5f) : 0;
                    if (stepV > 0) {
                        float stepVel = (stepV == 1 ? 0.4f : (stepV == 2 ? 0.7f : 1.0f));
                        triggerChannel (ch, stepVel);
                        lastFiredBeat[ch] = exactBeats;
                        lastFiredSemitone[ch] = channelStepSemitones[ch].load();

                        // 12-Channel MIDI Output
                        int baseNote = getMidiNoteForChannel (ch);
                        int targetNote = juce::jlimit (0, 127, baseNote + (int)channelStepSemitones[ch].load());
                        int midiChan = ch + 1; // MIDI channel 1 to 12

                        if (activeMidiNotes[ch] >= 0) {
                            midi.addEvent (juce::MidiMessage::noteOff (midiChan, activeMidiNotes[ch]), s);
                        }
                        midi.addEvent (juce::MidiMessage::noteOn (midiChan, targetNote, stepVel), s);
                        activeMidiNotes[ch] = targetNote;
                    } else {
                        if (activeMidiNotes[ch] >= 0) {
                            midi.addEvent (juce::MidiMessage::noteOff (ch + 1, activeMidiNotes[ch]), s);
                            activeMidiNotes[ch] = -1;
                        }
                    }
                }
            }

            bool fillTriplet = (cachedParams.tripletFill != nullptr && cachedParams.tripletFill->load() > 0.5f);
            bool fillFit = (cachedParams.fillFit != nullptr && cachedParams.fillFit->load() > 0.5f);
            int numFillSteps = juce::jlimit(1, 16, (int)(cachedParams.fillLength != nullptr ? cachedParams.fillLength->load() : 16.0f));
            
            double fillMult = fillFit ? ((double)numFillSteps / 4.0) : (fillTriplet ? 3.0 : 4.0);
            double fillSubStepD = exactBeats * fillMult; 
            int fillSubStep = (int)fillSubStepD;
            
            if (fillSubStep != lastFillSubStep) {
                lastFillSubStep = fillSubStep;
                int mappedFillStep = 0;
                int fMode = fillSeqMode.load();

                if (numFillSteps <= 1) {
                    mappedFillStep = 0;
                } else if (fMode == 1) { 
                    mappedFillStep = (numFillSteps - 1) - (fillSubStep % numFillSteps);
                    if (mappedFillStep < 0) mappedFillStep += numFillSteps;
                } else if (fMode == 2) { 
                    mappedFillStep = random.nextInt(numFillSteps);
                } else if (fMode == 3) { 
                    int cycleLen = (numFillSteps - 1) * 2;
                    if (cycleLen > 0) {
                        int phase = fillSubStep % cycleLen;
                        if (phase < 0) phase += cycleLen;
                        mappedFillStep = (phase < numFillSteps) ? phase : cycleLen - phase;
                    } else mappedFillStep = 0;
                } else { 
                    mappedFillStep = fillSubStep % numFillSteps;
                    if (mappedFillStep < 0) mappedFillStep += numFillSteps;
                }
                fillSeqPos = mappedFillStep;

                auto* fillParam = cachedParams.fillStepParams[mappedFillStep];
                int fillV = (fillParam != nullptr) ? (int)(fillParam->load() + 0.5f) : 0;
                if (fillV > 0) {
                    // 1. Buscar el instante más reciente en el que algún canal haya sonado
                    double maxBeat = -1.0;
                    for (int i = 0; i < 12; ++i) {
                        if (lastFiredBeat[i].load() > maxBeat) {
                            maxBeat = lastFiredBeat[i].load();
                        }
                    }

                    // 2. Repetir todos los instrumentos que compartan ese último instante
                    for (int ch = 0; ch < 12; ++ch) {
                        if (sampleBuffers[ch] != nullptr) {
                            // Tolerancia para absorber desajustes por polirritmos (0.02 beats)
                            if (maxBeat >= 0.0 && std::abs(lastFiredBeat[ch].load() - maxBeat) < 0.02) {
                                
                                channelStepSemitones[ch] = lastFiredSemitone[ch].load();
                                float fillVel = (fillV == 1) ? 0.4f : (fillV == 2 ? 0.7f : 1.0f);
                                triggerChannel(ch, fillVel);

                                // Salida MIDI de la repetición
                                int targetNote = juce::jlimit(0, 127, getMidiNoteForChannel(ch) + (int)channelStepSemitones[ch].load());
                                if (activeMidiNotes[ch] >= 0) {
                                    midi.addEvent(juce::MidiMessage::noteOff(ch + 1, activeMidiNotes[ch]), s);
                                }
                                midi.addEvent(juce::MidiMessage::noteOn(ch + 1, targetNote, fillVel), s);
                                activeMidiNotes[ch] = targetNote;
                            }
                        }
                    }
                }
            }
        }

        float kickL = 0.0f, kickR = 0.0f;
        float otherL = 0.0f, otherR = 0.0f;
        float reverbSendTotalL = 0.0f, reverbSendTotalR = 0.0f; 
        float delaySendTotalL = 0.0f, delaySendTotalR = 0.0f;

        for (int i = 0; i < 12; ++i) {
            float chOutL = 0.0f; float chOutR = 0.0f;
            float env = 1.0f;

            if (fadeOld[i].load() > 0.0f) {
                double posOld = samplePositionsOld[i].load();
                if (posOld >= 0.0 && posOld < localSamples[i]->numSamples) {
                    float sL = getSampleHermite(i, posOld, localSamples[i]->numSamples, 0);
                    float sR = localSamples[i]->numChannels > 1 ? getSampleHermite(i, posOld, localSamples[i]->numSamples, 1) : sL;
                    
                    float fVal = fadeOld[i].load();
                    chOutL += sL * fVal; chOutR += sR * fVal;
                    
                    fadeOld[i] = fVal - 0.01f; 
                    samplePositionsOld[i] = posOld + chanPitch[i];
                } else fadeOld[i] = 0.0f;
            }

            double pos = samplePositions[i].load();
            if (pos >= 0.0 && localSamples[i]->numSamples > 0) {
                if (pos >= localSamples[i]->numSamples) { samplePositions[i] = -1.0; continue; }

                float sL = getSampleHermite(i, pos, localSamples[i]->numSamples, 0);
                float sR = localSamples[i]->numChannels > 1 ? getSampleHermite(i, pos, localSamples[i]->numSamples, 1) : sL;

                if (chanMute[i] || (anySolo && !chanSolo[i])) {
                    double nextPos = pos + chanPitch[i]; if (nextPos >= localSamples[i]->numSamples) samplePositions[i] = -1.0; else samplePositions[i] = nextPos;
                    continue;
                }

                float timeInSec = (float)pos / (float)getSampleRate();
                if (timeInSec < chanAtt[i]) env = timeInSec / juce::jmax(0.0001f, chanAtt[i]);
                else env = std::exp (-(timeInSec - chanAtt[i]) / juce::jmax(0.01f, chanDec[i]));

                float antiClickFade = juce::jmin(1.0f, (float)pos / (0.003f * (float)getSampleRate()));

                float attackEnv = std::exp (-timeInSec / 0.015f); float sustainEnv = 1.0f - attackEnv;
                float transMod = 1.0f + (transAtt * attackEnv * 1.3f) + (transSus * sustainEnv * 0.7f);

                chOutL += (sL * env * transMod * antiClickFade); chOutR += (sR * env * transMod * antiClickFade);
                
                double nextPos = pos + chanPitch[i]; if (nextPos >= localSamples[i]->numSamples) samplePositions[i] = -1.0; else samplePositions[i] = nextPos;
            }

            if (chOutL != 0.0f || chOutR != 0.0f) {
                chOutL = channelToneFilters[i].processSample(0, chOutL); 
                chOutR = channelToneFilters[i].processSample(0, chOutR);

                if (chanEnv[i]) {
                    float dynCutoff = juce::jlimit(20.0f, 20000.0f, envCut * (0.2f + 0.8f * env));
                    envFilterL[i].setCutoffFrequency(dynCutoff);
                    envFilterR[i].setCutoffFrequency(dynCutoff);
                    chOutL = envFilterL[i].processSample(0, chOutL);
                    chOutR = envFilterR[i].processSample(0, chOutR);
                }

                float finalGain = 0.35f * chanVol[i] * channelVelocities[i].load();
                float pannedL = chOutL * std::sqrt (0.5f * (1.0f - chanPan[i])) * finalGain; 
                float pannedR = chOutR * std::sqrt (0.5f * (1.0f + chanPan[i])) * finalGain;
                pannedL = fastTanh (pannedL * 1.5f); pannedR = fastTanh (pannedR * 1.5f);

                // Multi-Out Stem bus routing (Bypasses Master FX when routed separately in DAW)
                if (stemBusEnabled[i]) {
                    int numStemChans = stemBuses[i].getNumChannels();
                    if (numStemChans >= 2) {
                        stemBuses[i].setSample (0, s, pannedL);
                        stemBuses[i].setSample (1, s, pannedR);
                    } else if (numStemChans == 1) {
                        stemBuses[i].setSample (0, s, 0.5f * (pannedL + pannedR));
                    }
                }

                if (i == 0) {
                    kickL += pannedL; kickR += pannedR;
                } else {
                    otherL += pannedL; otherR += pannedR;
                }

                reverbSendTotalL += pannedL * chanSSend[i]; reverbSendTotalR += pannedR * chanSSend[i]; 
                delaySendTotalL += pannedL * chanDSend[i]; delaySendTotalR += pannedR * chanDSend[i];
            }
        }

        auto processMasterBusChain = [&](float& l, float& r, bool isKick) {
            if (pcmBits < 16.0f) { 
                float steps = std::pow(2.0f, pcmBits); 
                l = std::round(l * steps) / steps; 
                r = std::round(r * steps) / steps; 
            }
            if (isKick) {
                l = kickHpfL.processSample(0, l); r = kickHpfR.processSample(0, r);
                l = kickLpfL.processSample(0, l); r = kickLpfR.processSample(0, r);
                float ratGainFactor = 1.0f + (std::pow(ratDist, 2.0f) * 100.0f);
                l = fastTanh (l * ratGainFactor); r = fastTanh (r * ratGainFactor);
                l = kickRatLpfL.processSample(0, l) * ratVol; r = kickRatLpfR.processSample(0, r) * ratVol;
            } else {
                l = otherHpfL.processSample(0, l); r = otherHpfR.processSample(0, r);
                l = otherLpfL.processSample(0, l); r = otherLpfR.processSample(0, r);
                float ratGainFactor = 1.0f + (std::pow(ratDist, 2.0f) * 100.0f);
                l = fastTanh (l * ratGainFactor); r = fastTanh (r * ratGainFactor);
                l = otherRatLpfL.processSample(0, l) * ratVol; r = otherRatLpfR.processSample(0, r) * ratVol;
            }
        };

        processMasterBusChain(kickL, kickR, true);
        processMasterBusChain(otherL, otherR, false);

        if (flangerOn && !flangerBufferL.empty()) {
            flangerLfoPhase += (flangerRate / (float)getSampleRate()); if (flangerLfoPhase >= 1.0f) flangerLfoPhase -= 1.0f;
            float lfo = std::sin(2.0f * (float)juce::MathConstants<double>::pi * flangerLfoPhase);
            int delaySamples = (int)((0.005f + 0.004f * (lfo + 1.0f)) * (float)getSampleRate());
            
            int readPosL = (flangerWritePos - delaySamples + (int)flangerBufferL.size()) % (int)flangerBufferL.size();
            float delayedL = flangerBufferL[(size_t)readPosL]; flangerBufferL[(size_t)flangerWritePos] = otherL + delayedL * flangerFb;

            int readPosR = (flangerWritePos - delaySamples + (int)flangerBufferR.size()) % (int)flangerBufferR.size();
            float delayedR = flangerBufferR[(size_t)readPosR]; flangerBufferR[(size_t)flangerWritePos] = otherR + delayedR * flangerFb;

            flangerWritePos = (flangerWritePos + 1) % (int)flangerBufferL.size();
            otherL = 0.5f * (otherL + delayedL); otherR = 0.5f * (otherR + delayedR);
        }

        if (chorusOn && !chorusBufferL.empty()) {
            chorusLfoPhase += (chorusRate / (float)getSampleRate()); 
            if (chorusLfoPhase >= 1.0f) chorusLfoPhase -= 1.0f;
            
            float lfoL = std::sin(2.0f * (float)juce::MathConstants<double>::pi * chorusLfoPhase);
            float lfoR = std::cos(2.0f * (float)juce::MathConstants<double>::pi * chorusLfoPhase);

            float maxModMs = 0.001f + chorusDepth * 0.007f; 
            float baseDelMs = 0.012f; 
            
            float delSamplesL = (baseDelMs + maxModMs * lfoL) * (float)getSampleRate();
            float delSamplesR = (baseDelMs + maxModMs * lfoR) * (float)getSampleRate();

            int cBufSize = (int)chorusBufferL.size();
            auto readChorusInterp = [](const std::vector<float>& buf, float exactPos, int size) -> float {
                while (exactPos < 0.0f) exactPos += (float)size;
                while (exactPos >= (float)size) exactPos -= (float)size;
                int i1 = (int)exactPos;
                int i2 = (i1 + 1) % size;
                float frac = exactPos - (float)i1;
                return buf[(size_t)i1] + frac * (buf[(size_t)i2] - buf[(size_t)i1]);
            };

            float cDelayedL = readChorusInterp(chorusBufferL, (float)chorusWritePos - delSamplesL, cBufSize);
            float cDelayedR = readChorusInterp(chorusBufferR, (float)chorusWritePos - delSamplesR, cBufSize);

            chorusBufferL[(size_t)chorusWritePos] = otherL;
            chorusBufferR[(size_t)chorusWritePos] = otherR;
            chorusWritePos = (chorusWritePos + 1) % cBufSize;

            otherL = juce::jlimit(-1.0f, 1.0f, 0.707f * (otherL + cDelayedL));
            otherR = juce::jlimit(-1.0f, 1.0f, 0.707f * (otherR + cDelayedR));
        }

        float mixedL = kickL + otherL;
        float mixedR = kickR + otherR;

        if (!springDelayL[0].empty()) {
            float feedback = 0.2f + springDec * 0.75f;
            float driveL = fastTanh (reverbSendTotalL * 1.5f); float driveR = fastTanh (reverbSendTotalR * 1.5f);
            float tankInL = springToneFilterL.processSample(0, driveL); float tankInR = springToneFilterR.processSample(0, driveR);
            float springOutL = 0.0f; float springOutR = 0.0f;

            for (int j = 0; j < 3; ++j) {
                float dL = springDelayL[j][springPos[j]]; float dR = springDelayR[j][springPos[j]];
                float apL = springApL[j].processSample(0, tankInL + dL * feedback); float apR = springApR[j].processSample(0, tankInR + dR * feedback);
                springDelayL[j][springPos[j]] = fastTanh (apL); springDelayR[j][springPos[j]] = fastTanh (apR); springOutL += dL; springOutR += dR;
                springPos[j] = (springPos[j] + 1) % springDelayL[j].size();
            }
            mixedL += springOutL * 0.5f; mixedR += springOutR * 0.5f;
        }

        if (delayBufferLength > 0) {
            delayLfoPhase += (delModRate / (float)getSampleRate()); 
            if (delayLfoPhase >= 1.0f) delayLfoPhase -= 1.0f;
            float lfoVal = std::sin(2.0f * (float)juce::MathConstants<double>::pi * delayLfoPhase);
            float effectiveDelSamples = juce::jlimit(1.0f, (float)(delayBufferLength - 4), smoothedDelayTime + (lfoVal * delModDepth * 50.0f));
            
            int writeP = delayWritePos; 
            float readPosExact = (float)writeP - effectiveDelSamples; 
            while (readPosExact < 0.0f) readPosExact += (float)delayBufferLength;
            while (readPosExact >= (float)delayBufferLength) readPosExact -= (float)delayBufferLength;
            
            int idx1 = (int)readPosExact; 
            int idx2 = (idx1 + 1) % delayBufferLength; 
            float frac = readPosExact - (float)idx1;

            float dOutL = delayBufferL[(size_t)idx1] + frac * (delayBufferL[(size_t)idx2] - delayBufferL[(size_t)idx1]); 
            float dOutR = delayBufferR[(size_t)idx1] + frac * (delayBufferR[(size_t)idx2] - delayBufferR[(size_t)idx1]);
            
            dOutL = delayFeedbackLpfL.processSample(0, dOutL); 
            dOutR = delayFeedbackLpfR.processSample(0, dOutR);
            
            float fbL = fastTanh (dOutL * delFb);
            float fbR = fastTanh (dOutR * delFb);
            
            delayBufferL[(size_t)writeP] = delaySendTotalL + fbL; 
            delayBufferR[(size_t)writeP] = delaySendTotalR + fbR;
            
            delayWritePos = (writeP + 1) % delayBufferLength;
            mixedL += dOutL; mixedR += dOutR;
        }

        mixedL *= (mVol * 1.4125f); mixedR *= (mVol * 1.4125f);
        mixedL = fastTanh (mixedL / mClip) * mClip; mixedR = fastTanh (mixedR / mClip) * mClip;

        if (pumpOn) {
            float envFollower = juce::jmax(std::abs(mixedL), std::abs(mixedR));
            float thresholdLinear = juce::Decibels::decibelsToGain(pumpThr);
            if (envFollower > thresholdLinear) {
                float reduction = 1.0f - (pumpAmt * (1.0f - (thresholdLinear / juce::jmax(0.0001f, envFollower))));
                mixedL *= reduction; mixedR *= reduction;
            }
        }

        if (masterAnalog) {
            mixedL = fastTanh (mixedL * 1.25f) * 0.9f;
            mixedR = fastTanh (mixedR * 1.25f) * 0.9f;
        }
        if (masterVinyl) {
            mixedL *= 0.95f; mixedR *= 0.95f;
            mixedL = otherHpfL.processSample(0, mixedL);
            mixedR = otherHpfR.processSample(0, mixedR);
        }

        if (masterAnti) {
            mixedL = fastTanh (mixedL * 0.98f);
            mixedR = fastTanh (mixedR * 0.98f);
        }

        if (masterLimiter) {
            const float ceiling = 0.9772f; 
            if (std::abs(mixedL) > ceiling) mixedL = (mixedL > 0.0f ? ceiling : -ceiling);
            if (std::abs(mixedR) > ceiling) mixedR = (mixedR > 0.0f ? ceiling : -ceiling);
        }

        masterBus.setSample (0, s, mixedL); 
        if (masterBus.getNumChannels() > 1) 
            masterBus.setSample (1, s, mixedR);
    }

    float curRmsL = buffer.getRMSLevel (0, 0, buffer.getNumSamples()); 
    float curRmsR = buffer.getNumChannels() > 1 ? buffer.getRMSLevel (1, 0, buffer.getNumSamples()) : curRmsL;
    outputLevelL = juce::jmax (curRmsL, outputLevelL.load() * 0.8f); 
    outputLevelR = juce::jmax (curRmsR, outputLevelR.load() * 0.8f);

    auto endTime = juce::Time::getHighResolutionTicks();
    double elapsedSeconds = juce::Time::highResolutionTicksToSeconds(endTime - startTime);
    double blockDuration = (double)buffer.getNumSamples() / getSampleRate();
    if (blockDuration > 0.0) {
        float instantLoad = (float)(elapsedSeconds / blockDuration) * 100.0f;
        float prev = cpuLoad.load();
        cpuLoad.store(prev * 0.85f + instantLoad * 0.15f);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ExtasisRhythmProcessor();
}

void ExtasisRhythmProcessor::loadTagsFromJson() {
    juce::File docsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    juce::File sampleDir = docsDir.getChildFile("ExtasisRhythm_Samples");
    if (!sampleDir.exists()) sampleDir.createDirectory();

    juce::File tagsFile = sampleDir.getChildFile("tags.json");
    if (!tagsFile.existsAsFile()) {
        // Create default JSON
        juce::DynamicObject::Ptr root = new juce::DynamicObject();
        root->setProperty("0_Kick", "bd kik kick kck bombo sub bassdrum bassd kickdrum drum_kik");
        root->setProperty("1_Snare", "sd sn snare tarola caja rim snar s snaredrum drum_snr");
        root->setProperty("2_ClosedHat", "ch hh closed hat hihat clh hat_c hh_c cht closedhat closed_hh");
        root->setProperty("3_OpenHat", "oh open oph ohat hat_o hh_o oht openhat open_hh splash hho");
        root->setProperty("4_Clap", "cp clap clp palmas handclap groupclap cla");
        root->setProperty("5_Rimshot", "rs rim rimshot side sidestick woodblock wblk wood clave claves");
        root->setProperty("6_HiPerc", "ht hightom tomhi tom1 h-tom tom_hi conga_hi conga_high bongo_hi bnh");
        root->setProperty("7_MidPerc", "mt midtom tommid tom2 m-tom tom_mid conga_mid conga bongo_lo bol");
        root->setProperty("8_LowPerc", "lt lowtom tomlow tom3 floor ftom l-tom tom_lo timbale tmb conga_lo conga_low");
        root->setProperty("9_Cowbell", "cb cowbell bell cwb cow quijada vibraslap agogo ago shaker shk tambourine tamb maraca mrc");
        root->setProperty("10_Crash", "cr crash cym cymbal crs china");
        root->setProperty("11_Ride", "rd ride rid ride_bell ride_bow cym_ride");
        
        juce::var jsonVar(root.get());
        juce::String jsonString = juce::JSON::toString(jsonVar);
        tagsFile.replaceWithText(jsonString);
    }

    juce::var parsed = juce::JSON::parse(tagsFile);
    if (auto* obj = parsed.getDynamicObject()) {
        juce::StringArray keys = {"0_Kick", "1_Snare", "2_ClosedHat", "3_OpenHat", "4_Clap", "5_Rimshot", "6_HiPerc", "7_MidPerc", "8_LowPerc", "9_Cowbell", "10_Crash", "11_Ride"};
        for (int i = 0; i < 12; ++i) {
            juce::String tokens = obj->getProperty(keys[i]).toString();
            channelTags[i].addTokens(tokens, " ,", "");
        }
    } else {
        // Fallback if parsing fails
        channelTags[0].addTokens("bd kik kick kck bombo sub bassdrum bassd kickdrum drum_kik", " ", "");
        channelTags[1].addTokens("sd sn snare tarola caja rim snar s snaredrum drum_snr", " ", "");
        channelTags[2].addTokens("ch hh closed hat hihat clh hat_c hh_c cht closedhat closed_hh", " ", "");
        channelTags[3].addTokens("oh open oph ohat hat_o hh_o oht openhat open_hh splash hho", " ", "");
        channelTags[4].addTokens("cp clap clp palmas handclap groupclap cla", " ", "");
        channelTags[5].addTokens("rs rim rimshot side sidestick woodblock wblk wood clave claves", " ", "");
        channelTags[6].addTokens("ht hightom tomhi tom1 h-tom tom_hi conga_hi conga_high bongo_hi bnh", " ", "");
        channelTags[7].addTokens("mt midtom tommid tom2 m-tom tom_mid conga_mid conga bongo_lo bol", " ", "");
        channelTags[8].addTokens("lt lowtom tomlow tom3 floor ftom l-tom tom_lo timbale tmb conga_lo conga_low", " ", "");
        channelTags[9].addTokens("cb cowbell bell cwb cow quijada vibraslap agogo ago shaker shk tambourine tamb maraca mrc", " ", "");
        channelTags[10].addTokens("cr crash cym cymbal crs china", " ", "");
        channelTags[11].addTokens("rd ride rid ride_bell ride_bow cym_ride", " ", "");
    }
}

int ExtasisRhythmProcessor::levenshteinDistance(const juce::String& s1, const juce::String& s2) const {
    int m = s1.length(); int n = s2.length();
    if (m == 0) return n; if (n == 0) return m;
    std::vector<int> costs(n + 1);
    for (int k = 0; k <= n; k++) costs[k] = k;
    for (int i = 0; i < m; i++) {
        int cost = i; int pC = i;
        for (int j = 0; j < n; j++) {
            int val = costs[j];
            costs[j] = pC;
            pC = val;
            if (s1[i] != s2[j]) {
                cost = std::min(std::min(costs[j], costs[j + 1]), cost) + 1;
            } else {
                cost = costs[j];
            }
            costs[j + 1] = cost;
        }
    }
    return costs[n];
}

int ExtasisRhythmProcessor::analyzeAudioFile(const juce::File& file) {
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (!reader) return -1;
    
    int numSamples = juce::jmin((int)reader->lengthInSamples, 8192); // Leer primeros ~185ms
    if (numSamples < 100) return -1;

    juce::AudioBuffer<float> buffer(1, numSamples);
    reader->read(&buffer, 0, numSamples, 0, true, false);
    
    const float* data = buffer.getReadPointer(0);
    int zeroCrossings = 0;
    float energy = 0.0f;
    float peak = 0.0f;
    
    for (int i = 1; i < numSamples; ++i) {
        energy += data[i] * data[i];
        if (std::abs(data[i]) > peak) peak = std::abs(data[i]);
        if ((data[i] >= 0.0f && data[i-1] < 0.0f) || (data[i] < 0.0f && data[i-1] >= 0.0f)) {
            zeroCrossings++;
        }
    }
    
    float zcrRate = (float)zeroCrossings / (float)numSamples; // 0.0 a 1.0
    float rms = std::sqrt(energy / numSamples);
    
    // Heurísticas súper simples:
    // ZCR bajo (< 0.05) y mucha energía -> Bombo (0)
    // ZCR muy alto (> 0.20) -> Hihats/Cymbals (2, 3, 10, 11)
    // ZCR medio (0.05 - 0.20) -> Snares/Claps/Percs
    if (zcrRate < 0.04f && peak > 0.4f) return 0; // Kick
    if (zcrRate > 0.25f) return 2; // Hat
    if (zcrRate > 0.10f && zcrRate <= 0.25f && peak > 0.5f) return 1; // Snare
    if (zcrRate >= 0.04f && zcrRate <= 0.10f) return 7; // Mid Perc / Tom
    
    return -1; // Desconocido
}

