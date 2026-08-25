import re

# 1. Update PluginEditor.h
with open('src/PluginEditor.h', 'r') as f:
    h = f.read()

# Add include
h = h.replace('#include "UIComponents.h"', '#include "UIComponents.h"\n#include "ChannelStripComponent.h"')

# Delete old arrays
deletions = [
    r'juce::ComboBox sampleSourceSelectors\[12\];.*?\n',
    r'juce::ComboBox sampleVariantSelectors\[12\];.*?\n',
    r'std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sampleSourceAtts\[12\];.*?\n',
    r'juce::TextButton muteButtons\[12\], soloButtons\[12\], envChannelButtons\[12\], fitButtons\[12\];.*?\n',
    r'std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAtts\[12\], soloAtts\[12\], envChanAtts\[12\], fitAtts\[12\];.*?\n',
    r'juce::Slider volumeSliders\[12\], panSliders\[12\], pitchSliders\[12\], toneSliders\[12\];.*?\n',
    r'juce::Slider attackSliders\[12\], decaySliders\[12\], springSendSliders\[12\], delaySendSliders\[12\];.*?\n',
    r'std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAtts\[12\], panAtts\[12\], pitchAtts\[12\], toneAtts\[12\];.*?\n',
    r'std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attAtts\[12\], decAtts\[12\], springSendAtts\[12\], delaySendAtts\[12\];.*?\n',
    r'std::vector<std::unique_ptr<ChannelLedButton>> channelLedButtons;.*?\n'
]
for d in deletions:
    h = re.sub(d, '', h, flags=re.DOTALL)

# Add channelStrips[12]
h = re.sub(r'(std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stepLengthAtts\[12\];)', 
           r'std::unique_ptr<ChannelStripComponent> channelStrips[12];\n    \1', h)

with open('src/PluginEditor.h', 'w') as f:
    f.write(h)


# 2. Update PluginEditor.cpp
with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# Replace massive constructor loop with the ChannelStrip instantiation
constructor_regex = r'for \(int i = 0; i < 12; \+\+i\) \{\s*juce::String chStr = juce::String\(i\).*?(?=\s*addAndMakeVisible \(browseFolderButton\);)'
new_constructor = '''for (int i = 0; i < 12; ++i) {
        channelStrips[i] = std::make_unique<ChannelStripComponent>(audioProcessor, i, &safeComboBoxLAF, &masterChannelKnobLAF);
        addAndMakeVisible(*channelStrips[i]);
    }
'''
cpp = re.sub(constructor_regex, new_constructor, cpp, flags=re.DOTALL)

# Delete old bounds setting in resized()
bounds_regex = r'channelLedButtons\[i\]->setBounds \(x \+ 36, 172, 20, 20\);.*?fitButtons\[i\]\.setBounds \(x \+ 52, 480, 32, 16\);'
new_bounds = 'channelStrips[i]->setBounds (x, 170, 92, 330);'
cpp = re.sub(bounds_regex, new_bounds, cpp, flags=re.DOTALL)

# Delete old painting code in paint()
paint_regex = r'g\.setColour \(juce::Colours::black\.withAlpha \(0\.15f\)\);.*?g\.drawText \("DELAY", x \+ 48, 465, 40, 9, juce::Justification::centred\);'
cpp = re.sub(paint_regex, '', cpp, flags=re.DOTALL)
# The above paint replacement also left `int x = 10 + i * 98;` which is empty. We can leave it or remove it. Let's just remove the empty loop.
empty_paint_loop = r'for \(int i = 0; i < 12; \+\+i\) \{\s*int x = 10 \+ i \* 98;\s*\}'
cpp = re.sub(empty_paint_loop, '', cpp, flags=re.DOTALL)

# Fix references to fitButtons
cpp = cpp.replace('fitButtons[i].setToggleState', 'channelStrips[i]->fitButton.setToggleState')
cpp = cpp.replace('fitButtons[i].getToggleState', 'channelStrips[i]->fitButton.getToggleState')
cpp = cpp.replace('fitButtons[i].setButtonText', 'channelStrips[i]->fitButton.setButtonText')

# Fix references to selectors
cpp = cpp.replace('sampleSourceSelectors[i]', 'channelStrips[i]->sampleSourceSelector')
cpp = cpp.replace('sampleVariantSelectors[i]', 'channelStrips[i]->sampleVariantSelector')

# Fix references to LED
cpp = cpp.replace('channelLedButtons[i]->repaint()', 'channelStrips[i]->ledButton->repaint()')

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)

print("Refactor complete")
