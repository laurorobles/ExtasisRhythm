import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# Replace the block from `channelLedButtons.push_back` up to `fitAtts[i] = ...`
pattern_led_to_fitAtt = r'channelLedButtons\.push_back \(std::make_unique<ChannelLedButton> \(audioProcessor, i\)\);\s*addAndMakeVisible \(\*channelLedButtons\[i\]\);.*?fitAtts\[i\] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> \(audioProcessor\.apvts, "fit" \+ chStr, fitButtons\[i\]\);'
replacement = '''channelStrips[i] = std::make_unique<ChannelStripComponent>(audioProcessor, i, &safeComboBoxLAF, &masterChannelKnobLAF);
        addAndMakeVisible(*channelStrips[i]);'''
cpp = re.sub(pattern_led_to_fitAtt, replacement, cpp, flags=re.DOTALL)

# Replace mkChannelKnob calls
pattern_knobs = r'mkChannelKnob \(volumeSliders\[i\].*?mkChannelKnob \(delaySendSliders\[i\].*?juce::Colour\(0xff00d2ff\)\);'
cpp = re.sub(pattern_knobs, '', cpp, flags=re.DOTALL)

# Fix fitButtons -> channelStrips[i]->fitButton
cpp = cpp.replace('fitButtons[i].onClick', 'channelStrips[i]->fitButton.onClick')
cpp = cpp.replace('fitButtons[i].getToggleState()', 'channelStrips[i]->fitButton.getToggleState()')
cpp = cpp.replace('fitButtons[i].setButtonText', 'channelStrips[i]->fitButton.setButtonText')
cpp = cpp.replace('fitButtons[i].setToggleState', 'channelStrips[i]->fitButton.setToggleState')

# Fix sampleSourceSelectors and sampleVariantSelectors
cpp = cpp.replace('sampleSourceSelectors[i]', 'channelStrips[i]->sampleSourceSelector')
cpp = cpp.replace('sampleVariantSelectors[i]', 'channelStrips[i]->sampleVariantSelector')

# Fix channelLedButtons
cpp = cpp.replace('channelLedButtons[i]->repaint()', 'channelStrips[i]->ledButton->repaint()')

# In resized(), remove the setBounds for the old items
pattern_bounds = r'channelLedButtons\[i\]->setBounds \(x \+ 36, 172, 20, 20\);.*?fitButtons\[i\]\.setBounds \(x \+ 52, 480, 32, 16\);'
cpp = re.sub(pattern_bounds, 'channelStrips[i]->setBounds (x, 170, 92, 330);', cpp, flags=re.DOTALL)

# In paint(), remove the background painting of the channels
pattern_paint = r'int x = 10 \+ i \* 98;.*?g\.drawText \("DELAY", 48, 279, 40, 9, juce::Justification::centred\);'
# Wait, the original paint had absolute coordinates! Let's just match the start and end of that specific loop.
pattern_paint2 = r'for \(int i = 0; i < 12; \+\+i\) \{\s*int x = 10 \+ i \* 98; \s*g\.setColour \(juce::Colours::black\.withAlpha \(0\.15f\)\);.*?g\.drawText \("DELAY", x \+ 48, 465, 40, 9, juce::Justification::centred\);\s*\}'
cpp = re.sub(pattern_paint2, '', cpp, flags=re.DOTALL)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
print("Done")
