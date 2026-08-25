import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# 1. Restore randomKitButton
# Find where it was originally initialized and add it back to the constructor.
# It should be around globalKitSelector or seqToggleViewButton.
init_marker = r'addAndMakeVisible \(seqToggleViewButton\);'
init_replacement = '''addAndMakeVisible (seqToggleViewButton);
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
    cpp = re.sub(init_marker, init_replacement, cpp)

# 2. Fix VU Meter Repaint
timer_marker = r'repaint \(0, 452, getWidth\(\), getHeight\(\) - 452\);'
timer_replacement = '''repaint (0, 452, getWidth(), getHeight() - 452); 
    
    // Repaint master section (VU Meter & CPU)
    int masterX = 1192 - 16 - 146;
    float s = (float)getWidth() / 1192.0f;
    repaint((int)(masterX * s), (int)(15 * s), (int)(146 * s), (int)(160 * s));'''
if 'Repaint master section' not in cpp:
    cpp = re.sub(timer_marker, timer_replacement, cpp)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)

with open('src/PluginEditor.h', 'r') as f:
    h = f.read()

if 'juce::TextButton randomKitButton;' not in h:
    h = re.sub(r'juce::TextButton seqToggleViewButton;', r'juce::TextButton seqToggleViewButton;\n    juce::TextButton randomKitButton;', h)
    with open('src/PluginEditor.h', 'w') as f:
        f.write(h)
