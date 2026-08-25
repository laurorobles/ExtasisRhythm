import re

with open('src/PluginEditor.h', 'r') as f:
    hdr = f.read()

# Add juce::HyperlinkButton bandcampButton;
if 'juce::HyperlinkButton bandcampButton;' not in hdr:
    hdr = hdr.replace('bool isSequencerVisible = true;', 'bool isSequencerVisible = true;\n\n    juce::HyperlinkButton bandcampButton;')

with open('src/PluginEditor.h', 'w') as f:
    f.write(hdr)

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# Add bandcampButton initialization
if 'bandcampButton.setButtonText' not in cpp:
    old_init = r'addAndMakeVisible \(browseFolderButton\);'
    new_init = '''addAndMakeVisible (browseFolderButton);
    
    addAndMakeVisible (bandcampButton);
    bandcampButton.setButtonText ("extasisrecords.bandcamp.com");
    bandcampButton.setURL (juce::URL ("https://extasisrecords.bandcamp.com"));
    bandcampButton.setColour (juce::HyperlinkButton::textColourId, juce::Colour (0xff00d2ff));
    bandcampButton.setFont (juce::FontOptions (12.0f, juce::Font::bold), false);'''
    cpp = re.sub(old_init, new_init, cpp)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
