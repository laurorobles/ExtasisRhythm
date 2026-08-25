import re

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

old_loop = r'for \(int s = 0; s < buffer\.getNumSamples\(\); \+\+s\) \{\s*smoothedDelayTime = smoothedDelayTime \* 0\.995f \+ targetDelSamples \* 0\.005f;'
new_loop = '''for (int s = 0; s < buffer.getNumSamples(); ++s) {
        smoothedDelayTime = smoothedDelayTime * 0.995f + targetDelSamples * 0.005f;

        for (int i = 0; i < 12; ++i) {
            chanVol[i] = volSmoother[i].getNextValue();
            chanPan[i] = panSmoother[i].getNextValue();
            float semitones = pitchSmoother[i].getNextValue();
            chanPitch[i] = (float)std::pow(2.0, (double)semitones / 12.0) * ((localSamples[i] && localSamples[i]->sampleRate > 0) ? (localSamples[i]->sampleRate / getSampleRate()) : 1.0);
        }'''

cpp = re.sub(old_loop, new_loop, cpp)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)
