import re

with open('src/HardwareManager.h', 'r') as f:
    code = f.read()

code = code.replace(
'''enum Mode { Sequencer = 0, Browser, FXRack };''',
'''enum Mode { Sequencer = 0, Browser, FXRack, Mixer };''')

code = code.replace(
'''void processFXRack(const juce::MidiMessage& m);''',
'''void processFXRack(const juce::MidiMessage& m);
    void processMixer(const juce::MidiMessage& m);''')

with open('src/HardwareManager.h', 'w') as f:
    f.write(code)

with open('src/HardwareManager.cpp', 'r') as f:
    code = f.read()

code = code.replace(
'''else if (currentMode == FXRack) processFXRack(message);''',
'''else if (currentMode == FXRack) processFXRack(message);
        else if (currentMode == Mixer) processMixer(message);''')

code = code.replace(
'''else if (cc == 110) { // User 2
        currentMode = FXRack;
    }''',
'''else if (cc == 110) { // User 2
        currentMode = FXRack;
    } else if (cc == 111) { // Mixer
        currentMode = Mixer;
    }''')

# Add processMixer
mixer_impl = '''
void HardwareManager::processMixer(const juce::MidiMessage& m) {
    int note = m.getNoteNumber();
    int row = note / 16;
    int col = note % 16;
    
    if (col < 8) { // Grid
        // 8 Columns = 8 Channels. 8 Rows = Volume 0 to 7 (Row 7 is bottom, Row 0 is top).
        // Let's say Row 7 = 0%, Row 0 = 100%.
        float vol = 1.0f - ((float)row / 7.0f);
        if (auto* p = processor.getParam("chan_gain_" + juce::String(col))) *p = vol;
    }
    
    if (row == 7 && col == 8) { // Bottom Right Round Button
        // Toggle Play/Stop
        if (auto* p = processor.getParam("isPlaying")) *p = (p->load() > 0.5f) ? 0.0f : 1.0f;
    }
}
'''
code = code.replace('void HardwareManager::updateLEDs() {', mixer_impl + '\nvoid HardwareManager::updateLEDs() {')

# Add Mixer LED updates
mixer_led = '''            } else if (currentMode == Mixer) {
                if (col < 8) {
                    float vol = 0.0f;
                    if (auto* p = processor.getParam("chan_gain_" + juce::String(col))) vol = p->load();
                    int targetRow = 7 - (int)(vol * 7.0f);
                    if (row == targetRow) color = 60; // Green dot for volume level
                    else if (row > targetRow) color = 13; // Dim red below level
                }
            }
'''
code = code.replace('} else if (currentMode == FXRack) {', '} else if (currentMode == FXRack) {')
code = code.replace('sendColor(note, color);', mixer_led + '            sendColor(note, color);')

# Top buttons
code = code.replace('setButtonLED(110, currentMode == FXRack ? 60 : 13);',
'''setButtonLED(110, currentMode == FXRack ? 60 : 13);
    setButtonLED(111, currentMode == Mixer ? 60 : 13);''')

with open('src/HardwareManager.cpp', 'w') as f:
    f.write(code)
