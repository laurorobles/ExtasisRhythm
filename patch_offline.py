import re

with open('src/PluginProcessor.h', 'r') as f:
    content = f.read()

if 'std::atomic<bool> isOfflineRendering { false };' not in content:
    content = content.replace('std::atomic<bool> hostPlaying { false };', 'std::atomic<bool> hostPlaying { false };\n    std::atomic<bool> isOfflineRendering { false };\n    std::atomic<double> offlinePpqPosition { 0.0 };')
    with open('src/PluginProcessor.h', 'w') as f:
        f.write(content)

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

# Replace getPlayHead() logic
playhead_old = """    if (auto* playHead = getPlayHead()) {
        if (auto pos = playHead->getPosition()) {
            currentHostPlaying = pos->getIsPlaying();
            if (pos->getBpm().hasValue()) currentHostBpm = *pos->getBpm();
            if (pos->getPpqPosition().hasValue() && currentHostPlaying) { 
                ppqPosition = *pos->getPpqPosition(); 
                hasHostTime = true; 
            }
        }
    }"""
playhead_new = """    if (isOfflineRendering.load()) {
        currentHostPlaying = true;
        hasHostTime = true;
        ppqPosition = offlinePpqPosition.load();
    } else if (auto* playHead = getPlayHead()) {
        if (auto pos = playHead->getPosition()) {
            currentHostPlaying = pos->getIsPlaying();
            if (pos->getBpm().hasValue()) currentHostBpm = *pos->getBpm();
            if (pos->getPpqPosition().hasValue() && currentHostPlaying) { 
                ppqPosition = *pos->getPpqPosition(); 
                hasHostTime = true; 
            }
        }
    }"""
cpp = cpp.replace(playhead_old, playhead_new)

# At the end of processBlock, increment offlinePpqPosition if offline
end_pb_old = """    cpuLoad = (float)(juce::Time::getHighResolutionTicks() - startTime) / (float)juce::Time::getHighResolutionTicksPerSecond();
}"""
end_pb_new = """    if (isOfflineRendering.load()) {
        double ppqPerSample = bpm / (60.0 * getSampleRate());
        offlinePpqPosition = offlinePpqPosition.load() + (ppqPerSample * buffer.getNumSamples());
    }
    cpuLoad = (float)(juce::Time::getHighResolutionTicks() - startTime) / (float)juce::Time::getHighResolutionTicksPerSecond();
}"""
cpp = cpp.replace(end_pb_old, end_pb_new)

# Modify renderOfflineLoop to set flags
render_old = """    // Reset state for clean bounce
    hostPlaying = true;"""
render_new = """    // Reset state for clean bounce
    isOfflineRendering.store(true);
    offlinePpqPosition.store(0.0);
    hostPlaying = true;"""
cpp = cpp.replace(render_old, render_new)

render_restore_old = """    // Restore state
    hostPlaying = wasPlaying;"""
render_restore_new = """    // Restore state
    isOfflineRendering.store(false);
    hostPlaying = wasPlaying;"""
cpp = cpp.replace(render_restore_old, render_restore_new)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)
