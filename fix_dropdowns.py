import re

with open('src/ChannelStripComponent.cpp', 'r') as f:
    cpp = f.read()

# Replace initialization of variant selector
old_init = r'auto initVariants = audioProcessor\.getVariantsForChannel \(initialChKit, channelIndex\);\s*sampleVariantSelector\.addItemList \(initVariants, 1\);\s*int initMatch = initVariants\.indexOf \(audioProcessor\.currentSampleName\[channelIndex\]\);\s*sampleVariantSelector\.setSelectedId \(initMatch >= 0 \? initMatch \+ 1 : 1, juce::dontSendNotification\);'
new_init = '''auto initVariants = audioProcessor.getVariantsForChannel (initialChKit, channelIndex);
    for (int i = 0; i < initVariants.size(); ++i) {
        sampleVariantSelector.addItem(initVariants[i].upToLastOccurrenceOf(".wav", false, true).upToLastOccurrenceOf(".WAV", false, true), i + 1);
    }
    int initMatch = initVariants.indexOf (audioProcessor.currentSampleName[channelIndex]);
    sampleVariantSelector.setSelectedId (initMatch >= 0 ? initMatch + 1 : 1, juce::dontSendNotification);'''

cpp = re.sub(old_init, new_init, cpp)

# Replace onChange for sampleSourceSelector
old_source_onchange = r'sampleSourceSelector\.onChange = \[this, chStr\] \{\s*int kitIdx = sampleSourceSelector\.getSelectedId\(\) - 1;\s*sampleVariantSelector\.clear \(juce::dontSendNotification\);\s*auto variants = audioProcessor\.getVariantsForChannel \(kitIdx, channelIndex\);\s*sampleVariantSelector\.addItemList \(variants, 1\);\s*int match = variants\.indexOf \(audioProcessor\.currentSampleName\[channelIndex\]\);\s*sampleVariantSelector\.setSelectedId \(match >= 0 \? match \+ 1 : 1, juce::dontSendNotification\);\s*\};'
new_source_onchange = '''sampleSourceSelector.onChange = [this, chStr] {
        int kitIdx = sampleSourceSelector.getSelectedId() - 1;
        sampleVariantSelector.clear (juce::dontSendNotification);
        auto variants = audioProcessor.getVariantsForChannel (kitIdx, channelIndex);
        for (int i = 0; i < variants.size(); ++i) {
            sampleVariantSelector.addItem(variants[i].upToLastOccurrenceOf(".wav", false, true).upToLastOccurrenceOf(".WAV", false, true), i + 1);
        }
        int match = variants.indexOf (audioProcessor.currentSampleName[channelIndex]);
        sampleVariantSelector.setSelectedId (match >= 0 ? match + 1 : 1, juce::dontSendNotification);
    };'''
cpp = re.sub(old_source_onchange, new_source_onchange, cpp)

# Replace onChange for sampleVariantSelector
old_variant_onchange = r'sampleVariantSelector\.onChange = \[this\] \{\s*int kitIdx = sampleSourceSelector\.getSelectedId\(\) - 1;\s*juce::String sName = sampleVariantSelector\.getText\(\);\s*if \(sName\.isNotEmpty\(\)\) \{\s*audioProcessor\.loadSampleForChannel \(channelIndex, kitIdx, sName\);\s*\}\s*\};'
new_variant_onchange = '''sampleVariantSelector.onChange = [this] {
        int kitIdx = sampleSourceSelector.getSelectedId() - 1;
        int varIdx = sampleVariantSelector.getSelectedId() - 1;
        auto variants = audioProcessor.getVariantsForChannel(kitIdx, channelIndex);
        if (varIdx >= 0 && varIdx < variants.size()) {
            audioProcessor.loadSampleForChannel(channelIndex, kitIdx, variants[varIdx]);
        }
    };'''
cpp = re.sub(old_variant_onchange, new_variant_onchange, cpp)

with open('src/ChannelStripComponent.cpp', 'w') as f:
    f.write(cpp)
