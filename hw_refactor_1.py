import re

with open('src/PluginProcessor.h', 'r') as f:
    code = f.read()

# Remove FFT and Cache
code = re.sub(r'// --- SISTEMA DE CACHÉ MIR.*?(?=\n\n)', '', code, flags=re.DOTALL)
code = re.sub(r'juce::DynamicObject::Ptr analysisCacheObj;.*?(?=private:)', '', code, flags=re.DOTALL)
code = re.sub(r'int analyzeAudioFile.*?const;', '', code, flags=re.DOTALL)
code = re.sub(r'int analyzeAudioFile.*?;\n', '', code)
code = re.sub(r'void loadAnalysisCache.*?;\n', '', code)
code = re.sub(r'void saveAnalysisCache.*?;\n', '', code)

# Add category lists
code = code.replace(
'''    juce::StringArray channelTags[8];''',
'''    juce::StringArray channelTags[8];
    juce::Array<juce::File> categorizedFiles[8]; // Stores files for each category (0=Kick, 1=Snare...)
    std::atomic<int> currentSampleIndex[8];
    std::atomic<int> currentCategoryIndex[8];
    
    void loadNextSample(int channel, int direction);
    void changeCategory(int channel, int direction);
    void loadSampleFromCategory(int channel);
''')

with open('src/PluginProcessor.h', 'w') as f:
    f.write(code)

with open('src/PluginProcessor.cpp', 'r') as f:
    code = f.read()

# Remove Cache logic and FFT analysis from cpp
code = re.sub(r'void ExtasisRhythmProcessor::loadAnalysisCache.*?\}\n\n', '', code, flags=re.DOTALL)
code = re.sub(r'void ExtasisRhythmProcessor::saveAnalysisCache.*?\}\n\n', '', code, flags=re.DOTALL)
code = re.sub(r'int ExtasisRhythmProcessor::analyzeAudioFile.*?\}\n\n', '', code, flags=re.DOTALL)

# Refactor initialization
code = code.replace(
'''    for (int i = 0; i < 8; ++i) {
        lastSubStep[i] = -1;
        volSmoother[i].reset(getSampleRate(), 0.05);
''',
'''    for (int i = 0; i < 8; ++i) {
        currentSampleIndex[i].store(0);
        currentCategoryIndex[i].store(i); // Default: Ch 0 is Kick (cat 0), Ch 1 is Snare (cat 1)...
        lastSubStep[i] = -1;
        volSmoother[i].reset(getSampleRate(), 0.05);
''')

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(code)

