import re

with open('src/ChannelStripComponent.cpp', 'r') as f:
    cpp = f.read()

# Fix paint()
# We need to change the channelLabel text Y positions.
# Old:
# g.drawText (channelLabel, 5, 34, 82, 11, juce::Justification::centred);
# g.setColour (juce::Colour (0xff222222)); 
# g.drawText (channelLabel, 4, 33, 82, 11, juce::Justification::centred);
# New:
# g.drawText (channelLabel, 5, 24, 82, 11, juce::Justification::centred);
# g.setColour (juce::Colour (0xff222222)); 
# g.drawText (channelLabel, 4, 23, 82, 11, juce::Justification::centred);

cpp = cpp.replace('g.drawText (channelLabel, 5, 34, 82, 11, juce::Justification::centred);', 'g.drawText (channelLabel, 5, 24, 82, 11, juce::Justification::centred);')
cpp = cpp.replace('g.drawText (channelLabel, 4, 33, 82, 11, juce::Justification::centred);', 'g.drawText (channelLabel, 4, 23, 82, 11, juce::Justification::centred);')

# Fix resized()
old_resized = r'ledButton->setBounds \(sz\(5, 17, 82, 28\)\);.*?sampleVariantSelector\.setBounds \(sz\(6, 86, 80, 17\)\);'
new_resized = '''ledButton->setBounds (sz(5, 17, 82, 24));
    
    muteButton.setBounds (sz(6, 45, 22, 16));
    soloButton.setBounds (sz(30, 45, 22, 16));
    envChannelButton.setBounds (sz(54, 45, 32, 16));
    
    sampleSourceSelector.setBounds (sz(6, 64, 80, 16));
    sampleVariantSelector.setBounds (sz(6, 83, 80, 16));'''

cpp = re.sub(old_resized, new_resized, cpp, flags=re.DOTALL)

with open('src/ChannelStripComponent.cpp', 'w') as f:
    f.write(cpp)
