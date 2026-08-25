import re

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

# 1. Update the block-level loading
old_load = r'chanVol\[i\] = cachedParams\.chanGain\[i\]->load\(\); chanPan\[i\] = cachedParams\.chanPan\[i\]->load\(\);\s*float basePitchParam = cachedParams\.chanPitch\[i\]->load\(\);\s*double srRatio = localSamples\[i\]->sampleRate / getSampleRate\(\);\s*chanPitch\[i\] = std::pow \(2\.0f, \(basePitchParam \+ channelStepSemitones\[i\]\.load\(\)\) / 12\.0f\) \* pcmClockRatio \* \(float\)srRatio;'

new_load = '''volSmoother[i].setTargetValue(cachedParams.chanGain[i]->load());
        panSmoother[i].setTargetValue(cachedParams.chanPan[i]->load());
        pitchSmoother[i].setTargetValue(cachedParams.chanPitch[i]->load() + channelStepSemitones[i].load());
        
        // We still keep the arrays for anything that isn't per-sample, but we will overwrite them in the sample loop
        double srRatio = localSamples[i]->sampleRate / getSampleRate();'''

cpp = re.sub(old_load, new_load, cpp, flags=re.DOTALL)

# 2. Inside the sample loop, update the per-sample variables
# Look for: for (int s = 0; s < numSamples; ++s) {
old_sample_loop = r'(for \(int s = 0; s < numSamples; \+\+s\) \{)'
new_sample_loop = r'''\1
        float curVol[12], curPan[12], curPitch[12];
        for(int i=0; i<12; ++i) {
            curVol[i] = volSmoother[i].getNextValue();
            curPan[i] = panSmoother[i].getNextValue();
            double srRatio = localSamples[i] ? (localSamples[i]->sampleRate / getSampleRate()) : 1.0;
            curPitch[i] = std::pow(2.0f, pitchSmoother[i].getNextValue() / 12.0f) * pcmClockRatio * (float)srRatio;
        }'''
cpp = re.sub(old_sample_loop, new_sample_loop, cpp)

# 3. Replace usage of chanVol[i], chanPan[i], chanPitch[i] inside the sample loop with curVol[i], curPan[i], curPitch[i]
# But wait, chanVol is used outside the loop? No, it was a local array declared at block scope!
# Let's replace chanVol -> curVol, chanPan -> curPan, chanPitch -> curPitch inside the sample loop block.
# I'll just write a quick script logic for this.

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)
