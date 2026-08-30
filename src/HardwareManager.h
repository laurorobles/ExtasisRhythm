#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_devices/juce_audio_devices.h>

class ExtasisRhythmProcessor;

class HardwareManager : public juce::MidiInputCallback {
public:
    HardwareManager(ExtasisRhythmProcessor& p);
    ~HardwareManager();

    void scanAndOpenDevices();
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    
    void updateLEDs();

private:
    ExtasisRhythmProcessor& processor;
    std::unique_ptr<juce::MidiOutput> launchpadOutput;
    std::vector<std::unique_ptr<juce::MidiInput>> openedInputs;
    
    enum Mode { Sequencer = 0, Browser, FXRack };
    Mode currentMode = Sequencer;
    
    int sequencerPage = 0; // 0 = Ch 1-4, 1 = Ch 5-8
    int heldBrowserChannel = -1; // -1 = none
    
    void processSequencer(const juce::MidiMessage& m);
    void processBrowser(const juce::MidiMessage& m);
    void processFXRack(const juce::MidiMessage& m);
    void processTopRow(int cc, int val);
    
    void sendColor(int note, int colorVel);
    void setButtonLED(int cc, int colorVel);
};
