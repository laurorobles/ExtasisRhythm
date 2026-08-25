import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

old_random_btn = r'randomKitButton\.onClick = \[this\] \{.*?^\s*\};\n'
new_random_btn = '''    randomKitButton.onClick = [this] {
        audioProcessor.randomizeKit();
        for (int i = 0; i < 12; ++i) { 
            juce::String chStr = juce::String(i);
            int chKitIdx = (int)audioProcessor.apvts.getRawParameterValue("sampleSource_" + chStr)->load();
            channelStrips[i]->sampleVariantSelector.clear(juce::dontSendNotification);
            auto variants = audioProcessor.getVariantsForChannel(chKitIdx, i); 
            for (int j = 0; j < variants.size(); ++j) {
                channelStrips[i]->sampleVariantSelector.addItem(variants[j].upToLastOccurrenceOf(".wav", false, true).upToLastOccurrenceOf(".WAV", false, true), j + 1);
            }
            int matchIdx = variants.indexOf(audioProcessor.currentSampleName[i]); 
            channelStrips[i]->sampleVariantSelector.setSelectedId(matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
        }
    };
'''

cpp = re.sub(old_random_btn, new_random_btn, cpp, flags=re.DOTALL|re.MULTILINE)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
