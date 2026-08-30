with open('src/PluginProcessor.cpp', 'r') as f:
    code = f.read()

code = code.replace(
'''        if (currentElapsed >= maxDemoSamples) {
            demoExpired.store (true);
            return;
        } else {''',
'''        if (currentElapsed >= maxDemoSamples) {
            demoExpired.store (true);
            buffer.clear(); // MUTE THE AUDIO
            return;
        } else {'''
)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(code)
