import re

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

# Add thread_local variable
thread_local_code = """thread_local bool isBouncingThread = false;

void ExtasisRhythmProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {"""
cpp = cpp.replace('void ExtasisRhythmProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {', thread_local_code)

# Add protection at start of processBlock
protection_code = """    buffer.clear(); 
    if (getSampleRate() <= 0.0 || buffer.getNumChannels() == 0) return;

    if (isOfflineRendering.load() && !isBouncingThread) {
        return; // Mutear el thread de audio real mientras el thread de UI renderiza
    }"""
cpp = cpp.replace("""    buffer.clear(); 
    if (getSampleRate() <= 0.0 || buffer.getNumChannels() == 0) return;""", protection_code)


# Update renderOfflineLoop to set isBouncingThread
old_render = """    // Reset state for clean bounce
    isOfflineRendering.store(true);
    offlinePpqPosition.store(0.0);
    hostPlaying = true;"""
new_render = """    // Reset state for clean bounce
    isBouncingThread = true;
    isOfflineRendering.store(true);
    offlinePpqPosition.store(0.0);
    hostPlaying = true;"""
cpp = cpp.replace(old_render, new_render)


old_restore = """    // Restore state
    isOfflineRendering.store(false);
    hostPlaying = wasPlaying;"""
new_restore = """    // Restore state
    isOfflineRendering.store(false);
    isBouncingThread = false;
    hostPlaying = wasPlaying;"""
cpp = cpp.replace(old_restore, new_restore)


with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)

