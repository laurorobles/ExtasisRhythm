import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

old_random = r'randomKitButton\.onClick = \[this\] \{\s*audioProcessor\.randomizeKit\(\);\s*for \(int i = 0; i < 12; \+\+i\) \{ \s*juce::String chStr = juce::String\(i\);\s*int chKitIdx = \(int\)audioProcessor\.apvts\.getRawParameterValue\("sampleSource_" \+ chStr\)->load\(\);\s*channelStrips\[i\]->sampleVariantSelector\.clear\(juce::dontSendNotification\);\s*auto variants = audioProcessor\.getVariantsForChannel\(chKitIdx, i\); \s*for \(int j = 0; j < variants\.size\(\); \+\+j\) \{\s*channelStrips\[i\]->sampleVariantSelector\.addItem\(variants\[j\]\.upToLastOccurrenceOf\("\.wav", false, true\)\.upToLastOccurrenceOf\("\.WAV", false, true\), j \+ 1\);\s*\}\s*int matchIdx = variants\.indexOf\(audioProcessor\.currentSampleName\[i\]\); \s*channelStrips\[i\]->sampleVariantSelector\.setSelectedId\(matchIdx >= 0 \? matchIdx \+ 1 : 1, juce::dontSendNotification\);\s*\}\s*\};'

new_random = '''randomKitButton.onClick = [this] {
        audioProcessor.randomizeKit();
        // APVTS ComboBoxAttachment will automatically update sampleSourceSelector.
        // sampleSourceSelector.onChange will automatically update sampleVariantSelector.
    };'''

cpp = re.sub(old_random, new_random, cpp)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
