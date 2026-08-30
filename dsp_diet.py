with open('src/PluginProcessor.cpp', 'r') as f:
    code = f.read()

# Comment out Flanger
code = code.replace(
'''        if (flangerMix > 0.01f && fBufSize > 1) {''',
'''        if (false && flangerMix > 0.01f && fBufSize > 1) { // BYPASSED FOR HW DSP DIET''')

# Comment out Chorus
code = code.replace(
'''        if (chorusMix > 0.01f && cBufSize > 1) {''',
'''        if (false && chorusMix > 0.01f && cBufSize > 1) { // BYPASSED FOR HW DSP DIET''')

# Comment out Spring Reverb
code = code.replace(
'''        if (!springDelayL[0].empty()) {''',
'''        if (false && !springDelayL[0].empty()) { // BYPASSED FOR HW DSP DIET''')

# Now for the Channel Filters (Env/Tone) which are expensive
code = code.replace(
'''                if (chanTone[ch] && chanTone[ch]->load() != 0.5f) {
                    chanOutL = channelToneFilters[ch].processSample(0, chanOutL);
                    chanOutR = channelToneFilters[ch].processSample(0, chanOutR);
                }
                
                if (chanEnv[ch] && chanEnv[ch]->load() > 0.01f) {
                    float amt = chanEnv[ch]->load();
                    float mod = envFollower[ch] * amt * 4000.0f;
                    envFilterL[ch].setCutoffFrequency(juce::jlimit(20.0f, 20000.0f, 200.0f + mod));
                    envFilterR[ch].setCutoffFrequency(juce::jlimit(20.0f, 20000.0f, 200.0f + mod));
                    chanOutL = envFilterL[ch].processSample(0, chanOutL);
                    chanOutR = envFilterR[ch].processSample(0, chanOutR);
                }''',
'''                // FILTERS BYPASSED FOR HW DSP DIET
                // if (chanTone[ch]...)
                // if (chanEnv[ch]...)''')

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(code)
