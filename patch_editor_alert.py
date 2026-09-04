import re

with open('src/PluginEditor.cpp', 'r') as f:
    content = f.read()

setup_search = """    collectSaveButton.onClick = [this] {
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

setup_replace = """    collectSaveButton.onClick = [this] {
        auto* alert = new juce::AlertWindow("Save Custom Kit", "Enter a name for your custom kit:", juce::AlertWindow::QuestionIcon);
        alert->addTextEditor("kitName", "My_Custom_Kit");
        alert->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
        alert->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
        
        alert->enterModalState(true, juce::ModalCallbackFunction::create([this, alert](int result) {
            if (result == 1) {
                juce::String kitName = alert->getTextEditorContents("kitName");
                if (kitName.isNotEmpty()) {
                    audioProcessor.saveCustomKit(kitName);
                    refreshKitSelectors(); // Actually, the combo box is handled by ExtasisRhythmEditor but this might not refresh globalKitSelector
                    
                    audioProcessor.scanSampleFolders(); // Force a scan just in case
                    globalKitSelector.clear(juce::dontSendNotification);
                    auto kits = audioProcessor.getDrumKitNames();
                    for (int i = 0; i < kits.size(); ++i) globalKitSelector.addItem(kits[i], i + 1);
                    
                    int currentKit = 0;
                    for (int i = 0; i < kits.size(); ++i) {
                        if (kits[i] == kitName) currentKit = i;
                    }
                    globalKitSelector.setSelectedId(currentKit + 1, juce::sendNotificationSync);
                }
            }
            delete alert;
        }));
    };"""
content = content.replace(setup_search, setup_replace)

resize_search = "saveKitButton.setBounds (btnStartX + 84, topControlsY, 80, 24);"
resize_replace = """saveKitButton.setBounds (btnStartX + 84, topControlsY, 80, 24);
    collectSaveButton.setBounds(btnStartX + 168, topControlsY, 90, 24);"""
content = content.replace(resize_search, resize_replace)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(content)
