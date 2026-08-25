import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

old_timer = r'for \(int i = 0; i < 12; \+\+i\) \{ juce::String chStr = juce::String\(i\); \s*if \(audioProcessor\.flashCounters\[i\]\.load\(\) > 0\)\s*channelStrips\[i\]->ledButton->repaint\(\);\s*\}'
new_timer = '''for (int i = 0; i < 12; ++i) { 
        bool isLit = audioProcessor.flashCounters[i].load() > 0;
        if (isLit != lastLitState[i]) {
            channelStrips[i]->ledButton->repaint();
            lastLitState[i] = isLit;
        }
    }'''

cpp = re.sub(old_timer, new_timer, cpp)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)

with open('src/PluginEditor.h', 'r') as f:
    h = f.read()

if 'bool lastLitState[12]' not in h:
    h = re.sub(r'ExtasisRhythmProcessor\& audioProcessor;', r'ExtasisRhythmProcessor& audioProcessor;\n    bool lastLitState[12] = {false};', h)
    with open('src/PluginEditor.h', 'w') as f:
        f.write(h)
