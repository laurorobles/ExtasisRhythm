import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

old_random = r'randomKitButton\.onClick = \[this\] \{\s*audioProcessor\.randomizeKit\(\);\s*// APVTS ComboBoxAttachment will automatically update sampleSourceSelector\.\s*// sampleSourceSelector\.onChange will automatically update sampleVariantSelector\.\s*\};'

new_random = '''randomKitButton.onClick = [this] {
        audioProcessor.randomizeKit();
        for (int i = 0; i < 12; ++i) { 
            int chKitIdx = (int)audioProcessor.apvts.getRawParameterValue("sampleSource_" + juce::String(i))->load();
            channelStrips[i]->sampleSourceSelector.setSelectedId(chKitIdx + 1, juce::dontSendNotification);
            
            channelStrips[i]->sampleVariantSelector.clear(juce::dontSendNotification);
            auto variants = audioProcessor.getVariantsForChannel(chKitIdx, i); 
            for (int j = 0; j < variants.size(); ++j) {
                channelStrips[i]->sampleVariantSelector.addItem(variants[j].upToLastOccurrenceOf(".wav", false, true).upToLastOccurrenceOf(".WAV", false, true), j + 1);
            }
            int matchIdx = variants.indexOf(audioProcessor.currentSampleName[i]); 
            channelStrips[i]->sampleVariantSelector.setSelectedId(matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
        }
    };'''

cpp = re.sub(old_random, new_random, cpp)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
