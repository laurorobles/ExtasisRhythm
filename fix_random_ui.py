import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

old_click = r'randomKitButton\.onClick = \[this\] \{.*?^\s*\};\n'
new_click = '''    randomKitButton.onClick = [this] {
        audioProcessor.randomizeKit();
        for (int i = 0; i < 12; ++i) { 
            juce::String chStr = juce::String(i);
            int chKitIdx = (int)audioProcessor.apvts.getRawParameterValue("sampleSource_" + chStr)->load();
            channelStrips[i]->sampleVariantSelector.clear(juce::dontSendNotification);
            auto variants = audioProcessor.getVariantsForChannel(chKitIdx, i); 
            channelStrips[i]->sampleVariantSelector.addItemList(variants, 1);
            int matchIdx = variants.indexOf(audioProcessor.currentSampleName[i]); 
            channelStrips[i]->sampleVariantSelector.setSelectedId(matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
        }
    };
'''

cpp = re.sub(old_click, new_click, cpp, flags=re.DOTALL|re.MULTILINE)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
