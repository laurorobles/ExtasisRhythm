import re

with open('src/PluginProcessor.cpp', 'r') as f:
    content = f.read()

old_block = """            if (maxPeak > 0.0001f) {
                tempBuffer.applyGain(0.707f / maxPeak);
            }
            loadedSamples[ch] = tempBuffer;
            originalSampleRates[ch] = reader->sampleRate;"""

new_block = """            if (maxPeak > 0.0001f) {
                tempBuffer.applyGain(0.707f / maxPeak);
            }
            double fileSr = reader->sampleRate;
            auto newBuf = new SampleBuffer(std::move(tempBuffer), fileSr > 0.0 ? fileSr : 44100.0);
            {
                juce::SpinLock::ScopedLockType sl(pointerLock);
                sampleBuffers[ch] = newBuf;
            }
            currentSampleName.set(ch, sample.getFileName());"""

content = content.replace(old_block, new_block)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(content)
