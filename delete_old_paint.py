import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# We need to find the loop:
# for (int i = 0; i < 12; ++i) { 
#     int x = 10 + i * 98; 
#     ...
#     g.drawText ("DLY",   x + 48, 428, 40, 9, juce::Justification::centred);
# }

# Find the specific block
pattern = r'for \(int i = 0; i < 12; \+\+i\) \{[^{]*?g\.drawText \("DLY",\s*x \+ 48,\s*428,\s*40,\s*9,\s*juce::Justification::centred\);\s*\}'
cpp = re.sub(pattern, '', cpp, flags=re.DOTALL)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
