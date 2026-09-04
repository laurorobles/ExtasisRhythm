import re

with open('src/PluginEditor.h', 'r') as f:
    content = f.read()

# Remove setTooltip
content = content.replace('setTooltip("Drag to DAW or Desktop \\nClick to Save As...");', '')
# Add SettableTooltipClient inheritance
content = content.replace('class ExportDragComponent : public juce::Component', 'class ExportDragComponent : public juce::Component, public juce::SettableTooltipClient')

old_decl = "juce::TextButton saveKitButton, loadKitButton, randomKitButton, browseFolderButton, collectSaveButton;"
new_decl = "juce::TextButton saveKitButton, loadKitButton, randomKitButton, browseFolderButton, collectSaveButton;\n    ExportDragComponent exportButton { audioProcessor };"
content = content.replace(old_decl, new_decl)

with open('src/PluginEditor.h', 'w') as f:
    f.write(content)
