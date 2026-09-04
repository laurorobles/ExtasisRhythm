import re

with open('src/ChannelStripComponent.cpp', 'r') as f:
    content = f.read()

# Add button init to constructor
ctor_search = "sampleSourceSelector.setJustificationType(juce::Justification::centred);"
ctor_replace = """sampleSourceSelector.setJustificationType(juce::Justification::centred);

    loadCustomButton.setButtonText("...");
    loadCustomButton.setTooltip("Load Custom Sample");
    loadCustomButton.onClick = [this] {
        fileChooser = std::make_unique<juce::FileChooser>("Select Audio File", juce::File::getSpecialLocation(juce::File::userMusicDirectory), "*.wav;*.aif;*.mp3");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this](const juce::FileChooser& fc) {
            juce::File file = fc.getResult();
            if (file.existsAsFile()) {
                audioProcessor.customSamplePaths[channelIndex] = file.getFullPathName();
                audioProcessor.loadSmartSampleForChannel(channelIndex, 0); // Kit param is ignored when custom path is set
            }
        });
    };
    addAndMakeVisible(loadCustomButton);"""
content = content.replace(ctor_search, ctor_replace)

# Paint modification for dragging highlight
paint_old = """    g.setColour (juce::Colour(30, 30, 30));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 8.0f);"""
paint_new = """    g.setColour (isDragging ? juce::Colour(50, 60, 50) : juce::Colour(30, 30, 30));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 8.0f);
    if (isDragging) {
        g.setColour(juce::Colours::yellow);
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(2.0f), 8.0f, 2.0f);
    }"""
content = content.replace(paint_old, paint_new)

# Resized modification for button
resize_search = "sampleSourceSelector.setBounds (5, yPos, w - 10, 20);"
resize_replace = """sampleSourceSelector.setBounds (5, yPos, w - 35, 20);
    loadCustomButton.setBounds (w - 25, yPos, 20, 20);"""
content = content.replace(resize_search, resize_replace)

# Drag and drop methods
methods = """
bool ChannelStripComponent::isInterestedInFileDrag(const juce::StringArray& files) {
    for (auto file : files) {
        if (file.endsWithIgnoreCase(".wav") || file.endsWithIgnoreCase(".aif") || file.endsWithIgnoreCase(".mp3")) {
            return true;
        }
    }
    return false;
}

void ChannelStripComponent::filesDropped(const juce::StringArray& files, int x, int y) {
    isDragging = false;
    repaint();
    for (auto file : files) {
        if (file.endsWithIgnoreCase(".wav") || file.endsWithIgnoreCase(".aif") || file.endsWithIgnoreCase(".mp3")) {
            audioProcessor.customSamplePaths[channelIndex] = file;
            audioProcessor.loadSmartSampleForChannel(channelIndex, 0);
            return;
        }
    }
}

void ChannelStripComponent::fileDragEnter(const juce::StringArray& files, int x, int y) {
    isDragging = true;
    repaint();
}

void ChannelStripComponent::fileDragExit(const juce::StringArray& files) {
    isDragging = false;
    repaint();
}
"""

content += methods

with open('src/ChannelStripComponent.cpp', 'w') as f:
    f.write(content)
