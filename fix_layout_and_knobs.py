import re

# 1. FIX KNOB NEEDLES in UIComponents.h
with open('src/UIComponents.h', 'r') as f:
    ui = f.read()

# Replace p.addRectangle (-1.0f, -capRadius * 0.95f, 2.0f, capRadius * 0.95f);
ui = re.sub(r'p\.addRectangle \(-1\.0f, -capRadius \* 0\.95f, 2\.0f, capRadius \* 0\.95f\);',
            r'p.addRectangle (-1.3f, -radius * 1.0f, 2.6f, radius * 1.0f);', ui)

# Replace p.addRectangle (-1.0f, -capRadius * 0.85f, 2.0f, capRadius * 0.85f);
ui = re.sub(r'p\.addRectangle \(-1\.0f, -capRadius \* 0\.85f, 2\.0f, capRadius \* 0\.85f\);',
            r'p.addRectangle (-1.3f, -radius * 1.0f, 2.6f, radius * 1.0f);', ui)

with open('src/UIComponents.h', 'w') as f:
    f.write(ui)


# 2. FIX CHANNEL STRIP LAYOUT in ChannelStripComponent.cpp
with open('src/ChannelStripComponent.cpp', 'r') as f:
    cpp = f.read()

# Fix paint()
# g.drawText (channelLabel, 5, 24, 82, 11, juce::Justification::centred);
# g.setColour (juce::Colour (0xff222222)); 
# g.drawText (channelLabel, 4, 23, 82, 11, juce::Justification::centred);
old_paint_text = r'g\.drawText \(channelLabel, 5, 24, 82, 11, juce::Justification::centred\);\s*g\.setColour \(juce::Colour \(0xff222222\)\); \s*g\.drawText \(channelLabel, 4, 23, 82, 11, juce::Justification::centred\);'
new_paint_text = '''g.drawText (channelLabel, 5, 36, 82, 11, juce::Justification::centred);
    g.setColour (juce::Colour (0xff222222)); 
    g.drawText (channelLabel, 4, 35, 82, 11, juce::Justification::centred);'''
cpp = re.sub(old_paint_text, new_paint_text, cpp, flags=re.DOTALL)

# Fix resized()
old_resized = r'ledButton->setBounds \(sz\(5, 17, 82, 24\)\);.*?sampleVariantSelector\.setBounds \(sz\(6, 83, 80, 16\)\);'
new_resized = '''ledButton->setBounds (sz(5, 17, 82, 16));
    
    muteButton.setBounds (sz(6, 48, 22, 15));
    soloButton.setBounds (sz(30, 48, 22, 15));
    envChannelButton.setBounds (sz(54, 48, 32, 15));
    
    sampleSourceSelector.setBounds (sz(6, 66, 80, 16));
    sampleVariantSelector.setBounds (sz(6, 84, 80, 16));'''
cpp = re.sub(old_resized, new_resized, cpp, flags=re.DOTALL)

with open('src/ChannelStripComponent.cpp', 'w') as f:
    f.write(cpp)

