#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout ExtasisRhythmProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("isPlaying", 1), "Play", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("bpm", 1), "BPM", juce::NormalisableRange<float>(40.0f, 240.0f, 0.1f), 120.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterVolume", 1), "Master", 0.0f, 2.0f, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterClipper", 1), "Clipper", 0.1f, 1.0f, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("globalKitChoice", 1), "Kit", 0, 30, 0));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterHpf", 1), "HPF", 20.0f, 2000.0f, 20.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterHpfRes", 1), "HRes", 0.1f, 10.0f, 0.71f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterLpf", 1), "LPF", 500.0f, 20000.0f, 20000.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterLpfRes", 1), "LRes", 0.1f, 10.0f, 0.71f));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pcmBits", 1), "Bits", 4.0f, 16.0f, 16.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pcmRate", 1), "Rate", 6.25f, 100.0f, 25.0f));
    
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("flangerOn", 1), "FlangOn", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("flangerRate", 1), "FRate", 0.1f, 10.0f, 0.40f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("flangerFeedback", 1), "FFB", -0.9f, 0.9f, 0.0f));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("transientAttack", 1), "TAtt", -1.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("transientSustain", 1), "TSus", -1.0f, 1.0f, 0.0f));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("driveDist", 1), "Dist", 0.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("driveFilter", 1), "Filter", 0.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("driveVol", 1), "Vol", 0.0f, 2.0f, 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("springDecay", 1), "SDec", 0.1f, 1.0f, 0.40f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("springTone", 1), "STon", 500.0f, 10000.0f, 4000.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("delayTime", 1), "DTime", 10.0f, 1125.0f, 300.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("delayFb", 1), "DFb", 0.0f, 0.95f, 0.30f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("delayModRate", 1), "DModR", 0.1f, 10.0f, 1.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("delayModDepth", 1), "DModD", 0.0f, 1.0f, 0.2f));

    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("tripletFill", 1), "TripFill", false));
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
        params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("triplet" + chStr, 1), "Triplet", false));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("length" + chStr, 1), "Length", 1, 16, 16));
        for (int step = 0; step < 16; ++step)
            params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("step_" + chStr + "_" + juce::String(step), 1), "Step", 0, 3, 0));
    }
    return { params.begin(), params.end() };
}

ExtasisRhythmProcessor::ExtasisRhythmProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "APVTS", createParameterLayout()) {
    
    formatManager.registerBasicFormats();
    samplesFolder = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("ExtasisRhythm_Samples");
    samplesFolder.findChildFiles (drumFolders, juce::File::findDirectories, false);
    drumFolders.sort();
    
    for (int p=0; p<8; ++p) {
        for (int i=0; i<12; ++i) {
            for (int s=0; s<16; ++s) savedPatterns[p][i][s] = 0;
        }
        for (int s=0; s<16; ++s) savedFills[p][s] = 0;
    }

    for (int i=0; i<12; ++i) { 
        samplePositions[i] = -1.0; 
        channelVelocities[i] = 1.0f; 
        channelSteps[i] = 0;
        currentSampleName[i] = "";
        lastSubStep[i] = -1;
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

    if (!drumFolders.isEmpty()) loadGlobalDrumKit(defaultKitIdx);
    isInitialized = true;
}

ExtasisRhythmProcessor::~ExtasisRhythmProcessor() {}
const juce::String ExtasisRhythmProcessor::getName() const { return JucePlugin_Name; }
bool ExtasisRhythmProcessor::acceptsMidi() const { return true; }
bool ExtasisRhythmProcessor::producesMidi() const { return false; }
bool ExtasisRhythmProcessor::isMidiEffect() const { return false; }
double ExtasisRhythmProcessor::getTailLengthSeconds() const { return 0.0; }
int ExtasisRhythmProcessor::getNumPrograms() { return 1; }
int ExtasisRhythmProcessor::getCurrentProgram() { return 0; }
void ExtasisRhythmProcessor::setCurrentProgram (int) {}
const juce::String ExtasisRhythmProcessor::getProgramName (int) { return {}; }
void ExtasisRhythmProcessor::changeProgramName (int, const juce::String&) {}
bool ExtasisRhythmProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const { return true; }

void ExtasisRhythmProcessor::prepareToPlay(double sr, int bs) {
    currentSampleRate = sr; currentSamplesPerBlock = bs;
    lastHostStep = -1; internalElapsedBeats = 0.0; lastFillSubSubStep = -1;
    for (int i = 0; i < 12; ++i) lastSubStep[i] = -1;
    
    killAllAudio();

    juce::dsp::ProcessSpec spec { sr, (uint32_t)bs, 2 };
    
    masterHpfL.prepare(spec); masterHpfR.prepare(spec); masterHpfL.setType(juce::dsp::StateVariableTPTFilterType::highpass); masterHpfR.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    masterLpfL.prepare(spec); masterLpfR.prepare(spec); masterLpfL.setType(juce::dsp::StateVariableTPTFilterType::lowpass); masterLpfR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    ratLpfL.prepare(spec); ratLpfR.prepare(spec); ratLpfL.setType(juce::dsp::StateVariableTPTFilterType::lowpass); ratLpfR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    delayFeedbackLpfL.prepare(spec); delayFeedbackLpfR.prepare(spec); delayFeedbackLpfL.setType(juce::dsp::StateVariableTPTFilterType::lowpass); delayFeedbackLpfR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    delayFeedbackLpfL.setCutoffFrequency(8000.0f); delayFeedbackLpfR.setCutoffFrequency(8000.0f);
    springToneFilterL.prepare(spec); springToneFilterR.prepare(spec); springToneFilterL.setType(juce::dsp::StateVariableTPTFilterType::lowpass); springToneFilterR.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    for (int i = 0; i < 12; ++i) {
        channelToneFilters[i].prepare(spec);
        channelToneFilters[i].setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    }

    flangerBufferL.resize((size_t)(sr * 0.1), 0.0f); flangerBufferR.resize((size_t)(sr * 0.1), 0.0f);
    
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
    delayWritePos = 0; delayLfoPhase = 0.0f; flangerWritePos = 0; flangerLfoPhase = 0.0f;
}

void ExtasisRhythmProcessor::releaseResources() {
    const juce::ScopedLock lock (sampleLock);
    for (int i=0; i<12; ++i) { sampleBuffers[i].setSize(0, 0); sampleLengths[i] = 0; }
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
            const juce::ScopedLock lock (sampleLock);
            sampleBuffers[ch].setSize((int) reader->numChannels, numSamps);
            reader->read(&sampleBuffers[ch], 0, numSamps, 0, true, true);
            sampleLengths[ch] = numSamps;
            currentSampleName[ch] = fileName;
            delete reader;
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

    juce::StringArray classicTokens[12];
    // Diccionario maestro ampliado con soporte para percusión auxiliar, latina y prefijos compuestos
    classicTokens[0].addTokens("bd kik kick kck bombo sub 808_kick 909_kick kickdrum drum_kik", " ", "");
    classicTokens[1].addTokens("sd sn snare tarola caja rim snar s snaredrum drum_snr", " ", "");
    classicTokens[2].addTokens("ch hh closed hat hihat clh hat_c hh_c cht closedhat closed_hh", " ", "");
    classicTokens[3].addTokens("oh open oph ohat hat_o hh_o oht openhat open_hh splash", " ", "");
    classicTokens[4].addTokens("cp clap clp palmas handclap groupclap", " ", "");
    classicTokens[5].addTokens("rs rim rimshot side sidestick woodblock wblk wood clave claves", " ", "");
    classicTokens[6].addTokens("ht hightom tomhi tom1 h-tom tom_hi conga_hi conga_high bongo_hi bnh", " ", "");
    classicTokens[7].addTokens("mt midtom tommid tom2 m-tom tom_mid conga_mid conga bongo_lo bol", " ", "");
    classicTokens[8].addTokens("lt lowtom tomlow tom3 floor ftom l-tom tom_lo timbale tmb conga_lo conga_low", " ", "");
    classicTokens[9].addTokens("cb cowbell bell cwb cow quijada vibraslap agogo ago shaker shk tambourine tamb maraca mrc", " ", "");
    classicTokens[10].addTokens("cr crash cym cymbal crs china", " ", "");
    classicTokens[11].addTokens("rd ride rid ride_bell ride_bow cym_ride", " ", "");

    juce::String bestMatch = "";
    for (auto& fileObj : allFiles) {
        if (fileObj.getFileExtension().toLowerCase() == ".wav") {
            juce::String nameLower = fileObj.getFileNameWithoutExtension().toLowerCase();
            juce::StringArray fileTokens;
            fileTokens.addTokens(nameLower, "_-. ", "");

            bool matched = false;
            for (auto& tok : classicTokens[i]) {
                if (fileTokens.contains(tok)) { matched = true; break; }
            }
            if (matched) { bestMatch = fileObj.getFileName(); break; }
        }
    }

    if (bestMatch.isNotEmpty()) loadSampleForChannel(i, kit, bestMatch);
    else {
        int fallbackIdx = i % allVariants.size();
        loadSampleForChannel(i, kit, allVariants[fallbackIdx]);
    }
}

void ExtasisRhythmProcessor::loadGlobalDrumKit(int kit) {
    for (int i = 0; i < 12; ++i) loadSmartSampleForChannel(i, kit);
}

void ExtasisRhythmProcessor::triggerChannel(int ch, float vel) {
    if (ch >= 0 && ch < 12 && sampleLengths[ch] > 0) {
        samplePositions[ch] = 0.0; channelVelocities[ch] = vel; flashCounters[ch] = 10;
    }
}

void ExtasisRhythmProcessor::killAllAudio() {
    const juce::ScopedLock lock (sampleLock);
    for (int i = 0; i < 12; ++i) {
        samplePositions[i] = -1.0;
        channelSteps[i] = 0;
        flashCounters[i] = 0;
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
    setParam ("masterHpf", 20.0f); setParam ("masterHpfRes", 0.71f);
    setParam ("masterLpf", 20000.0f); setParam ("masterLpfRes", 0.71f); setParam ("pcmBits", 16.0f); setParam ("pcmRate", 25.0f);
    setParam ("flangerOn", 0.0f); setParam ("flangerRate", 0.40f); setParam ("flangerFeedback", 0.0f);
    setParam ("transientAttack", 0.0f); setParam ("transientSustain", 0.0f);
    setParam ("driveDist", 0.0f); setParam ("driveFilter", 0.0f); setParam ("driveVol", 1.0f);
    setParam ("springDecay", 0.40f); setParam ("springTone", 4000.0f);
    setParam ("delayTime", 300.0f); setParam ("delayFb", 0.30f); setParam ("delayModRate", 1.5f); setParam ("delayModDepth", 0.2f);
    setParam ("tripletFill", 0.0f);

    for (int i = 0; i < 12; ++i) {
        juce::String ch = juce::String(i);
        setParam ("gain" + ch, 0.85f); setParam ("pan" + ch, 0.0f); setParam ("pitch" + ch, 0.0f); setParam ("tone" + ch, 7600.0f);
        setParam ("attack" + ch, 0.0005f); setParam ("decay" + ch, 0.40f); setParam ("springSend" + ch, 0.0f); setParam ("delaySend" + ch, 0.0f);
        setParam ("mute" + ch, 0.0f); setParam ("solo" + ch, 0.0f); setParam ("triplet" + ch, 0.0f); setParam ("length" + ch, 16.0f);
    }
}

void ExtasisRhythmProcessor::resetSequencer() {
    for (int i = 0; i < 12; ++i) {
        channelSteps[i] = 0; lastSubStep[i] = -1;
        for (int step = 0; step < 16; ++step) {
            if (auto* p = apvts.getParameter ("step_" + juce::String(i) + "_" + juce::String(step))) {
                p->beginChangeGesture(); p->setValueNotifyingHost (0.0f); p->endChangeGesture();
            }
        }
    }
    lastFillSubSubStep = -1;
    for (int step = 0; step < 16; ++step) {
        if (auto* p = apvts.getParameter ("fill_step_" + juce::String(step))) {
            p->beginChangeGesture(); p->setValueNotifyingHost (0.0f); p->endChangeGesture();
        }
    }
}

void ExtasisRhythmProcessor::changePattern(int newPattern) {
    if (!isInitialized || newPattern == currentPattern || newPattern < 0 || newPattern > 7) return;
    for (int i = 0; i < 12; ++i) {
        for (int s = 0; s < 16; ++s) {
            if (auto* param = apvts.getRawParameterValue("step_" + juce::String(i) + "_" + juce::String(s)))
                savedPatterns[currentPattern][i][s] = (int)(param->load());
        }
    }
    for (int s = 0; s < 16; ++s) {
        if (auto* param = apvts.getRawParameterValue("fill_step_" + juce::String(s)))
            savedFills[currentPattern][s] = (int)(param->load());
    }
    currentPattern = newPattern;
    for (int i = 0; i < 12; ++i) {
        for (int s = 0; s < 16; ++s) {
            if (auto* p = apvts.getParameter ("step_" + juce::String(i) + "_" + juce::String(s))) {
                if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(p)) {
                    rp->beginChangeGesture(); rp->setValueNotifyingHost (rp->convertTo0to1((float)savedPatterns[currentPattern][i][s])); rp->endChangeGesture();
                }
            }
        }
    }
    for (int s = 0; s < 16; ++s) {
        if (auto* p = apvts.getParameter ("fill_step_" + juce::String(s))) {
            if (auto* rp = dynamic_cast<juce::RangedAudioParameter*>(p)) {
                rp->beginChangeGesture(); rp->setValueNotifyingHost (rp->convertTo0to1((float)savedFills[currentPattern][s])); rp->endChangeGesture();
            }
        }
    }
}

void ExtasisRhythmProcessor::getStateInformation (juce::MemoryBlock& destData) {
    if (isInitialized && apvts.getParameter("step_0_0") != nullptr) {
        for (int i = 0; i < 12; ++i) {
            for (int s = 0; s < 16; ++s) {
                if (auto* param = apvts.getRawParameterValue("step_" + juce::String(i) + "_" + juce::String(s)))
                    savedPatterns[currentPattern][i][s] = (int)(param->load());
            }
        }
        for (int s = 0; s < 16; ++s) {
            if (auto* param = apvts.getRawParameterValue("fill_step_" + juce::String(s)))
                savedFills[currentPattern][s] = (int)(param->load());
        }
    }
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    auto patternsXml = new juce::XmlElement("PATTERNS");
    for (int p=0; p<8; ++p) {
        for (int i=0; i<12; ++i) {
            juce::String stepData;
            for (int s=0; s<16; ++s) stepData += juce::String(savedPatterns[p][i][s]) + ",";
            patternsXml->setAttribute("P" + juce::String(p) + "_CH" + juce::String(i), stepData);
        }
        juce::String fillData;
        for (int s=0; s<16; ++s) fillData += juce::String(savedFills[p][s]) + ",";
        patternsXml->setAttribute("P" + juce::String(p) + "_FILLS", fillData);
    }
    xml->addChildElement(patternsXml);

    auto samplesXml = new juce::XmlElement("SAMPLES");
    for (int i = 0; i < 12; ++i) samplesXml->setAttribute("CH" + juce::String(i), currentSampleName[i]);
    xml->addChildElement(samplesXml);

    copyXmlToBinary (*xml, destData);
}

void ExtasisRhythmProcessor::setStateInformation (const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr) {
        if (xmlState->hasTagName (apvts.state.getType())) apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
        if (auto* patternsXml = xmlState->getChildByName("PATTERNS")) {
            for (int p=0; p<8; ++p) {
                for (int i=0; i<12; ++i) {
                    juce::StringArray steps;
                    steps.addTokens(patternsXml->getStringAttribute("P" + juce::String(p) + "_CH" + juce::String(i)), ",", "");
                    for (int s=0; s<16 && s<steps.size(); ++s) savedPatterns[p][i][s] = steps[s].getIntValue();
                }
                juce::StringArray fills;
                fills.addTokens(patternsXml->getStringAttribute("P" + juce::String(p) + "_FILLS"), ",", "");
                for (int s=0; s<16 && s<fills.size(); ++s) savedFills[p][s] = fills[s].getIntValue();
            }
        }
        if (auto* samplesXml = xmlState->getChildByName("SAMPLES")) {
            int kitIdx = (int)apvts.getRawParameterValue("globalKitChoice")->load();
            for (int i = 0; i < 12; ++i) {
                juce::String sName = samplesXml->getStringAttribute("CH" + juce::String(i));
                if (sName.isNotEmpty()) loadSampleForChannel(i, kitIdx, sName);
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

void ExtasisRhythmProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals noDenormals; buffer.clear(); if (currentSampleRate <= 0.0) return;
    for (int i = 0; i < 12; ++i) { int c = flashCounters[i].load(); if (c > 0) flashCounters[i] = c - 1; }
    for (const auto meta : midi) { auto msg = meta.getMessage(); if (msg.isNoteOn()) triggerChannel (msg.getNoteNumber() - 36, msg.getFloatVelocity()); }

    bool currentHostPlaying = false; double currentHostBpm = 120.0; double ppqPosition = 0.0; bool hasHostTime = false;
    if (auto* playHead = getPlayHead()) {
        if (auto pos = playHead->getPosition()) {
            currentHostPlaying = pos->getIsPlaying();
            if (pos->getBpm().hasValue()) currentHostBpm = *pos->getBpm();
            if (pos->getPpqPosition().hasValue()) { ppqPosition = *pos->getPpqPosition(); hasHostTime = true; }
        }
    }
    hostPlaying = currentHostPlaying; hostBpm = currentHostBpm; isSyncedToHost = hasHostTime;

    bool playing = hasHostTime ? currentHostPlaying : (apvts.getRawParameterValue ("isPlaying")->load() > 0.5f);
    double bpm = hasHostTime ? currentHostBpm : apvts.getRawParameterValue ("bpm")->load();

    float hpfFreq = apvts.getRawParameterValue ("masterHpf")->load(); float hpfRes = apvts.getRawParameterValue ("masterHpfRes")->load();
    float lpfFreq = apvts.getRawParameterValue ("masterLpf")->load(); float lpfRes = apvts.getRawParameterValue ("masterLpfRes")->load();
    masterHpfL.setCutoffFrequency(hpfFreq); masterHpfL.setResonance(hpfRes); masterHpfR.setCutoffFrequency(hpfFreq); masterHpfR.setResonance(hpfRes);
    masterLpfL.setCutoffFrequency(lpfFreq); masterLpfL.setResonance(lpfRes); masterLpfR.setCutoffFrequency(lpfFreq); masterLpfR.setResonance(lpfRes);

    float pcmBits = apvts.getRawParameterValue ("pcmBits")->load(); float pcmRateVal = apvts.getRawParameterValue ("pcmRate")->load(); float pcmClockRatio = pcmRateVal / 25.0f;
    bool flangerOn = apvts.getRawParameterValue ("flangerOn")->load() > 0.5f; float flangerRate = apvts.getRawParameterValue ("flangerRate")->load(); float flangerFb = apvts.getRawParameterValue ("flangerFeedback")->load();
    float transAtt = apvts.getRawParameterValue ("transientAttack")->load(); float transSus = apvts.getRawParameterValue ("transientSustain")->load();
    float ratDist = apvts.getRawParameterValue ("driveDist")->load(); float ratFilter = apvts.getRawParameterValue ("driveFilter")->load(); float ratVol = apvts.getRawParameterValue ("driveVol")->load();
    float ratCutoff = 500.0f + (1.0f - ratFilter) * 19500.0f; ratLpfL.setCutoffFrequency(ratCutoff); ratLpfR.setCutoffFrequency(ratCutoff);
    float springDec = apvts.getRawParameterValue ("springDecay")->load(); float springTon = apvts.getRawParameterValue ("springTone")->load();
    springToneFilterL.setCutoffFrequency(springTon); springToneFilterL.setResonance(0.71f); springToneFilterR.setCutoffFrequency(springTon); springToneFilterR.setResonance(0.71f);
    float delTime = apvts.getRawParameterValue("delayTime")->load(); float delFb = apvts.getRawParameterValue("delayFb")->load(); float delModRate = apvts.getRawParameterValue("delayModRate")->load(); float delModDepth = apvts.getRawParameterValue("delayModDepth")->load();
    float mVol = apvts.getRawParameterValue ("masterVolume")->load(); float mClip = apvts.getRawParameterValue ("masterClipper")->load();

    float chanVol[12], chanPan[12], chanPitch[12], chanSSend[12], chanDSend[12], chanAtt[12], chanDec[12];
    bool chanMute[12], chanSolo[12], chanTriplet[12];
    bool anySolo = false;

    for (int i = 0; i < 12; ++i) {
        juce::String ch = juce::String(i);
        chanVol[i] = apvts.getRawParameterValue ("gain" + ch)->load(); chanPan[i] = apvts.getRawParameterValue ("pan" + ch)->load();
        chanPitch[i] = std::pow (2.0f, apvts.getRawParameterValue ("pitch" + ch)->load() / 12.0f) * pcmClockRatio;
        chanSSend[i] = apvts.getRawParameterValue ("springSend" + ch)->load(); chanDSend[i] = apvts.getRawParameterValue ("delaySend" + ch)->load();
        chanAtt[i] = apvts.getRawParameterValue ("attack" + ch)->load(); chanDec[i] = apvts.getRawParameterValue ("decay" + ch)->load();
        chanMute[i] = apvts.getRawParameterValue ("mute" + ch)->load() > 0.5f; chanSolo[i] = apvts.getRawParameterValue ("solo" + ch)->load() > 0.5f;
        chanTriplet[i] = apvts.getRawParameterValue ("triplet" + ch)->load() > 0.5f;
        if (chanSolo[i]) anySolo = true;
        channelToneFilters[i].setCutoffFrequency(apvts.getRawParameterValue("tone" + ch)->load());
    }

    if (!playing) {
        internalElapsedBeats = 0.0;
        for (int i = 0; i < 12; ++i) lastSubStep[i] = -1;
        lastFillSubSubStep = -1;
    }

    const juce::ScopedLock lock (sampleLock);

    for (int s = 0; s < buffer.getNumSamples(); ++s) {
        double beatIncPerSample = (bpm / 60.0) / currentSampleRate;
        if (playing && !hasHostTime) internalElapsedBeats += beatIncPerSample;
        double exactBeats = hasHostTime ? (ppqPosition + ((double)s * beatIncPerSample)) : internalElapsedBeats;

        if (playing) {
            for (int ch = 0; ch < 12; ++ch) {
                double mult = chanTriplet[ch] ? 3.0 : 4.0;
                int maxLen = (int) apvts.getRawParameterValue ("length" + juce::String(ch))->load();
                int numSteps = chanTriplet[ch] ? juce::jmin(maxLen, 12) : (maxLen > 0 ? maxLen : 16);

                double chSubStepD = exactBeats * mult;
                int chSubStep = (int)chSubStepD;

                if (chSubStep != lastSubStep[ch]) {
                    lastSubStep[ch] = chSubStep;
                    int mappedStep = chSubStep % numSteps;
                    if (mappedStep < 0) mappedStep += numSteps;
                    channelSteps[ch] = mappedStep;

                    auto* stepParam = apvts.getRawParameterValue ("step_" + juce::String(ch) + "_" + juce::String(mappedStep));
                    int stepV = (stepParam != nullptr) ? (int)(*stepParam + 0.5f) : 0;
                    if (stepV > 0) triggerChannel(ch, (stepV == 1 ? 0.4f : (stepV == 2 ? 0.7f : 1.0f)));
                }
            }

            bool fillTriplet = apvts.getRawParameterValue("tripletFill")->load() > 0.5f;
            double fillMult = fillTriplet ? 3.0 : 4.0;
            int numFillSteps = fillTriplet ? 12 : 16;
            double fillSubSubStepD = exactBeats * fillMult * 2.0; 
            int fillSubSubStep = (int)fillSubSubStepD;
            
            if (fillSubSubStep != lastFillSubSubStep) {
                lastFillSubSubStep = fillSubSubStep;
                if ((fillSubSubStep % 2) != 0) {
                    int globalFillStep = (fillSubSubStep / 2) % numFillSteps;
                    if (globalFillStep < 0) globalFillStep += numFillSteps;
                    
                    auto* fillParam = apvts.getRawParameterValue ("fill_step_" + juce::String(globalFillStep));
                    int fillV = (fillParam != nullptr) ? (int)(*fillParam + 0.5f) : 0;
                    if (fillV > 0) {
                        for (int ch = 0; ch < 12; ++ch) {
                            if (sampleLengths[ch] > 0) {
                                int curS = channelSteps[ch].load();
                                auto* stepParam = apvts.getRawParameterValue ("step_" + juce::String(ch) + "_" + juce::String(curS));
                                int stepV = (stepParam != nullptr) ? (int)(*stepParam + 0.5f) : 0;
                                if (stepV > 0) triggerChannel(ch, stepV == 1 ? 0.4f : (stepV == 2 ? 0.7f : 1.0f));
                            }
                        }
                    }
                }
            }
        }

        float mixedL = 0.0f, mixedR = 0.0f; float flangedL = 0.0f, flangedR = 0.0f;
        float reverbSendTotalL = 0.0f, reverbSendTotalR = 0.0f; float delaySendTotalL = 0.0f, delaySendTotalR = 0.0f;

        for (int i = 0; i < 12; ++i) {
            double pos = samplePositions[i].load();
            if (pos >= 0.0 && sampleLengths[i] > 0) {
                int idx1 = (int)pos; int idx2 = idx1 + 1;
                if (idx1 >= sampleLengths[i]) { samplePositions[i] = -1.0; continue; }

                float frac = (float)(pos - (double)idx1); float sL = 0.0f, sR = 0.0f;
                if (idx2 < sampleLengths[i]) {
                    float s1L = sampleBuffers[i].getSample(0, idx1); float s2L = sampleBuffers[i].getSample(0, idx2);
                    sL = s1L + frac * (s2L - s1L);
                    if (sampleBuffers[i].getNumChannels() > 1) { float s1R = sampleBuffers[i].getSample(1, idx1); float s2R = sampleBuffers[i].getSample(1, idx2); sR = s1R + frac * (s2R - s1R); } else sR = sL;
                } else { sL = sampleBuffers[i].getSample(0, idx1); sR = sampleBuffers[i].getNumChannels() > 1 ? sampleBuffers[i].getSample(1, idx1) : sL; }

                if (chanMute[i] || (anySolo && !chanSolo[i])) {
                    double nextPos = pos + chanPitch[i]; if (nextPos >= sampleLengths[i]) samplePositions[i] = -1.0; else samplePositions[i] = nextPos;
                    continue;
                }

                float timeInSec = (float)pos / (float)currentSampleRate;
                float env = 1.0f;
                if (timeInSec < chanAtt[i]) env = timeInSec / juce::jmax(0.0001f, chanAtt[i]);
                else env = std::exp (-(timeInSec - chanAtt[i]) / juce::jmax(0.01f, chanDec[i]));

                float attackEnv = std::exp (-timeInSec / 0.015f); float sustainEnv = 1.0f - attackEnv;
                float transMod = 1.0f + (transAtt * attackEnv * 1.3f) + (transSus * sustainEnv * 0.7f);

                sL *= (env * transMod); sR *= (env * transMod);
                sL = channelToneFilters[i].processSample(0, sL); sR = channelToneFilters[i].processSample(0, sR);

                float finalGain = 0.35f * chanVol[i] * channelVelocities[i].load();
                float chL = sL * std::sqrt (0.5f * (1.0f - chanPan[i])) * finalGain; float chR = sR * std::sqrt (0.5f * (1.0f + chanPan[i])) * finalGain;
                chL = std::tanh(chL * 1.5f); chR = std::tanh(chR * 1.5f);

                if (i == 0) { mixedL += chL; mixedR += chR; } else { flangedL += chL; flangedR += chR; }
                reverbSendTotalL += chL * chanSSend[i]; reverbSendTotalR += chR * chanSSend[i]; delaySendTotalL += chL * chanDSend[i]; delaySendTotalR += chR * chanDSend[i];
                double nextPos = pos + chanPitch[i]; if (nextPos >= sampleLengths[i]) samplePositions[i] = -1.0; else samplePositions[i] = nextPos;
            }
        }

        if (flangerOn && !flangerBufferL.empty()) {
            flangerLfoPhase += (flangerRate / (float)currentSampleRate); if (flangerLfoPhase >= 1.0f) flangerLfoPhase -= 1.0f;
            float lfo = std::sin(2.0f * (float)juce::MathConstants<double>::pi * flangerLfoPhase);
            int delaySamples = (int)((0.005f + 0.004f * (lfo + 1.0f)) * (float)currentSampleRate);
            
            int readPosL = (flangerWritePos - delaySamples + (int)flangerBufferL.size()) % (int)flangerBufferL.size();
            float delayedL = flangerBufferL[(size_t)readPosL]; flangerBufferL[(size_t)flangerWritePos] = flangedL + delayedL * flangerFb;

            int readPosR = (flangerWritePos - delaySamples + (int)flangerBufferR.size()) % (int)flangerBufferR.size();
            float delayedR = flangerBufferR[(size_t)readPosR]; flangerBufferR[(size_t)flangerWritePos] = flangedR + delayedR * flangerFb;

            flangerWritePos = (flangerWritePos + 1) % (int)flangerBufferL.size();
            flangedL = 0.5f * (flangedL + delayedL); flangedR = 0.5f * (flangedR + delayedR);
        }

        mixedL += flangedL; mixedR += flangedR;
        float ratGainFactor = 1.0f + (std::pow(ratDist, 2.0f) * 100.0f);
        mixedL = std::tanh(mixedL * ratGainFactor); mixedR = std::tanh(mixedR * ratGainFactor);
        mixedL = ratLpfL.processSample(0, mixedL) * ratVol; mixedR = ratLpfR.processSample(0, mixedR) * ratVol;

        if (!springDelayL[0].empty()) {
            float feedback = 0.2f + springDec * 0.75f;
            float driveL = std::tanh(reverbSendTotalL * 1.5f); float driveR = std::tanh(reverbSendTotalR * 1.5f);
            float tankInL = springToneFilterL.processSample(0, driveL); float tankInR = springToneFilterR.processSample(0, driveR);
            float springOutL = 0.0f; float springOutR = 0.0f;

            for (int j = 0; j < 3; ++j) {
                float dL = springDelayL[j][springPos[j]]; float dR = springDelayR[j][springPos[j]];
                float apL = springApL[j].processSample(0, tankInL + dL * feedback); float apR = springApR[j].processSample(0, tankInR + dR * feedback);
                springDelayL[j][springPos[j]] = apL; springDelayR[j][springPos[j]] = apR; springOutL += dL; springOutR += dR;
                springPos[j] = (springPos[j] + 1) % springDelayL[j].size();
            }
            mixedL += springOutL * 0.5f; mixedR += springOutR * 0.5f;
        }

        if (delayBufferLength > 0) {
            delayLfoPhase += (delModRate / (float)currentSampleRate); if (delayLfoPhase >= 1.0f) delayLfoPhase -= 1.0f;
            float lfoVal = std::sin(2.0f * (float)juce::MathConstants<double>::pi * delayLfoPhase);
            float currentDelMs = delTime + (lfoVal * delModDepth * 5.0f); float delSamples = (currentDelMs / 1000.0f) * (float)currentSampleRate;
            int writeP = delayWritePos; float readPosExact = (float)writeP - delSamples; if (readPosExact < 0.0f) readPosExact += (float)delayBufferLength;
            int idx1 = (int)readPosExact; int idx2 = (idx1 + 1) % delayBufferLength; float frac = readPosExact - (float)idx1;

            float dOutL = delayBufferL[idx1] + frac * (delayBufferL[idx2] - delayBufferL[idx1]); float dOutR = delayBufferR[idx1] + frac * (delayBufferR[idx2] - delayBufferR[idx1]);
            dOutL = delayFeedbackLpfL.processSample(0, dOutL); dOutR = delayFeedbackLpfR.processSample(0, dOutR);
            delayBufferL[writeP] = delaySendTotalL + dOutL * delFb; delayBufferR[writeP] = delaySendTotalR + dOutR * delFb;
            delayWritePos = (writeP + 1) % delayBufferLength;
            mixedL += dOutL; mixedR += dOutR;
        }

        if (pcmBits < 16.0f) { float steps = std::pow(2.0f, pcmBits); mixedL = std::round(mixedL * steps) / steps; mixedR = std::round(mixedR * steps) / steps; }
        mixedL = masterHpfL.processSample(0, mixedL); mixedR = masterHpfR.processSample(0, mixedR);
        mixedL = masterLpfL.processSample(0, mixedL); mixedR = masterLpfR.processSample(0, mixedR);
        mixedL *= (mVol * 1.4125f); mixedR *= (mVol * 1.4125f);
        mixedL = std::tanh (mixedL / mClip) * mClip; mixedR = std::tanh (mixedR / mClip) * mClip;

        buffer.setSample (0, s, mixedL); if (buffer.getNumChannels() > 1) buffer.setSample (1, s, mixedR);
    }
    float curRmsL = buffer.getRMSLevel (0, 0, buffer.getNumSamples()); float curRmsR = buffer.getNumChannels() > 1 ? buffer.getRMSLevel (1, 0, buffer.getNumSamples()) : curRmsL;
    outputLevelL = juce::jmax (curRmsL, outputLevelL.load() * 0.8f); outputLevelR = juce::jmax (curRmsR, outputLevelR.load() * 0.8f);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new ExtasisRhythmProcessor(); }