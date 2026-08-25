import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# We need to find the `for (int i = 0; i < 12; ++i)` loop that sets up the channels.
# Luckily it ends right before `for (int i = 0; i < 8; ++i) { addAndMakeVisible (patternButtons[i]);`
match = re.search(r'for \(int i = 0; i < 12; \+\+i\) \{\s*channelStrips\[i\] = std::make_unique<ChannelStripComponent>\(audioProcessor, i, &safeComboBoxLAF, &masterChannelKnobLAF\);\s*addAndMakeVisible\(\*channelStrips\[i\]\);\s*\}', cpp)
if not match:
    # If the previous regex failed to replace everything, let's fix it by completely stripping it and re-adding.
    # The previous regex replaced up to `addAndMakeVisible (browseFolderButton);`, which was WRONG.
    pass

# Wait, in the previous run, I replaced from `for(int i=0; i<12...)` up to `addAndMakeVisible(browseFolderButton)`. BUT `browseFolderButton` was BEFORE the massive setup loop for `volSliders`!
# Ah! There were multiple `for (int i=0...` loops in the constructor!
