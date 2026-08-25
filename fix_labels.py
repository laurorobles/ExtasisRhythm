import re

with open('src/ChannelStripComponent.cpp', 'r') as f:
    cpp = f.read()

# Replace the paint text lines
old_paint = r'g\.drawText \("VOL",.*?g\.drawText \("DELAY",   48, 279, 40, 9, juce::Justification::centred\);'
new_paint = '''g.drawText ("VOL",   4,  139, 40, 9, juce::Justification::centred);
    g.drawText ("PAN",   48, 139, 40, 9, juce::Justification::centred);
    g.drawText ("PITCH", 4,  180, 40, 9, juce::Justification::centred);
    g.drawText ("TONE",  48, 180, 40, 9, juce::Justification::centred);
    g.drawText ("ATT",   4,  221, 40, 9, juce::Justification::centred);
    g.drawText ("DEC",   48, 221, 40, 9, juce::Justification::centred);
    g.drawText ("SPRING",4,  262, 40, 9, juce::Justification::centred);
    g.drawText ("DELAY", 48, 262, 40, 9, juce::Justification::centred);'''
cpp = re.sub(old_paint, new_paint, cpp, flags=re.DOTALL)

with open('src/ChannelStripComponent.cpp', 'w') as f:
    f.write(cpp)
