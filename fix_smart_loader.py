import re

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

old_smart = r'if \(!matchedVars\.isEmpty\(\)\) \{\s*int rIdx = juce::Random::getSystemRandom\(\)\.nextInt\(matchedVars\.size\(\)\);\s*loadSampleForChannel\(i, kit, matchedVars\[rIdx\]\);\s*\}'
new_smart = '''if (!matchedVars.isEmpty()) {
        int rIdx = juce::Random::getSystemRandom().nextInt(matchedVars.size());
        loadSampleForChannel(i, kit, matchedVars[rIdx]);
    } else {
        // FALLBACK: If no keyword matches, just load a random sample from this kit to avoid desync!
        int rIdx = juce::Random::getSystemRandom().nextInt(allVariants.size());
        loadSampleForChannel(i, kit, allVariants[rIdx]);
    }'''

cpp = re.sub(old_smart, new_smart, cpp, flags=re.DOTALL)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)
