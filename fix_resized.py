import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# The loop is:
# for (int cIdx = 0; cIdx < 12; ++cIdx) {
#     int x = 10 + cIdx * 98;
#     channelLedButtons[cIdx]->setBounds (sz(x + 5, 187, 82, 28));
#     ...
#     delaySendSliders[cIdx].setBounds  (sz(x + 51, 396, kSize, kSize));
# }

# We will just replace it with channelStrips[cIdx]->setBounds
pattern_cidx = r'channelLedButtons\[cIdx\]->setBounds.*?delaySendSliders\[cIdx\]\.setBounds  \(sz\(x \+ 51, 396, kSize, kSize\)\);'
cpp = re.sub(pattern_cidx, 'channelStrips[cIdx]->setBounds (sz(x, 170, 92, 330));', cpp, flags=re.DOTALL)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
