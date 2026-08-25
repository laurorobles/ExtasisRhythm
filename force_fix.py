import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

replacement = '''addAndMakeVisible (seqToggleViewButton);
    addAndMakeVisible (randomKitButton);
    randomKitButton.setButtonText ("RANDOM KIT"); 
    randomKitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff8e44ad)); 
    randomKitButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    randomKitButton.onClick = [this] {
        audioProcessor.randomizeKit();
        for (int i = 0; i < 12; ++i) { 
            int chKitIdx = (int)(audioProcessor.apvts.getRawParameterValue("sampleSource_" + juce::String(i))->load() + 0.5f);
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

if 'randomKitButton.setButtonText' not in cpp:
    cpp = cpp.replace('addAndMakeVisible (seqToggleViewButton);', replacement, 1)
    with open('src/PluginEditor.cpp', 'w') as f:
        f.write(cpp)
        print("Success")
else:
    print("Already there")
