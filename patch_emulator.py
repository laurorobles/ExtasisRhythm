import re

with open('src/HardwareManager.h', 'r') as f:
    code = f.read()

# Add a public callback for emulation
code = code.replace(
'''    void sendColor(int note, int colorVel);
    void setButtonLED(int cc, int colorVel);''',
'''    void sendColor(int note, int colorVel);
    void setButtonLED(int cc, int colorVel);
    
    std::function<void(int, int)> onColorSent; // For testing/emulation
    std::function<void(int, int)> onButtonLEDSent;''')

with open('src/HardwareManager.h', 'w') as f:
    f.write(code)

with open('src/HardwareManager.cpp', 'r') as f:
    code = f.read()

# Modify sendColor to call callback
code = code.replace(
'''void HardwareManager::sendColor(int note, int colorVel) {
    if (launchpadOutput) {
        launchpadOutput->sendMessageNow(juce::MidiMessage::noteOn(1, note, (juce::uint8)colorVel));
    }
}''',
'''void HardwareManager::sendColor(int note, int colorVel) {
    if (onColorSent) onColorSent(note, colorVel);
    if (launchpadOutput) {
        launchpadOutput->sendMessageNow(juce::MidiMessage::noteOn(1, note, (juce::uint8)colorVel));
    }
}''')

code = code.replace(
'''void HardwareManager::setButtonLED(int cc, int colorVel) {
    if (launchpadOutput) {
        launchpadOutput->sendMessageNow(juce::MidiMessage::controllerEvent(1, cc, (juce::uint8)colorVel));
    }
}''',
'''void HardwareManager::setButtonLED(int cc, int colorVel) {
    if (onButtonLEDSent) onButtonLEDSent(cc, colorVel);
    if (launchpadOutput) {
        launchpadOutput->sendMessageNow(juce::MidiMessage::controllerEvent(1, cc, (juce::uint8)colorVel));
    }
}''')

with open('src/HardwareManager.cpp', 'w') as f:
    f.write(code)
