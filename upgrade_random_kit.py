import re

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

old_randomize = r'void ExtasisRhythmProcessor::randomizeKit\(\) \{.*?juce::StringArray kickTokens, snareTokens;.*?\}'
# Actually I'll just rewrite the whole function. Let's extract everything inside randomizeKit
