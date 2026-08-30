#include <algorithm>
#include "LicenseManager.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

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

    for (int i = 0; i < 8; ++i) {
        juce::String chStr = juce::String (i);
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("sampleSource_" + chStr, 1), "Source", 0, 30, 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("gain" + chStr, 1), "Gain", 0.0f, 1.0f, 0.85f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pan" + chStr, 1), "Pan", -1.0f, 1.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pitch" + chStr, 1), "Pitch", -24.0f, 24.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("tone" + chStr, 1), "Tone", -1.0f, 1.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("attack" + chStr, 1), "Attack", 0.0005f, 0.05f, 0.0005f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("decay" + chStr, 1), "Decay", juce::NormalisableRange<float>(0.02f, 3.0f, 0.001f, 0.25f), 0.40f));
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
    
    // --- RESET TOTAL DE PATRONES AL INICIAR ---
    for (int p=0; p<8; ++p) {
        for (int i=0; i< 8; ++i) {
            for (int s=0; s<32; ++s) {
                savedPatterns[p][i][s] = 0;
                savedGlides[p][i][s] = 0;
                savedNotes[p][i][s] = 0;
            }
        }
        for (int s=0; s<16; ++s) savedFills[p][s] = 0;
    }

    for (int i=0; i< 8; ++i) { 
        samplePositions[i] = -1.0; 
        samplePositionsOld[i] = -1.0;
        fadeOld[i] = 0.0f;
        channelVelocities[i] = 1.0f; 
        channelSteps[i] = 0;
        channelStepSemitones[i] = 0.0f;
        lastRatchetTick[i] = -1;
        currentMappedStep[i] = 0;

        currentSampleName.add("");
        lastSubStep[i] = -1;
        seqModes[i] = 0;
        seqPingDir[i] = 1;
        seqPingPos[i] = 0;
        
        // Forzamos el APVTS a cero (Blank Pattern UI sync)
        for (int s=0; s<32; ++s) {
            if (auto* param = apvts.getParameter("step_" + juce::String(i) + "_" + juce::String(s))) {
                param->setValueNotifyingHost(0.0f);
            }
        }
    }
    
    // --- BUSCAMOS EL KIT 808 POR DEFAULT ---
    int defaultKitIdx = 0;
    for (int k = 0; k < drumFolders.size(); ++k) {
        if (drumFolders[k].getFileName().toLowerCase().contains("707")) {
            defaultKitIdx = k;
            break;
        }
    }
    
    // Actualizamos el APVTS Global antes de inicializar la UI
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

void ExtasisRhythmProcessor::scanSampleFolders() {
    for (int i = 0; i < 8; ++i) categorizedFiles[i].clear();
    
    juce::Array<juce::File> allWavs;
    for (auto& folderPath : drumFolders) {
        juce::File folder(folderPath);
        if (folder.isDirectory()) {
            allWavs.addArray(folder.findChildFiles(juce::File::findFiles, true, "*.wav;*.aif;*.aiff"));
        }
    }
    
    for (auto& file : allWavs) {
        juce::String nameLower = file.getFileNameWithoutExtension().toLowerCase();
        juce::StringArray fileTokens;
        fileTokens.addTokens(nameLower, "_-. 0123456789()[]", "");
        
        int bestCat = -1;
        int bestDist = 999;
        
        for (int c = 0; c < 8; ++c) {
            for (auto& token : fileTokens) {
                if (token.length() <= 1) continue;
                for (auto& dictWord : channelTags[c]) {
                    int dist = levenshteinDistance(token, dictWord);
                    if (dist < bestDist && dist <= 1) { // Very strict fuzzy or exact match
                        bestDist = dist;
                        bestCat = c;
                    }
                }
            }
        }
        
        // If not matched, guess by primitive heuristic or just dump in Perc2
        if (bestCat == -1) {
            if (nameLower.contains("kick") || nameLower.contains("bd")) bestCat = 0;
            else if (nameLower.contains("snare") || nameLower.contains("sd")) bestCat = 1;
            else if (nameLower.contains("hat") || nameLower.contains("hh")) bestCat = 3;
            else bestCat = 6;
        }
        
        categorizedFiles[bestCat].add(file);
    }
    
    // Auto-load defaults
    for (int i = 0; i < 8; ++i) {
        if (!categorizedFiles[i].isEmpty()) {
            currentCategoryIndex[i].store(i);
            currentSampleIndex[i].store(0);
            loadSampleFromCategory(i);
        }
    }
}


void ExtasisRhythmProcessor::loadSmartSampleForChannel(int ch) {
    if (ch < 0 || ch >= 8) return;
    loadSampleFromCategory(ch);
}

void ExtasisRhythmProcessor::loadNextSample(int channel, int direction) {
    if (channel < 0 || channel >= 8) return;
    int cat = currentCategoryIndex[channel].load();
    if (categorizedFiles[cat].isEmpty()) return;
    
    int currentIdx = currentSampleIndex[channel].load();
    currentIdx += direction;
    if (currentIdx < 0) currentIdx = categorizedFiles[cat].size() - 1;
    if (currentIdx >= categorizedFiles[cat].size()) currentIdx = 0;
    
    currentSampleIndex[channel].store(currentIdx);
    loadSampleFromCategory(channel);
}

void ExtasisRhythmProcessor::changeCategory(int channel, int direction) {
    if (channel < 0 || channel >= 8) return;
    int cat = currentCategoryIndex[channel].load();
    cat += direction;
    if (cat < 0) cat = 7;
    if (cat > 7) cat = 0;
    
    currentCategoryIndex[channel].store(cat);
    currentSampleIndex[channel].store(0);
    loadSampleFromCategory(channel);
}

void ExtasisRhythmProcessor::loadSampleFromCategory(int channel) {
    int cat = currentCategoryIndex[channel].load();
    int idx = currentSampleIndex[channel].load();
    if (cat >= 0 && cat < 8 && !categorizedFiles[cat].isEmpty()) {
        idx = juce::jlimit(0, categorizedFiles[cat].size() - 1, idx);
        sampleBuffers[channel] = new SampleBuffer(categorizedFiles[cat][idx], formatManager);
    }
}


void ExtasisRhythmProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
    auto startTime = juce::Time::getHighResolutionTicks(); 

    juce::ScopedNoDenormals noDenormals; 
    
    juce::MidiBuffer incomingMidi;
    incomingMidi.addEvents (midi, 0, buffer.getNumSamples(), 0);
    midi.clear();
    SampleBuffer::Ptr localSamples[8];
    {
        juce::SpinLock::ScopedLockType sl(pointerLock);
        for (int i=0; i< 8; ++i) localSamples[i] = sampleBuffers[i];
    }

    buffer.clear(); 
    if (getSampleRate() <= 0.0 || buffer.getNumChannels() == 0) return;

    if (!isLicensedCached.load()) {
        int64_t currentElapsed = demoSamplesElapsed.load();
        int64_t maxDemoSamples = (int64_t)(getSampleRate() * 600.0); 
        if (currentElapsed >= maxDemoSamples) {
            demoExpired.store (true);
            buffer.clear(); // MUTE THE AUDIO
            return;
        } else {
            demoSamplesElapsed.store (currentElapsed + buffer.getNumSamples());
        }
    }
    
    float* stemWriteL[8] = { nullptr };
    float* stemWriteR[8] = { nullptr };
    bool stemEnabled[8] = { false };
    
    int totalBufferChannels = buffer.getNumChannels();
    float* masterL = totalBufferChannels > 0 ? buffer.getWritePointer(0) : nullptr;
    float* masterR = totalBufferChannels > 1 ? buffer.getWritePointer(1) : masterL;
    
    int totalOutputBuses = getBusCount(false);
    for (int b = 0; b < 8; ++b) {
        int busIdx = b + 1;
        if (busIdx < totalOutputBuses) {
            if (auto* bus = getBus(false, busIdx)) {
                if (bus->isEnabled()) {
                    int chOffset = bus->getChannelIndexInProcessBlockBuffer(false);
                    int numChans = bus->getNumberOfChannels();
                    
                    if (chOffset >= 0 && (chOffset + numChans) <= totalBufferChannels) {
                        stemEnabled[b] = true;
                        if (numChans > 0) stemWriteL[b] = buffer.getWritePointer(chOffset);
                        if (numChans > 1) stemWriteR[b] = buffer.getWritePointer(chOffset + 1);
                        else stemWriteR[b] = stemWriteL[b];
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < 8; ++i) { int c = flashCounters[i].load(); if (c > 0) flashCounters[i] = c - 1; }
    
    for (const auto meta : incomingMidi) { 
        auto msg = meta.getMessage(); 
        int ch = getChannelForMidiNote (msg.getNoteNumber());
        if (ch >= 0 && ch < 8) {
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

    float pcmBits = cachedParams.pcmBits->load(); float pcmRateVal = cachedParams.pcmRate->load();
    
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

    float chanVol[8], chanPan[8], chanPitch[8], chanSSend[8], chanDSend[8], chanAtt[8], chanDec[8];
    bool chanMute[8], chanSolo[8], chanEnv[8], chanTriplet[8], chanFit[8];
    bool anySolo = false;

    for (int i = 0; i < 8; ++i) {
        juce::String ch = juce::String(i);
        volSmoother[i].setTargetValue(cachedParams.chanGain[i]->load());
        panSmoother[i].setTargetValue(cachedParams.chanPan[i]->load());
        pitchSmoother[i].setTargetValue(cachedParams.chanPitch[i]->load() + channelStepSemitones[i].load());

        chanSSend[i] = cachedParams.chanSSend[i]->load(); chanDSend[i] = cachedParams.chanDSend[i]->load();
        chanAtt[i] = cachedParams.chanAttack[i]->load(); chanDec[i] = cachedParams.chanDecay[i]->load();
        chanMute[i] = cachedParams.chanMute[i]->load() > 0.5f; chanSolo[i] = cachedParams.chanSolo[i]->load() > 0.5f;
        chanEnv[i]  = cachedParams.chanEnv[i]->load() > 0.5f;
        chanTriplet[i] = cachedParams.chanTriplet[i]->load() > 0.5f;
        chanFit[i] = cachedParams.chanFit[i]->load() > 0.5f;
        if (chanSolo[i]) anySolo = true;
        
        float toneVal = cachedParams.chanTone[i]->load();
        if (toneVal < 0.0f) {
            float cutoff = 20000.0f * std::pow(20.0f / 20000.0f, -toneVal);
            channelToneFilters[i].setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            channelToneFilters[i].setCutoffFrequency(juce::jlimit(20.0f, 20000.0f, cutoff));
        } else if (toneVal > 0.0f) {
            float cutoff = 20.0f * std::pow(20000.0f / 20.0f, toneVal);
            channelToneFilters[i].setType(juce::dsp::StateVariableTPTFilterType::highpass);
            channelToneFilters[i].setCutoffFrequency(juce::jlimit(20.0f, 20000.0f, cutoff));
        }
        
        bool isMuted = chanMute[i] || (anySolo && !chanSolo[i]);
        muteSmoother[i].setTargetValue(isMuted ? 0.0f : 1.0f);
        
        envFilterL[i].setCutoffFrequency(envCut); envFilterL[i].setResonance(envRes);
        envFilterR[i].setCutoffFrequency(envCut); envFilterR[i].setResonance(envRes);
    }

    if (!playing) {
        internalElapsedBeats = 0.0;
        for (int i = 0; i < 8; ++i) { 
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

    auto readInterp = [](const std::vector<float>& buf, float exactPos, int size) -> float {
        while (exactPos < 0.0f) exactPos += (float)size;
        while (exactPos >= (float)size) exactPos -= (float)size;
        int i1 = (int)exactPos;
        int i2 = (i1 + 1) % size;
        float frac = exactPos - (float)i1;
        return buf[(size_t)i1] + frac * (buf[(size_t)i2] - buf[(size_t)i1]);
    };

    for (int s = 0; s < buffer.getNumSamples(); ++s) {
        smoothedDelayTime = smoothedDelayTime * 0.995f + targetDelSamples * 0.005f;

        float currentMuteGain[8];
        for (int i = 0; i < 8; ++i) {
            chanVol[i] = volSmoother[i].getNextValue();
            chanPan[i] = panSmoother[i].getNextValue();
            currentMuteGain[i] = muteSmoother[i].getNextValue();
            float semitones = pitchSmoother[i].getNextValue();
            chanPitch[i] = (float)(std::pow(2.0, (double)semitones / 12.0) * ((localSamples[i] && localSamples[i]->sampleRate > 0) ? (localSamples[i]->sampleRate / getSampleRate()) : 1.0));
        }

        double beatIncPerSample = (bpm / 60.0) / getSampleRate();
        if (playing && !hasHostTime) internalElapsedBeats += beatIncPerSample;
        double exactBeats = hasHostTime ? (ppqPosition + ((double)s * beatIncPerSample)) : internalElapsedBeats;

        if (playing) {
            bool fillTriplet = (cachedParams.tripletFill != nullptr && cachedParams.tripletFill->load() > 0.5f);
            bool fillFit = (cachedParams.fillFit != nullptr && cachedParams.fillFit->load() > 0.5f);
            int numFillSteps = 16;
            if (cachedParams.fillLength != nullptr) {
                numFillSteps = juce::jlimit(1, 16, (int)cachedParams.fillLength->load());
            }
            
            double fillMult = fillFit ? ((double)numFillSteps / 4.0) : (fillTriplet ? 3.0 : 4.0);
            double fillSubStepD = exactBeats * fillMult; 
            int fillSubStep = (int)std::floor(fillSubStepD); 
            
            if (fillSubStep != lastFillSubStep) {
                lastFillSubStep = fillSubStep;
                int mappedFillStep = 0;
                int fMode = fillSeqMode.load();
                
                int phaseFill = fillSubStep % numFillSteps;
                if (phaseFill < 0) phaseFill += numFillSteps;

                if (numFillSteps <= 1) mappedFillStep = 0;
                else if (fMode == 1) mappedFillStep = (numFillSteps - 1) - phaseFill;
                else if (fMode == 2) mappedFillStep = random.nextInt(numFillSteps);
                else if (fMode == 3) { 
                    int cycleLen = (numFillSteps - 1) * 2;
                    int p2 = fillSubStep % cycleLen;
                    if (p2 < 0) p2 += cycleLen;
                    mappedFillStep = (p2 < numFillSteps) ? p2 : cycleLen - p2;
                } else mappedFillStep = phaseFill;
                
                fillSeqPos = juce::jlimit(0, 15, mappedFillStep);
            }

            int safeFillIndex = juce::jlimit(0, 15, fillSeqPos);
            int currentFillV = 0;
            if (cachedParams.fillStepParams[safeFillIndex] != nullptr) {
                currentFillV = (int)(cachedParams.fillStepParams[safeFillIndex]->load() + 0.5f);
            }
            
            int ratchets = (currentFillV == 1) ? 2 : ((currentFillV == 2) ? 3 : 1);

            for (int ch = 0; ch < 8; ++ch) {
                int maxLen = (int) (cachedParams.lengthParams[ch] != nullptr ? cachedParams.lengthParams[ch]->load() : 16.0f);
                int numSteps = chanFit[ch] ? juce::jlimit(1, 32, maxLen > 0 ? maxLen : 16) : (chanTriplet[ch] ? juce::jlimit(1, 24, maxLen > 0 ? maxLen : 12) : juce::jlimit(1, 32, maxLen > 0 ? maxLen : 16));

                double mult = chanFit[ch] ? ((double)numSteps / 4.0) : (chanTriplet[ch] ? 3.0 : 4.0);
                double chSubStepD = exactBeats * mult;
                int chSubStep = (int)std::floor(chSubStepD); 
                double stepFraction = chSubStepD - (double)chSubStep;

                if (chSubStep != lastSubStep[ch]) {
                    lastSubStep[ch] = chSubStep;

                    int mappedStep = 0;
                    int mode = seqModes[ch].load();
                    
                    int phase = chSubStep % numSteps;
                    if (phase < 0) phase += numSteps;

                    if (numSteps <= 1) mappedStep = 0;
                    else if (mode == 1) mappedStep = (numSteps - 1) - phase;
                    else if (mode == 2) mappedStep = random.nextInt (numSteps);
                    else if (mode == 3) { 
                        int cycleLen = (numSteps - 1) * 2;
                        int p2 = chSubStep % cycleLen;
                        if (p2 < 0) p2 += cycleLen;
                        mappedStep = (p2 < numSteps) ? p2 : cycleLen - p2;
                    } else mappedStep = phase;
                    
                    mappedStep = juce::jlimit(0, 31, mappedStep);
                    currentMappedStep[ch] = mappedStep;
                    channelSteps[ch] = mappedStep;

                    int pat = getCurrentPattern();
                    channelStepSemitones[ch] = (float)savedNotes[pat][ch][mappedStep];

                    if (activeMidiNotes[ch] >= 0) {
                        midi.addEvent (juce::MidiMessage::noteOff (ch + 1, activeMidiNotes[ch]), s);
                        activeMidiNotes[ch] = -1;
                    }
                }

                int mappedStep = currentMappedStep[ch].load();
                auto* stepParam = cachedParams.stepParams[ch][mappedStep];
                int stepV = (stepParam != nullptr) ? (int)(stepParam->load() + 0.5f) : 0;

                if (stepV > 0) {
                    int currentRatchetIdx = (int)(stepFraction * ratchets);
                    if (currentRatchetIdx >= ratchets) currentRatchetIdx = ratchets - 1; 

                    int64_t absoluteRatchetTick = (int64_t)chSubStep * 12 + (currentRatchetIdx * 12 / ratchets);

                    if (absoluteRatchetTick != lastRatchetTick[ch].load()) {
                        lastRatchetTick[ch] = absoluteRatchetTick;
                        
                        float stepVel = (stepV == 1 ? 0.4f : (stepV == 2 ? 0.7f : 1.0f));
                        if (ratchets > 1 && currentRatchetIdx > 0) stepVel *= 0.82f;

                        if (localSamples[ch] != nullptr) {
                            triggerChannel (ch, stepVel);
                        } else {
                            flashCounters[ch] = 15;
                        }

                        int baseNote = getMidiNoteForChannel (ch);
                        int targetNote = juce::jlimit (0, 127, baseNote + (int)channelStepSemitones[ch].load());
                        int midiChan = ch + 1; 

                        if (activeMidiNotes[ch] >= 0) {
                            midi.addEvent (juce::MidiMessage::noteOff (midiChan, activeMidiNotes[ch]), s);
                        }
                        midi.addEvent (juce::MidiMessage::noteOn (midiChan, targetNote, stepVel), s);
                        activeMidiNotes[ch] = targetNote;
                    }
                }
            } 
        }

        float kickL = 0.0f, kickR = 0.0f;
        float otherL = 0.0f, otherR = 0.0f;
        float reverbSendTotalL = 0.0f, reverbSendTotalR = 0.0f; 
        float delaySendTotalL = 0.0f, delaySendTotalR = 0.0f;

        for (int i = 0; i < 8; ++i) {
            if (localSamples[i] == nullptr) continue;

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
                float toneVal = cachedParams.chanTone[i]->load();
                if (toneVal != 0.0f) {
                    chOutL = channelToneFilters[i].processSample(0, chOutL); 
                    chOutR = channelToneFilters[i].processSample(1, chOutR);
                }

                if (chanEnv[i]) {
                    float dynCutoff = juce::jlimit(20.0f, 20000.0f, envCut * (0.05f + 0.95f * env));
                    
                    envFilterL[i].setCutoffFrequency(dynCutoff);
                    envFilterR[i].setCutoffFrequency(dynCutoff);
                    
                    chOutL = envFilterL[i].processSample(0, chOutL);
                    chOutR = envFilterR[i].processSample(0, chOutR);
                }

                float finalGain = 0.35f * chanVol[i] * channelVelocities[i].load() * currentMuteGain[i];
                
                if (finalGain > 0.00001f) {
                    float pannedL = chOutL * std::sqrt (0.5f * (1.0f - chanPan[i])) * finalGain; 
                    float pannedR = chOutR * std::sqrt (0.5f * (1.0f + chanPan[i])) * finalGain;
                    pannedL = fastTanh (pannedL * 1.5f); pannedR = fastTanh (pannedR * 1.5f);

                    if (i < 8 && stemEnabled[i]) {
                        if (stemWriteL[i] != nullptr) stemWriteL[i][s] += (pannedL * 2.0f);
                        if (stemWriteR[i] != nullptr) stemWriteR[i][s] += (pannedR * 2.0f);
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
        }

        auto processMasterBusChain = [&](float& l, float& r, bool isKick) {
            if (isKick) {
                l = kickHpfL.processSample(0, l); r = kickHpfR.processSample(0, r);
                l = kickLpfL.processSample(0, l); r = kickLpfR.processSample(0, r);
                float ratGainFactor = 1.0f + (std::pow(ratDist, 2.0f) * 100.0f);
                l = fastTanh(l * ratGainFactor + 0.1f) - 0.0996f; 
                r = fastTanh(r * ratGainFactor + 0.1f) - 0.0996f;
                l = kickRatLpfL.processSample(0, l) * ratVol; r = kickRatLpfR.processSample(0, r) * ratVol;
            } else {
                l = otherHpfL.processSample(0, l); r = otherHpfR.processSample(0, r);
                l = otherLpfL.processSample(0, l); r = otherLpfR.processSample(0, r);
                float ratGainFactor = 1.0f + (std::pow(ratDist, 2.0f) * 100.0f);
                l = fastTanh(l * ratGainFactor + 0.1f) - 0.0996f; 
                r = fastTanh(r * ratGainFactor + 0.1f) - 0.0996f;
                l = otherRatLpfL.processSample(0, l) * ratVol; r = otherRatLpfR.processSample(0, r) * ratVol;
            }
        };

        processMasterBusChain(kickL, kickR, true);
        processMasterBusChain(otherL, otherR, false);

        if (flangerOn && !flangerBufferL.empty()) {
            flangerLfoPhase += (flangerRate / (float)getSampleRate()); if (flangerLfoPhase >= 1.0f) flangerLfoPhase -= 1.0f;
            float lfo = std::sin(2.0f * (float)juce::MathConstants<double>::pi * flangerLfoPhase);
            float delaySamples = (0.005f + 0.004f * (lfo + 1.0f)) * (float)getSampleRate();
            
            float delayedL = readInterp(flangerBufferL, (float)flangerWritePos - delaySamples, (int)flangerBufferL.size());
            float delayedR = readInterp(flangerBufferR, (float)flangerWritePos - delaySamples, (int)flangerBufferR.size());

            flangerBufferL[(size_t)flangerWritePos] = otherL + delayedL * flangerFb;
            flangerBufferR[(size_t)flangerWritePos] = otherR + delayedR * flangerFb;

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
            
            float cDelayedL = readInterp(chorusBufferL, (float)chorusWritePos - delSamplesL, cBufSize);
            float cDelayedR = readInterp(chorusBufferR, (float)chorusWritePos - delSamplesR, cBufSize);

            chorusBufferL[(size_t)chorusWritePos] = otherL;
            chorusBufferR[(size_t)chorusWritePos] = otherR;
            chorusWritePos = (chorusWritePos + 1) % cBufSize;

            otherL = juce::jlimit(-1.0f, 1.0f, 0.707f * (otherL + cDelayedL));
            otherR = juce::jlimit(-1.0f, 1.0f, 0.707f * (otherR + cDelayedR));
        }

        float mixedL = kickL + otherL;
        float mixedR = kickR + otherR;

        if (false && !springDelayL[0].empty()) { // BYPASSED FOR HW DSP DIET
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

        if (pcmBits < 16.0f || pcmRateVal < 100.0f) {
            float stepSize = 100.0f / juce::jmax(1.0f, pcmRateVal); 
            pcmPhase += 1.0f;
            if (pcmPhase >= stepSize) {
                pcmHoldL = mixedL;
                pcmHoldR = mixedR;
                pcmPhase -= stepSize;
            } else {
                mixedL = pcmHoldL;
                mixedR = pcmHoldR;
            }

            if (pcmBits < 16.0f) { 
                float steps = std::pow(2.0f, pcmBits); 
                mixedL = std::round(mixedL * steps) / steps; 
                mixedR = std::round(mixedR * steps) / steps; 
            }
        }

        mixedL *= (mVol * 1.4125f); mixedR *= (mVol * 1.4125f);
        mixedL = fastTanh (mixedL / mClip) * mClip; mixedR = fastTanh (mixedR / mClip) * mClip;

        if (pumpOn) {
            float currentPeak = juce::jmax(std::abs(mixedL), std::abs(mixedR));
            float attackCoef = 0.05f; 
            float releaseCoef = 0.0003f;
            if (currentPeak > pumpEnvelope) {
                pumpEnvelope += attackCoef * (currentPeak - pumpEnvelope);
            } else {
                pumpEnvelope += releaseCoef * (currentPeak - pumpEnvelope);
            }

            float thresholdLinear = juce::Decibels::decibelsToGain(pumpThr);
            if (pumpEnvelope > thresholdLinear) {
                float reduction = 1.0f - (pumpAmt * (1.0f - (thresholdLinear / juce::jmax(0.0001f, pumpEnvelope))));
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

        mixedL *= 2.0f;
        mixedR *= 2.0f;

        if (masterL != nullptr) masterL[s] = mixedL; 
        if (masterR != nullptr) masterR[s] = mixedR;
    }

    float curPeakL = buffer.getMagnitude (0, 0, buffer.getNumSamples()); 
    float curPeakR = buffer.getNumChannels() > 1 ? buffer.getMagnitude (1, 0, buffer.getNumSamples()) : curPeakL;
    
    outputLevelL = juce::jmax (curPeakL, outputLevelL.load() * 0.92f); 
    outputLevelR = juce::jmax (curPeakR, outputLevelR.load() * 0.92f);

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
        for (int i = 0; i < 8; ++i) {
            juce::String tokens = obj->getProperty(keys[i]).toString();
            channelTags[i].addTokens(tokens, " ,", "");
        }
    } else {
        channelTags[0].addTokens("bd kik kick kck bombo sub bassdrum bassd kickdrum drum_kik", " ", ""); // Kick
        channelTags[1].addTokens("sd sn snare tarola caja rim snar s snaredrum drum_snr rs rimshot side", " ", ""); // Snare/Rim
        channelTags[2].addTokens("cp clap clp palmas handclap groupclap cla", " ", ""); // Clap
        channelTags[3].addTokens("ch hh closed hat hihat clh hat_c hh_c cht closedhat closed_hh", " ", ""); // Closed Hat
        channelTags[4].addTokens("oh open oph ohat hat_o hh_o oht openhat open_hh hho", " ", ""); // Open Hat
        channelTags[5].addTokens("ht hightom tomhi tom1 h-tom tom_hi conga_hi conga_high bongo_hi bnh cb cowbell bell cwb", " ", ""); // Perc1
        channelTags[6].addTokens("mt midtom tommid tom2 m-tom tom_mid conga_mid conga bongo_lo bol lt lowtom tomlow tom3 floor ftom l-tom", " ", ""); // Perc2
        channelTags[7].addTokens("cr crash cym cymbal crs china splash rd ride rid ride_bell", " ", ""); // Crash/Ride
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
    
    const int fftOrder = 11;
    const int fftSize = 1 << fftOrder; 
    
    juce::AudioBuffer<float> buffer(1, fftSize);
    buffer.clear();
    int numToRead = juce::jmin((int)reader->lengthInSamples, fftSize);
    
    if (numToRead < 256) return -1; 
    
    reader->read(&buffer, 0, numToRead, 0, true, false);
    const float* data = buffer.getReadPointer(0);
    
    float peak = 0.0f;
    int zeroCrossings = 0;
    
    for (int i = 0; i < numToRead; ++i) {
        if (std::abs(data[i]) > peak) peak = std::abs(data[i]);
    }
    
    if (peak < 0.01f) return -1; 
    
    float threshold = peak * 0.05f; 
    
    for (int i = 1; i < numToRead; ++i) {
        if ((data[i] > threshold && data[i-1] < -threshold) || 
            (data[i] < -threshold && data[i-1] > threshold)) {
            zeroCrossings++;
        }
    }
    
    float zcrRate = (float)zeroCrossings / (float)numToRead;
    
    juce::dsp::FFT fft(fftOrder);
    std::vector<float> fftData(fftSize * 2, 0.0f);
    
    juce::dsp::WindowingFunction<float> window(fftSize, juce::dsp::WindowingFunction<float>::hann);
    std::vector<float> windowedData(fftSize, 0.0f);
    for(int i = 0; i < numToRead; ++i) windowedData[i] = data[i];
    window.multiplyWithWindowingTable(windowedData.data(), fftSize);
    
    for (int i = 0; i < fftSize; ++i) fftData[i] = windowedData[i];
    
    fft.performFrequencyOnlyForwardTransform(fftData.data());
    
    float sumMagnitudes = 0.0f;
    float sumWeightedFrequencies = 0.0f;
    float sampleRate = reader->sampleRate > 0 ? reader->sampleRate : 44100.0f;
    
    int numBins = fftSize / 2;
    for (int i = 0; i < numBins; ++i) {
        float freq = ((float)i / (float)fftSize) * sampleRate;
        float mag = fftData[i];
        sumMagnitudes += mag;
        sumWeightedFrequencies += mag * freq;
    }
    
    float spectralCentroid = (sumMagnitudes > 0.0f) ? (sumWeightedFrequencies / sumMagnitudes) : 0.0f;
    
    if (spectralCentroid < 350.0f && zcrRate < 0.06f) {
        return 0; 
    }
    
    if (spectralCentroid >= 350.0f && spectralCentroid < 1200.0f) {
        return 8; 
    }
    
    if (spectralCentroid >= 1200.0f && spectralCentroid < 4500.0f) {
        if (zcrRate > 0.18f) return 4; 
        if (zcrRate < 0.08f) return 5; 
        return 1; 
    }
    
    if (spectralCentroid >= 4500.0f) {
        if (zcrRate > 0.25f) return 2; 
        if (reader->lengthInSamples > sampleRate * 0.8) return 10; 
        return 3; 
    }
    
    return -1; 
}