#include <juce_core/juce_core.h>
#include <iostream>
#include <iomanip>
#include "src/PluginProcessor.h"
#include "src/HardwareManager.h"

int gridColors[8][8] = {0}; // 0 = off, 13 = dim red, 15 = bright red, 60 = bright green
int topButtons[8] = {0};

void printLaunchpad() {
    std::cout << "\033[2J\033[1;1H"; // Clear screen
    std::cout << "=== EXTASIS-8 LAUNCHPAD EMULATOR ===\n\n";
    
    // Top Buttons
    std::cout << " [UP] [DN] [LF] [RT]   [SE] [U1] [U2] [MX]\n  ";
    for (int i=0; i<8; i++) {
        int color = topButtons[i];
        if (color == 60) std::cout << "\033[1;32m(O)\033[0m "; // Green
        else if (color == 13 || color == 15) std::cout << "\033[1;31m(O)\033[0m "; // Red
        else std::cout << "( ) ";
        if (i == 3) std::cout << "  ";
    }
    std::cout << "\n\n";
    
    // 8x8 Grid
    for (int row=0; row<8; row++) {
        for (int col=0; col<8; col++) {
            int color = gridColors[row][col];
            if (color == 60) std::cout << "\033[1;32m[X] \033[0m"; // Green
            else if (color == 15) std::cout << "\033[1;31m[X] \033[0m"; // Bright Red
            else if (color == 13) std::cout << "\033[31m[-] \033[0m"; // Dim Red
            else std::cout << "[ ] ";
        }
        std::cout << "\n";
    }
    std::cout << "\n------------------------------------\n";
}

int main() {
    juce::ScopedJuceInitialiser_GUI juceInit;
    
    std::cout << "Booting Extasis Rhythm Processor...\n";
    ExtasisRhythmProcessor processor;
    
    if (processor.hardwareManager) {
        processor.hardwareManager->onColorSent = [](int note, int color) {
            int row = note / 16;
            int col = note % 16;
            if (row < 8 && col < 8) gridColors[row][col] = color;
        };
        
        processor.hardwareManager->onButtonLEDSent = [](int cc, int color) {
            if (cc >= 104 && cc <= 111) {
                topButtons[cc - 104] = color;
            }
        };
    }
    
    std::cout << "Processor booted. Simulating user interaction...\n";
    
    // Simulate pressing "Session" (CC 108)
    processor.hardwareManager->handleIncomingMidiMessage(nullptr, juce::MidiMessage::controllerEvent(1, 108, 127));
    
    // Simulate programming a basic beat
    // Kick on step 0, 4, 8, 12 (row 0, col 0, 4 and row 1, col 0, 4)
    processor.hardwareManager->handleIncomingMidiMessage(nullptr, juce::MidiMessage::noteOn(1, 0, (juce::uint8)127)); // row 0 col 0
    processor.hardwareManager->handleIncomingMidiMessage(nullptr, juce::MidiMessage::noteOn(1, 4, (juce::uint8)127)); // row 0 col 4
    processor.hardwareManager->handleIncomingMidiMessage(nullptr, juce::MidiMessage::noteOn(1, 16+0, (juce::uint8)127)); // row 1 col 0
    processor.hardwareManager->handleIncomingMidiMessage(nullptr, juce::MidiMessage::noteOn(1, 16+4, (juce::uint8)127)); // row 1 col 4
    
    // Snare on step 4, 12 (row 2 col 4, row 3 col 4)
    processor.hardwareManager->handleIncomingMidiMessage(nullptr, juce::MidiMessage::noteOn(1, 32+4, (juce::uint8)127)); 
    processor.hardwareManager->handleIncomingMidiMessage(nullptr, juce::MidiMessage::noteOn(1, 48+4, (juce::uint8)127)); 
    
    // Process audio blocks to advance sequencer and trigger LED updates
    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midi;
    
    processor.prepareToPlay(44100.0, 512);
    
    // Turn on Play
    if (auto* p = processor.getParam("isPlaying")) *p = 1.0f;
    
    for (int i=0; i<100; i++) { // Process 100 blocks
        processor.processBlock(buffer, midi);
    }
    
    // Print the grid
    printLaunchpad();
    
    return 0;
}
