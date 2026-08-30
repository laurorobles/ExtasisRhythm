#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "HardwareManager.h"

// Custom Component for Launchpad Pads
class EmulatorPad : public juce::Component {
public:
    EmulatorPad(int id, bool isCC, ExtasisRhythmProcessor& p);
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void setColor(int colorCode);

    int midiId;
    bool isCCMessage;
    int currentColorCode = 0;
    ExtasisRhythmProcessor& processor;
};

class ExtasisRhythmEditor : public juce::AudioProcessorEditor, public juce::Timer {
public:
    ExtasisRhythmEditor(ExtasisRhythmProcessor&);
    ~ExtasisRhythmEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    void logMessage(const juce::String& msg);

private:
    ExtasisRhythmProcessor& audioProcessor;
    
    std::vector<std::unique_ptr<EmulatorPad>> gridPads;
    std::vector<std::unique_ptr<EmulatorPad>> topButtons;
    std::vector<std::unique_ptr<EmulatorPad>> rightButtons;
    
    juce::TextEditor monitorLog;
    juce::Label titleLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtasisRhythmEditor)
};
