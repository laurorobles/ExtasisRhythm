import re

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

old_logic = r'if \(!matchKicks\.isEmpty\(\)\) \{\s*int rIdx = juce::Random::getSystemRandom\(\)\.nextInt\(matchKicks\.size\(\)\);\s*chosenSample = matchKicks\[rIdx\];\s*\}'
new_logic = '''if (!matchKicks.isEmpty()) {
                    int rIdx = juce::Random::getSystemRandom().nextInt(matchKicks.size());
                    chosenSample = matchKicks[rIdx];
                } else if (attempts == 9) { // Fallback on last attempt
                    int rIdx = juce::Random::getSystemRandom().nextInt(variants.size());
                    chosenSample = variants[rIdx];
                }'''
cpp = re.sub(old_logic, new_logic, cpp)

old_snare_logic = r'if \(!matchSnares\.isEmpty\(\)\) \{\s*int rIdx = juce::Random::getSystemRandom\(\)\.nextInt\(matchSnares\.size\(\)\);\s*chosenSample = matchSnares\[rIdx\];\s*\}'
new_snare_logic = '''if (!matchSnares.isEmpty()) {
                    int rIdx = juce::Random::getSystemRandom().nextInt(matchSnares.size());
                    chosenSample = matchSnares[rIdx];
                } else if (attempts == 9) { // Fallback on last attempt
                    int rIdx = juce::Random::getSystemRandom().nextInt(variants.size());
                    chosenSample = variants[rIdx];
                }'''
cpp = re.sub(old_snare_logic, new_snare_logic, cpp)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)
