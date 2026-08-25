import re

with open('src/PluginEditor.h', 'r') as f:
    h = f.read()

# re-add fitButtons[12]
if 'juce::TextButton fitButtons[12];' not in h:
    h = h.replace('juce::TextButton fillFitButton;', 'juce::TextButton fillFitButton;\n    juce::TextButton fitButtons[12];\n    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fitAtts[12];')

with open('src/PluginEditor.h', 'w') as f:
    f.write(h)


with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# Fix repaint
cpp = cpp.replace('channelLedButtons[i]->repaint (0, 452, getWidth(), getHeight() - 452);', 'channelStrips[i]->ledButton->repaint();')

# Fix fitButtons in PluginEditor
cpp = cpp.replace('channelStrips[seqIdx]->fitButton.', 'fitButtons[seqIdx].')

# In constructor, add fitButtons back
constructor_regex = r'channelStrips\[i\] = std::make_unique<ChannelStripComponent>\(audioProcessor, i, &safeComboBoxLAF, &masterChannelKnobLAF\);\s*addAndMakeVisible\(\*channelStrips\[i\]\);'
new_constructor = '''channelStrips[i] = std::make_unique<ChannelStripComponent>(audioProcessor, i, &safeComboBoxLAF, &masterChannelKnobLAF);
        addAndMakeVisible(*channelStrips[i]);
        
        juce::String chStr = juce::String(i);
        addAndMakeVisible (fitButtons[i]); 
        fitButtons[i].setLookAndFeel (&compactBtnLAF);
        fitButtons[i].setButtonText ("FIX"); 
        fitButtons[i].setClickingTogglesState (true); 
        fitButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); 
        fitButtons[i].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffe67e22)); 
        fitButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black); 
        fitButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        fitAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "fit" + chStr, fitButtons[i]);
        
        fitButtons[i].onClick = [this, i]() {
            bool isFit = fitButtons[i].getToggleState();
            fitButtons[i].setButtonText (isFit ? "FIT" : "FIX");
            if (!isFit) {
                int currentLen = (int) audioProcessor.apvts.getRawParameterValue("length" + juce::String(i))->load();
                if (currentLen > 16) {
                    if (auto* lenParam = audioProcessor.apvts.getParameter("length" + juce::String(i))) {
                        lenParam->beginChangeGesture();
                        lenParam->setValueNotifyingHost(lenParam->convertTo0to1(16.0f));
                        lenParam->endChangeGesture();
                    }
                }
            }
            updateLengthLabel(i);
            resized(); 
        };
'''
cpp = re.sub(constructor_regex, new_constructor, cpp)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
