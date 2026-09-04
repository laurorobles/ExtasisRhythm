import re

with open('src/PluginProcessor.h', 'r') as f:
    content = f.read()

content = content.replace('juce::String customSamplePaths[8];', 'juce::String customSamplePaths[12];')

with open('src/PluginProcessor.h', 'w') as f:
    f.write(content)

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

old_save = """    juce::String prefixes[] = {"01_Kick", "02_Snare", "03_CHH", "04_OHH", "05_Clap", "06_Tom", "07_Perc", "08_Crash"};

    for (int i = 0; i < 8; ++i) {"""

new_save = """    juce::String prefixes[] = {
        "01_Kick", "02_Snare", "03_CHH", "04_OHH", "05_Clap", "06_Rim", 
        "07_HiTom", "08_MidTom", "09_LowTom", "10_Cowbell", "11_Crash", "12_Ride"
    };

    for (int i = 0; i < 12; ++i) {"""

cpp = cpp.replace(old_save, new_save)

old_clear = """    // Clear custom paths since they are now part of a kit
    for (int i = 0; i < 8; ++i) customSamplePaths[i] = "";"""

new_clear = """    // Clear custom paths since they are now part of a kit
    for (int i = 0; i < 12; ++i) customSamplePaths[i] = "";"""

cpp = cpp.replace(old_clear, new_clear)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)
