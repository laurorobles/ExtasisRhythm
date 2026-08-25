import re

with open('src/ChannelStripComponent.cpp', 'r') as f:
    cpp = f.read()

old_source_onchange = r'sampleSourceSelector\.onChange = \[this, chStr\] \{.*?sampleVariantSelector\.setSelectedId \(match >= 0 \? match \+ 1 : 1, juce::dontSendNotification\);\s*\};'

new_source_onchange = '''sampleSourceSelector.onChange = [this, chStr] {
        int kitIdx = sampleSourceSelector.getSelectedId() - 1;
        
        // 1. Force load a smart sample from the new folder so the sound immediately updates!
        audioProcessor.loadSmartSampleForChannel(channelIndex, kitIdx);
        
        // 2. Rebuild the GUI variant list without .wav
        sampleVariantSelector.clear (juce::dontSendNotification);
        auto variants = audioProcessor.getVariantsForChannel (kitIdx, channelIndex);
        for (int i = 0; i < variants.size(); ++i) {
            sampleVariantSelector.addItem(variants[i].upToLastOccurrenceOf(".wav", false, true).upToLastOccurrenceOf(".WAV", false, true), i + 1);
        }
        
        // 3. Select the correct newly loaded variant
        int match = variants.indexOf (audioProcessor.currentSampleName[channelIndex]);
        sampleVariantSelector.setSelectedId (match >= 0 ? match + 1 : 1, juce::dontSendNotification);
    };'''

cpp = re.sub(old_source_onchange, new_source_onchange, cpp, flags=re.DOTALL)

with open('src/ChannelStripComponent.cpp', 'w') as f:
    f.write(cpp)
