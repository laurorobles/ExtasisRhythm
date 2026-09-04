import re

with open('src/PluginProcessor.h', 'r') as f:
    header = f.read()

if 'juce::File renderOfflineLoop' not in header:
    header = header.replace('void saveCustomKit (const juce::String& kitName);',
                            'void saveCustomKit (const juce::String& kitName);\n    bool renderOfflineLoop(const juce::File& outputFile);')
    with open('src/PluginProcessor.h', 'w') as f:
        f.write(header)

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

new_func = """
bool ExtasisRhythmProcessor::renderOfflineLoop(const juce::File& outputFile) {
    double renderSampleRate = 44100.0;
    double currentBpm = hostBpm.load();
    if (currentBpm <= 0) currentBpm = 120.0;
    
    // 32 steps = 8 beats (assuming 16th notes)
    double totalSeconds = 8.0 * (60.0 / currentBpm);
    int totalSamples = (int)(totalSeconds * renderSampleRate);
    
    juce::AudioBuffer<float> renderBuffer(2, totalSamples);
    renderBuffer.clear();

    // Save state
    bool wasPlaying = hostPlaying.load();
    double oldBpm = hostBpm.load();
    
    // Reset state for clean bounce
    hostPlaying = true;
    for (int i=0; i<12; ++i) {
        samplePositions[i] = -1.0;
        samplePositionsOld[i] = -1.0;
        currentMappedStep[i] = 0;
        flashCounters[i] = 0;
    }
    
    int blockSize = 512;
    int samplesRendered = 0;
    
    juce::MidiBuffer dummyMidi;
    
    while (samplesRendered < totalSamples) {
        int numToRender = std::min(blockSize, totalSamples - samplesRendered);
        juce::AudioBuffer<float> tempBuffer(2, numToRender);
        tempBuffer.clear();
        
        processBlock(tempBuffer, dummyMidi);
        
        for (int ch = 0; ch < 2; ++ch) {
            renderBuffer.copyFrom(ch, samplesRendered, tempBuffer, ch, 0, numToRender);
        }
        samplesRendered += numToRender;
    }
    
    // Restore state
    hostPlaying = wasPlaying;
    
    // Write to file
    if (outputFile.existsAsFile()) outputFile.deleteFile();
    
    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(new juce::FileOutputStream(outputFile), renderSampleRate, 2, 24, {}, 0)
    );
    
    if (writer != nullptr) {
        writer->writeFromAudioSampleBuffer(renderBuffer, 0, renderBuffer.getNumSamples());
        return true;
    }
    return false;
}
"""

if 'renderOfflineLoop' not in cpp:
    cpp += new_func
    with open('src/PluginProcessor.cpp', 'w') as f:
        f.write(cpp)
