import re

with open('src/PluginEditor.h', 'r') as f:
    header = f.read()

component_code = """
class ExportDragComponent : public juce::Component
{
public:
    ExportDragComponent(ExtasisRhythmProcessor& p) : processor(p) {
        setTooltip("Drag to DAW or Desktop \\nClick to Save As...");
    }

    void paint(juce::Graphics& g) override {
        g.setColour(juce::Colour(0xff2d3436));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
        
        g.setColour(isDragging ? juce::Colours::cyan : juce::Colour(0xff00d2ff));
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 4.0f, 1.5f);
        
        g.setFont(12.0f);
        g.drawText("〰️ WAV", getLocalBounds(), juce::Justification::centred);
    }
    
    void mouseDown(const juce::MouseEvent& e) override {
        isDragging = false;
    }

    void mouseDrag(const juce::MouseEvent& e) override {
        if (!isDragging && e.getDistanceFromDragStart() > 3) {
            isDragging = true;
            repaint();
            
            juce::File tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
            juce::File tempWav = tempDir.getChildFile("Extasis_Loop.wav");
            
            if (processor.renderOfflineLoop(tempWav)) {
                juce::StringArray files;
                files.add(tempWav.getFullPathName());
                juce::DragAndDropContainer::performExternalDragDropOfFiles(files, false, this);
            }
            
            isDragging = false;
            repaint();
        }
    }
    
    void mouseUp(const juce::MouseEvent& e) override {
        if (!isDragging) {
            // It was just a click
            chooser = std::make_unique<juce::FileChooser>("Save Loop as WAV...", 
                                                          juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Extasis_Loop.wav"), 
                                                          "*.wav");
            chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
                [this](const juce::FileChooser& fc) {
                    juce::File file = fc.getResult();
                    if (file != juce::File()) {
                        processor.renderOfflineLoop(file);
                    }
                });
        }
    }

private:
    ExtasisRhythmProcessor& processor;
    bool isDragging = false;
    std::unique_ptr<juce::FileChooser> chooser;
};

class ExtasisRhythmEditor  : public juce::AudioProcessorEditor,
"""

header = header.replace('class ExtasisRhythmEditor  : public juce::AudioProcessorEditor,', component_code)

if 'ExportDragComponent exportButton;' not in header:
    header = header.replace('juce::TextButton collectSaveButton;', 'juce::TextButton collectSaveButton;\n    ExportDragComponent exportButton { audioProcessor };')

with open('src/PluginEditor.h', 'w') as f:
    f.write(header)

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# Add to constructor
add_str = """    addAndMakeVisible (collectSaveButton);"""
add_str_new = """    addAndMakeVisible (collectSaveButton);
    addAndMakeVisible(exportButton);"""
cpp = cpp.replace(add_str, add_str_new)

# Add to resized
resize_str = """collectSaveButton.setBounds(btnStartX + 168, topControlsY, 90, 24);"""
resize_str_new = """collectSaveButton.setBounds(btnStartX + 168, topControlsY, 90, 24);
    exportButton.setBounds(btnStartX + 264, topControlsY, 60, 24);"""
cpp = cpp.replace(resize_str, resize_str_new)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
