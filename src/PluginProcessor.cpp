#include "PluginProcessor.h"
#include "PluginEditor.h"

ExtasisRhythmProcessor::ExtasisRhythmProcessor()
    : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    for (int ch = 0; ch < 10; ++ch) { flashCounters[ch] = 0; samplePositions[ch] = -1.0; channelVelocities[ch] = 1.0f; channelSteps[ch] = 0; }
    formatManager.registerBasicFormats();
    juce::File samplesFolder ("/Users/babyonk1/Desktop/ExtasisRhythm/Samples");
    samplesFolder.findChildFiles (drumFolders, juce::File::findDirectories, true);
    flangerBufferL.resize (88200, 0.0f); flangerBufferR.resize (88200, 0.0f);
}

juce::AudioProcessorValueTreeState::ParameterLayout ExtasisRhythmProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("isPlaying", 1), "Play", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("bpm", 1), "BPM", 40.0f, 240.0f, 120.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterVolume", 1), "Master", 0.0f, 2.0f, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("masterClipper", 1), "Limit", true));
    params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("globalKitChoice", 1), "Kit", 0, 10, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterHpf", 1), "HPF", 20.0f, 2000.0f, 20.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterHpfRes", 1), "HRes", 0.1f, 10.0f, 0.7f));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID ("masterHpfSlope", 1), "HSlope", juce::StringArray { "12dB", "24dB" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterLpf", 1), "LPF", 500.0f, 20000.0f, 20000.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("masterLpfRes", 1), "LRes", 0.1f, 10.0f, 0.7f));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID ("masterLpfSlope", 1), "LSlope", juce::StringArray { "12dB", "24dB" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pcmBits", 1), "Bits", 4.0f, 16.0f, 16.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pcmRate", 1), "Rate", 1.0f, 16.0f, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("flangerOn", 1), "FlangOn", false));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("flangerRate", 1), "FRate", 0.1f, 10.0f, 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("flangerFeedback", 1), "FFB", -0.9f, 0.9f, 0.3f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("springDecay", 1), "SDec", 0.1f, 1.0f, 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("springTone", 1), "STon", 500.0f, 10000.0f, 3500.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("transientAttack", 1), "Att", -1.0f, 1.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("transientSustain", 1), "Sus", -1.0f, 1.0f, 0.0f));

    for (int step = 0; step < 16; ++step)
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("fill_step_" + juce::String(step), 1), "Fill " + juce::String(step+1), 0, 2, 0));

    for (int i = 0; i < 10; ++i) {
        juce::String chStr = juce::String (i);
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("sampleSource_" + chStr, 1), "Source", 0, 10, 0));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("gain" + chStr, 1), "Gain", 0.0f, 1.0f, 0.8f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pan" + chStr, 1), "Pan", -1.0f, 1.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("pitch" + chStr, 1), "Pitch", 0.25f, 4.0f, 1.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("tone" + chStr, 1), "Tone", 0.0f, 1.0f, 0.5f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("attack" + chStr, 1), "Attack", 0.0f, 0.5f, 0.001f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("decay" + chStr, 1), "Decay", 0.01f, 5.0f, 1.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID ("springSend" + chStr, 1), "Send", 0.0f, 1.0f, 0.0f));
        params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("length" + chStr, 1), "Length", 1, 16, 16));
        for (int step = 0; step < 16; ++step)
            params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("step_" + chStr + "_" + juce::String(step), 1), "Step", 0, 3, 0));
    }
    return { params.begin(), params.end() };
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

void ExtasisRhythmProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    currentSampleRate = sampleRate; currentSamplesPerBlock = samplesPerBlock; stepPhase = 0.0;
    juce::dsp::ProcessSpec spec; spec.sampleRate = sampleRate; spec.maximumBlockSize = (unsigned int)samplesPerBlock; spec.numChannels = 2;
    masterHpf.prepare (spec); masterLpf.prepare (spec); springToneFilter.prepare (spec);
    springReverb.setSampleRate (sampleRate);
    envFastL = envSlowL = envFastR = envSlowR = 0.0f;
    flangerBufferL.assign (sampleRate * 2, 0.0f); flangerBufferR.assign (sampleRate * 2, 0.0f);
    loadGlobalDrumKit ((int)*apvts.getRawParameterValue ("globalKitChoice"));
}

juce::StringArray ExtasisRhythmProcessor::getDrumKitNames() const {
    juce::StringArray names; for (auto& f : drumFolders) names.add (f.getFileName()); return names;
}

void ExtasisRhythmProcessor::loadSampleForChannel (int ch, int kit) {
    if (ch < 0 || ch >= 10 || drumFolders.isEmpty()) return;
    juce::File selKit = drumFolders[juce::jlimit (0, drumFolders.size() - 1, kit)];
    juce::String files[10] = { "bd.wav", "sd.wav", "ch.wav", "oh.wav", "cp.wav", "cb.wav", "rs.wav", "ht.wav", "mt.wav", "lt.wav" };
    juce::File sample = selKit.getChildFile (files[ch]);
    if (!sample.existsAsFile()) {
        juce::Array<juce::File> wavs; selKit.findChildFiles (wavs, juce::File::findFiles, false, "*.wav");
        if (ch < wavs.size()) sample = wavs[ch];
    }
    if (sample.existsAsFile()) {
        auto* reader = formatManager.createReaderFor (sample);
        if (reader) {
            readerSources[ch] = std::make_unique<juce::AudioFormatReaderSource> (reader, true);
            readerSources[ch]->prepareToPlay (currentSamplesPerBlock, currentSampleRate);
        }
    }
}

void ExtasisRhythmProcessor::loadGlobalDrumKit (int kit) {
    isLoadingKits = true;
    for (int i = 0; i < 10; ++i) {
        loadSampleForChannel (i, kit);
        if (auto* p = apvts.getParameter ("sampleSource_" + juce::String(i))) {
            p->beginChangeGesture(); p->setValueNotifyingHost ((float)kit / (float)juce::jmax(1, drumFolders.size() - 1)); p->endChangeGesture();
        }
    }
    isLoadingKits = false;
}

void ExtasisRhythmProcessor::triggerChannel (int ch, float vel) {
    if (ch >= 0 && ch < 10 && readerSources[ch] != nullptr) {
        samplePositions[ch] = 0.0; channelVelocities[ch] = vel; flashCounters[ch] = 8;
    }
}

void ExtasisRhythmProcessor::saveCustomPreset (const juce::File& f) {
    std::unique_ptr<juce::XmlElement> xml (apvts.copyState().createXml()); xml->writeTo (f);
}

void ExtasisRhythmProcessor::loadCustomPreset (const juce::File& f) {
    std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse (f));
    if (xml && xml->hasTagName (apvts.state.getType())) apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

void ExtasisRhythmProcessor::resetAllParameters() {
    for (auto* p : getParameters()) {
        juce::String name = p->getName (100);
        if (!name.contains ("Source") && !name.contains ("Kit")) {
            p->beginChangeGesture(); p->setValueNotifyingHost (p->getDefaultValue()); p->endChangeGesture();
        }
    }
}

void ExtasisRhythmProcessor::releaseResources() { for (int i = 0; i < 10; ++i) readerSources[i].reset(); }

#ifndef JucePlugin_PreferredChannelConfigurations
bool ExtasisRhythmProcessor::isBusesLayoutSupported (const BusesLayout& l) const { return l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo(); }
#endif

void ExtasisRhythmProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    for (int i = 0; i < buffer.getNumChannels(); ++i) buffer.clear (i, 0, buffer.getNumSamples());
    if (isLoadingKits.load()) return;

    for (int i = 0; i < 10; ++i) { int c = flashCounters[i].load(); if (c > 0) flashCounters[i] = c - 1; }
    for (const auto meta : midiMessages) { auto msg = meta.getMessage(); if (msg.isNoteOn()) triggerChannel (msg.getNoteNumber() - 36, msg.getFloatVelocity()); }

    bool playing = apvts.getRawParameterValue ("isPlaying")->load() > 0.5f;
    double bpm = apvts.getRawParameterValue ("bpm")->load();
    if (auto* ph = getPlayHead()) {
        if (auto pos = ph->getPosition()) {
            if (pos->getIsPlaying()) playing = true;
            if (auto b = pos->getBpm()) bpm = *b;
        }
    }

    double phaseInc = (bpm / 60.0 * 4.0) / currentSampleRate;
    juce::AudioBuffer<float> sendBuffer (2, buffer.getNumSamples()); sendBuffer.clear();

    double localPos[10]; float localVel[10];
    for (int i = 0; i < 10; ++i) { localPos[i] = samplePositions[i].load(); localVel[i] = channelVelocities[i].load(); }

    for (int s = 0; s < buffer.getNumSamples(); ++s) {
        if (playing) {
            stepPhase += phaseInc;
            if (stepPhase >= 1.0) {
                stepPhase -= 1.0;
                for (int ch = 0; ch < 10; ++ch) {
                    int maxLen = (int) apvts.getRawParameterValue ("length" + juce::String(ch))->load();
                    int cur = channelSteps[ch].load(); int next = (cur + 1) % maxLen; channelSteps[ch] = next;
                    int stepV = (int) apvts.getRawParameterValue ("step_" + juce::String(ch) + "_" + juce::String(next))->load();
                    if (stepV > 0) {
                        localPos[ch] = 0.0; localVel[ch] = (stepV == 1 ? 0.4f : (stepV == 2 ? 0.7f : 1.0f)); flashCounters[ch] = 8;
                    }
                }
            }
        }

        for (int i = 0; i < 10; ++i) {
            if (localPos[i] >= 0.0 && readerSources[i] != nullptr) {
                auto* reader = readerSources[i]->getAudioFormatReader();
                if (!reader) continue;
                float pitch = apvts.getRawParameterValue ("pitch" + juce::String(i))->load();
                float attSec = apvts.getRawParameterValue ("attack" + juce::String(i))->load();
                float decSec = apvts.getRawParameterValue ("decay" + juce::String(i))->load();
                float vol = apvts.getRawParameterValue ("gain" + juce::String(i))->load();
                float pan = apvts.getRawParameterValue ("pan" + juce::String(i))->load();
                float send = apvts.getRawParameterValue ("springSend" + juce::String(i))->load();

                float totalSam = (float)reader->lengthInSamples * decSec;
                float attSam = attSec * (float)currentSampleRate;

                if (localPos[i] >= (double)reader->lengthInSamples || localPos[i] >= (double)totalSam) { localPos[i] = -1.0; continue; }

                juce::AudioBuffer<float> tmp ((int)reader->numChannels, 1);
                reader->read (&tmp, 0, 1, (int)localPos[i], true, true);
                float sL = tmp.getSample (0, 0); float sR = tmp.getNumChannels() > 1 ? tmp.getSample (1, 0) : sL;

                float env = 1.0f; float curS = (float)localPos[i];
                if (curS < 64.0f) env = curS / 64.0f;
                else if (curS < attSam && attSam > 0.0f) env = curS / attSam;
                else if (curS > totalSam * 0.7f) env = juce::jmax (0.0f, 1.0f - (curS - totalSam * 0.7f) / (totalSam * 0.3f));

                sL *= env; sR *= env;
                float gF = 0.2f * vol * localVel[i];
                float outL = sL * std::sqrt (0.5f * (1.0f - pan));
                float outR = sR * std::sqrt (0.5f * (1.0f + pan));

                buffer.addSample (0, s, outL * gF);
                if (buffer.getNumChannels() > 1) buffer.addSample (1, s, outR * gF); else buffer.addSample (0, s, outL * gF);
                if (send > 0.0f) { sendBuffer.addSample (0, s, outL * gF * send); sendBuffer.addSample (1, s, outR * gF * send); }
                localPos[i] += (double)pitch;
            }
        }
    }

    for (int i = 0; i < 10; ++i) { samplePositions[i].store (localPos[i]); channelVelocities[i].store (localVel[i]); }

    // PCM
    float quant = std::pow (2.0f, apvts.getRawParameterValue ("pcmBits")->load());
    int rateRed = (int) apvts.getRawParameterValue ("pcmRate")->load();
    for (int s = 0; s < buffer.getNumSamples(); ++s) {
        if (++pcmCounter >= rateRed) {
            pcmCounter = 0;
            pcmHoldSampleL = std::floor (buffer.getSample (0, s) * quant) / quant;
            pcmHoldSampleR = buffer.getNumChannels() > 1 ? std::floor (buffer.getSample (1, s) * quant) / quant : pcmHoldSampleL;
        }
        buffer.setSample (0, s, pcmHoldSampleL);
        if (buffer.getNumChannels() > 1) buffer.setSample (1, s, pcmHoldSampleR);
    }

    // Flanger (con interruptor On/Off)
    if (apvts.getRawParameterValue ("flangerOn")->load() > 0.5f) {
        float fRate = apvts.getRawParameterValue ("flangerRate")->load();
        float fFb = apvts.getRawParameterValue ("flangerFeedback")->load();
        int bSize = (int)flangerBufferL.size();
        for (int s = 0; s < buffer.getNumSamples(); ++s) {
            flangerLfoPhase += (double)fRate / currentSampleRate; if (flangerLfoPhase >= 1.0) flangerLfoPhase -= 1.0;
            float lfo = 0.5f * (1.0f + std::sin (2.0 * juce::MathConstants<double>::pi * flangerLfoPhase));
            float dSam = (1.0f + lfo * 6.0f) * (float)(currentSampleRate / 1000.0);
            int rPos = (flangerWritePos - (int)dSam + bSize) % bSize; int rPos1 = (rPos + 1) % bSize;
            float frac = dSam - (float)((int)dSam);
            float dL = flangerBufferL[rPos] + frac * (flangerBufferL[rPos1] - flangerBufferL[rPos]);
            float dR = flangerBufferR[rPos] + frac * (flangerBufferR[rPos1] - flangerBufferR[rPos]);
            float inL = buffer.getSample (0, s); float inR = buffer.getNumChannels() > 1 ? buffer.getSample (1, s) : inL;
            buffer.setSample (0, s, (inL + dL) * 0.707f);
            if (buffer.getNumChannels() > 1) buffer.setSample (1, s, (inR + dR) * 0.707f);
            flangerBufferL[flangerWritePos] = inL + dL * fFb; flangerBufferR[flangerWritePos] = inR + dR * fFb;
            flangerWritePos = (flangerWritePos + 1) % bSize;
        }
    }

    // Spring Reverb
    juce::Reverb::Parameters rp; rp.roomSize = apvts.getRawParameterValue ("springDecay")->load(); rp.damping = 0.4f; rp.wetLevel = 1.0f; rp.dryLevel = 0.0f; rp.width = 1.0f;
    springReverb.setParameters (rp);
    *springToneFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass (currentSampleRate, apvts.getRawParameterValue ("springTone")->load(), 1.2f);
    juce::dsp::AudioBlock<float> sb (sendBuffer); juce::dsp::ProcessContextReplacing<float> sc (sb); springToneFilter.process (sc);
    springReverb.processStereo (sendBuffer.getWritePointer(0), sendBuffer.getWritePointer(1), sendBuffer.getNumSamples());
    for (int s = 0; s < buffer.getNumSamples(); ++s) {
        buffer.addSample (0, s, sendBuffer.getSample(0, s) * 0.5f);
        if (buffer.getNumChannels() > 1) buffer.addSample (1, s, sendBuffer.getSample(1, s) * 0.5f);
    }

    // Master Filters & Volume/Clipper
    *masterHpf.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (currentSampleRate, apvts.getRawParameterValue ("masterHpf")->load(), apvts.getRawParameterValue ("masterHpfRes")->load());
    *masterLpf.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass (currentSampleRate, apvts.getRawParameterValue ("masterLpf")->load(), apvts.getRawParameterValue ("masterLpfRes")->load());
    juce::dsp::AudioBlock<float> bl (buffer); juce::dsp::ProcessContextReplacing<float> cx (bl);
    masterHpf.process (cx); masterLpf.process (cx);

    float mVol = apvts.getRawParameterValue ("masterVolume")->load();
    bool clipper = apvts.getRawParameterValue ("masterClipper")->load() > 0.5f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        auto* d = buffer.getWritePointer (ch);
        for (int s = 0; s < buffer.getNumSamples(); ++s) {
            d[s] *= mVol;
            if (clipper) d[s] = juce::jlimit (-1.0f, 1.0f, std::tanh (d[s]));
        }
    }

    outputLevelL = buffer.getRMSLevel (0, 0, buffer.getNumSamples());
    outputLevelR = buffer.getNumChannels() > 1 ? buffer.getRMSLevel (1, 0, buffer.getNumSamples()) : outputLevelL.load();
}

bool ExtasisRhythmProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* ExtasisRhythmProcessor::createEditor() { return new ExtasisRhythmEditor (*this); }
void ExtasisRhythmProcessor::getStateInformation (juce::MemoryBlock& dest) { std::unique_ptr<juce::XmlElement> xml (apvts.copyState().createXml()); copyXmlToBinary (*xml, dest); }
void ExtasisRhythmProcessor::setStateInformation (const void* data, int size) {
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, size));
    if (xml && xml->hasTagName (apvts.state.getType())) apvts.replaceState (juce::ValueTree::fromXml (*xml));
    else resetAllParameters();
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new ExtasisRhythmProcessor(); }
