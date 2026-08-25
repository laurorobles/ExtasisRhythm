import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

old_resized = r'licenseBadgeButton\.setBounds \(sz \(180, 42, 68, 16\)\);'
new_resized = '''licenseBadgeButton.setBounds (sz (180, 42, 68, 16));
    bandcampButton.setBounds (sz(10, 153, 270, 18));'''

cpp = re.sub(old_resized, new_resized, cpp)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
