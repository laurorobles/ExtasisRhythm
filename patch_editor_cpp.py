import re

with open('src/PluginEditor.cpp', 'r') as f:
    content = f.read()

# Add button properties and logic
add_search = """addAndMakeVisible (saveKitButton);"""
add_replace = """addAndMakeVisible (saveKitButton);
    addAndMakeVisible (collectSaveButton);"""
content = content.replace(add_search, add_replace)

setup_search = """saveKitButton.setButtonText ("SAVE");"""
setup_replace = """saveKitButton.setButtonText ("SAVE PRST");
    collectSaveButton.setButtonText ("COLLECT KIT");
    collectSaveButton.onClick = [this] {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::QuestionIcon,
            "Save Custom Kit",
            "Enter a name for your custom kit:",
            "Save",
            [this](int) {
                // To keep it simple without blocking, we can use an AlertWindow properly constructed
                // But JUCE showMessageBoxAsync doesn't accept text input easily like that.
                // We'll construct a custom AlertWindow
            }
        );
    };"""

content = content.replace(setup_search, setup_replace)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(content)
