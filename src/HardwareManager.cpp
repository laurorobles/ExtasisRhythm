#include "HardwareManager.h"
#include "PluginProcessor.h"

HardwareManager::HardwareManager(ExtasisRhythmProcessor& p) : processor(p) {
    scanAndOpenDevices();
}

HardwareManager::~HardwareManager() {
    openedInputs.clear();
    launchpadOutput = nullptr;
}

void HardwareManager::scanAndOpenDevices() {
    auto inputs = juce::MidiInput::getAvailableDevices();
    for (auto& dev : inputs) {
        if (auto in = juce::MidiInput::openDevice(dev.identifier, this)) {
            in->start();
            openedInputs.push_back(std::move(in));
        }
    }
    
    auto outputs = juce::MidiOutput::getAvailableDevices();
    for (auto& dev : outputs) {
        if (dev.name.toLowerCase().contains("launchpad")) {
            launchpadOutput = juce::MidiOutput::openDevice(dev.identifier);
            break;
        }
    }
    
    // Reset Launchpad (Send Reset SysEx or mapping byte)
    if (launchpadOutput) {
        launchpadOutput->sendMessageNow(juce::MidiMessage::controllerEvent(1, 0, 0)); // Resets mapping (usually CC0 = 0)
    }
}

void HardwareManager::sendColor(int note, int colorVel) {
    if (launchpadOutput) {
        launchpadOutput->sendMessageNow(juce::MidiMessage::noteOn(1, note, (juce::uint8)colorVel));
    }
}

void HardwareManager::setButtonLED(int cc, int colorVel) {
    if (launchpadOutput) {
        launchpadOutput->sendMessageNow(juce::MidiMessage::controllerEvent(1, cc, (juce::uint8)colorVel));
    }
}

void HardwareManager::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) {
    // Live MIDI Keyboard Drumming (Notes 36-43)
    if (message.isNoteOn() && message.getNoteNumber() >= 36 && message.getNoteNumber() <= 43 && !source->getName().toLowerCase().contains("launchpad")) {
        // We let processBlock handle this normally, or we trigger the envelope here if we bypass standard MIDI
        return; 
    }
    
    // Launchpad CC (Top Row)
    if (message.isController()) {
        processTopRow(message.getControllerNumber(), message.getControllerValue());
    }
    
    // Launchpad Grid (Note On)
    if (message.isNoteOn()) {
        if (currentMode == Sequencer) processSequencer(message);
        else if (currentMode == Browser) processBrowser(message);
        else if (currentMode == FXRack) processFXRack(message);
    } else if (message.isNoteOff() && currentMode == Browser) {
        // Release held channel
        int note = message.getNoteNumber();
        int row = note / 16;
        if (row == 7) { // Bottom row
            int col = note % 16;
            if (col < 8 && heldBrowserChannel == col) {
                heldBrowserChannel = -1;
            }
        }
    }
}

void HardwareManager::processTopRow(int cc, int val) {
    if (val == 0) return; // Only process on press
    
    if (cc == 104) { // Up Arrow
        if (currentMode == Sequencer) {
            sequencerPage = 0; 
        } else if (currentMode == Browser && heldBrowserChannel != -1) {
            processor.loadNextSample(heldBrowserChannel, 1);
        }
    } else if (cc == 105) { // Down Arrow
        if (currentMode == Sequencer) {
            sequencerPage = 1;
        } else if (currentMode == Browser && heldBrowserChannel != -1) {
            processor.loadNextSample(heldBrowserChannel, -1);
        }
    } else if (cc == 106) { // Left Arrow
        if (currentMode == Browser && heldBrowserChannel != -1) {
            processor.changeCategory(heldBrowserChannel, -1);
        }
    } else if (cc == 107) { // Right Arrow
        if (currentMode == Browser && heldBrowserChannel != -1) {
            processor.changeCategory(heldBrowserChannel, 1);
        }
    } else if (cc == 108) { // Session
        currentMode = Sequencer;
    } else if (cc == 109) { // User 1
        currentMode = Browser;
    } else if (cc == 110) { // User 2
        currentMode = FXRack;
    }
}

void HardwareManager::processSequencer(const juce::MidiMessage& m) {
    int note = m.getNoteNumber();
    int row = note / 16;
    int col = note % 16;
    if (col > 7) return; // Right side round buttons
    
    int chOffset = sequencerPage * 4;
    int channel = chOffset + (row / 2);
    int step = (row % 2 == 0) ? col : col + 8;
    
    if (channel < 8) {
        int currentSteps = processor.channelSteps[channel].load();
        processor.channelSteps[channel].store(currentSteps ^ (1 << step)); // Toggle bit
    }
}

void HardwareManager::processBrowser(const juce::MidiMessage& m) {
    int note = m.getNoteNumber();
    int row = note / 16;
    int col = note % 16;
    
    if (row == 7 && col < 8) { // Bottom row = Triggers
        heldBrowserChannel = col;
        // The processor's audio thread will handle standard MIDI if it routes, 
        // but since we are headless, let's inject a trigger:
        processor.activeMidiNotes[col] = 127; // Quick hack to trigger envelope on next block
    }
}

void HardwareManager::processFXRack(const juce::MidiMessage& m) {
    int note = m.getNoteNumber();
    int row = note / 16;
    int col = note % 16;
    
    if (row == 0 && col == 0) {
        fxState[0] = !fxState[0]; // Bitcrush
        if (auto* p = processor.getParam("pcmBits")) *p = fxState[0] ? 12.0f : 24.0f;
    } else if (row == 1 && col == 0) {
        fxState[1] = !fxState[1]; // Delay
        if (auto* p = processor.getParam("delaySendL")) *p = fxState[1] ? 0.3f : 0.0f;
    } else if (row == 2 && col == 0) {
        fxState[2] = !fxState[2]; // Pump
        if (auto* p = processor.getParam("pumpMix")) *p = fxState[2] ? 1.0f : 0.0f;
    }
}

void HardwareManager::updateLEDs() {
    if (!launchpadOutput) return;
    
    // Update Mode Buttons
    setButtonLED(108, currentMode == Sequencer ? 60 : 13);
    setButtonLED(109, currentMode == Browser ? 60 : 13);
    setButtonLED(110, currentMode == FXRack ? 60 : 13);
    setButtonLED(104, sequencerPage == 0 ? 60 : 13);
    setButtonLED(105, sequencerPage == 1 ? 60 : 13);
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            int note = row * 16 + col;
            int color = 0; // Off
            
            if (currentMode == Sequencer) {
                int chOffset = sequencerPage * 4;
                int channel = chOffset + (row / 2);
                int step = (row % 2 == 0) ? col : col + 8;
                
                if (channel < 8) {
                    bool isActive = (processor.channelSteps[channel].load() & (1 << step)) != 0;
                    bool isPlaying = (processor.currentMappedStep[channel].load() == step);
                    
                    if (isPlaying) color = 15; // Bright Red
                    else if (isActive) color = 60; // Bright Green
                    else color = 13; // Dim Red
                }
            } else if (currentMode == Browser) {
                if (row == 7) {
                    color = (heldBrowserChannel == col) ? 60 : 13; // Selected = Green, Else = Red
                }
            } else if (currentMode == FXRack) {
                if (col == 0 && row < 3) {
                    color = fxState[row] ? 60 : 13;
                }
            }
            sendColor(note, color);
        }
    }
}
