import re

with open('src/PluginEditor.h', 'r') as f:
    content = f.read()

content = content.replace('juce::TextButton saveKitButton, loadKitButton, randomKitButton, browseFolderButton;', 'juce::TextButton saveKitButton, loadKitButton, randomKitButton, browseFolderButton, collectSaveButton;')

with open('src/PluginEditor.h', 'w') as f:
    f.write(content)

