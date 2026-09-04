import re

with open('src/PluginProcessor.h', 'r') as f:
    header = f.read()
header = header.replace('void loadSampleForChannel (int i, int kit, const juce::String& baseName);',
                        'void loadSampleForChannel (int i, int kit, const juce::String& baseName);\n    void loadSampleFromAbsolutePath(int ch, const juce::String& absolutePath);')
with open('src/PluginProcessor.h', 'w') as f:
    f.write(header)

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

new_func = """void ExtasisRhythmProcessor::loadSampleFromAbsolutePath(int ch, const juce::String& absolutePath) {
    if (ch < 0 || ch >= 12) return;
    juce::File sample(absolutePath);
    if (sample.existsAsFile()) {
        auto* reader = formatManager.createReaderFor(sample);
        if (reader) {
            int numSamps = (int) reader->lengthInSamples;
            juce::AudioBuffer<float> tempBuffer ((int) reader->numChannels, numSamps);
            reader->read(&tempBuffer, 0, numSamps, 0, true, true);
            delete reader;
            
            float maxPeak = 0.0f;
            for (int c = 0; c < tempBuffer.getNumChannels(); ++c) {
                float channelPeak = tempBuffer.getMagnitude(c, 0, numSamps);
                if (channelPeak > maxPeak) maxPeak = channelPeak;
            }
            if (maxPeak > 0.0001f) {
                tempBuffer.applyGain(0.707f / maxPeak);
            }
            loadedSamples[ch] = tempBuffer;
            originalSampleRates[ch] = reader->sampleRate;
        }
    }
}
"""

cpp = cpp.replace('void ExtasisRhythmProcessor::loadSmartSampleForChannel(int i, int kit) {',
                  new_func + '\nvoid ExtasisRhythmProcessor::loadSmartSampleForChannel(int i, int kit) {')

# Modify loadSmartSampleForChannel
old_smart = "void ExtasisRhythmProcessor::loadSmartSampleForChannel(int i, int kit) {\n    if (kit < 0 || kit >= drumFolders.size()) return;"
new_smart = """void ExtasisRhythmProcessor::loadSmartSampleForChannel(int i, int kit) {
    if (customSamplePaths[i].isNotEmpty()) {
        loadSampleFromAbsolutePath(i, customSamplePaths[i]);
        return;
    }
    if (kit < 0 || kit >= drumFolders.size()) return;"""
cpp = cpp.replace(old_smart, new_smart)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)
