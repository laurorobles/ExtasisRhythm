#include "PluginEditor.h"

// --- EMULATOR PAD IMPLEMENTATION ---
EmulatorPad::EmulatorPad(int id, bool cc, ExtasisRhythmProcessor& p) : midiId(id), isCCMessage(cc), processor(p) {}

void EmulatorPad::paint(juce::Graphics& g) {
    juce::Colour c = juce::Colours::darkgrey;
    if (currentColorCode == 15) c = juce::Colours::red;
    else if (currentColorCode == 13) c = juce::Colours::darkred;
    else if (currentColorCode == 60) c = juce::Colours::lime;
    else if (currentColorCode == 62) c = juce::Colours::orange;
    
    g.setColour(c);
    if (isCCMessage) g.fillEllipse(getLocalBounds().toFloat().reduced(2.0f));
    else g.fillRect(getLocalBounds().reduced(2));
    
    g.setColour(juce::Colours::black);
    if (isCCMessage) g.drawEllipse(getLocalBounds().toFloat().reduced(2.0f), 2.0f);
    else g.drawRect(getLocalBounds().reduced(2), 2.0f);
}

void EmulatorPad::mouseDown(const juce::MouseEvent&) {
    if (processor.hardwareManager) {
        juce::MidiMessage m = isCCMessage ? juce::MidiMessage::controllerEvent(1, midiId, 127) 
                                          : juce::MidiMessage::noteOn(1, midiId, (juce::uint8)127);
        processor.hardwareManager->handleIncomingMidiMessage(nullptr, m);
    }
}

void EmulatorPad::mouseUp(const juce::MouseEvent&) {
    if (processor.hardwareManager) {
        juce::MidiMessage m = isCCMessage ? juce::MidiMessage::controllerEvent(1, midiId, 0) 
                                          : juce::MidiMessage::noteOff(1, midiId, (juce::uint8)0);
        processor.hardwareManager->handleIncomingMidiMessage(nullptr, m);
    }
}

void EmulatorPad::setColor(int colorCode) {
    if (currentColorCode != colorCode) {
        currentColorCode = colorCode;
        repaint();
    }
}


// --- EMULATOR EDITOR IMPLEMENTATION ---
ExtasisRhythmEditor::ExtasisRhythmEditor(ExtasisRhythmProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    setSize (800, 600);
    
    titleLabel.setText("EXTASIS-8 VIRTUAL LAUNCHPAD", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    
    monitorLog.setMultiLine(true);
    monitorLog.setReadOnly(true);
    monitorLog.setScrollbarsShown(true);
    monitorLog.setCaretVisible(false);
    monitorLog.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
    monitorLog.setColour(juce::TextEditor::textColourId, juce::Colours::green);
    addAndMakeVisible(monitorLog);
    logMessage("Extasis-8 Hardware Emulator Booted.");
    logMessage("Click pads to simulate Launchpad Mini.");
    
    // Top CC Buttons (104 - 111)
    for (int i = 0; i < 8; ++i) {
        auto btn = std::make_unique<EmulatorPad>(104 + i, true, p);
        addAndMakeVisible(btn.get());
        topButtons.push_back(std::move(btn));
    }
    
    // 8x8 Grid
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            auto pad = std::make_unique<EmulatorPad>(row * 16 + col, false, p);
            addAndMakeVisible(pad.get());
            gridPads.push_back(std::move(pad));
        }
    }
    
    // Right Side Buttons (8, 24, 40, etc.)
    for (int row = 0; row < 8; ++row) {
        auto btn = std::make_unique<EmulatorPad>(row * 16 + 8, true, p);
        addAndMakeVisible(btn.get());
        rightButtons.push_back(std::move(btn));
    }

    if (p.hardwareManager) {
        p.hardwareManager->onColorSent = [this](int note, int color) {
            juce::MessageManager::callAsync([this, note, color]() {
                int row = note / 16;
                int col = note % 16;
                if (row < 8 && col < 8) gridPads[row * 8 + col]->setColor(color);
            });
        };
        p.hardwareManager->onButtonLEDSent = [this](int cc, int color) {
            juce::MessageManager::callAsync([this, cc, color]() {
                if (cc >= 104 && cc <= 111) topButtons[cc - 104]->setColor(color);
            });
        };
    }
    
    startTimerHz(30);
}

ExtasisRhythmEditor::~ExtasisRhythmEditor() {}

void ExtasisRhythmEditor::logMessage(const juce::String& msg) {
    monitorLog.moveCaretToEnd();
    monitorLog.insertTextAtCaret(msg + "\n");
}

void ExtasisRhythmEditor::timerCallback() {
    // We can pull state from processor to update monitor if needed
}

void ExtasisRhythmEditor::paint(juce::Graphics& g) {
    g.fillAll (juce::Colour(0xff222222));
    
    // Labels for top buttons
    g.setColour(juce::Colours::white);
    g.setFont(10.0f);
    juce::String topLabels[] = {"UP", "DN", "LF", "RT", "SESS", "USR1", "USR2", "MIX"};
    for (int i=0; i<8; i++) {
        g.drawText(topLabels[i], topButtons[i]->getX(), topButtons[i]->getY() - 15, 40, 15, juce::Justification::centred);
    }
}

void ExtasisRhythmEditor::resized() {
    auto area = getLocalBounds().reduced(20);
    titleLabel.setBounds(area.removeFromTop(30));
    
    auto monitorArea = area.removeFromRight(250);
    monitorLog.setBounds(monitorArea.reduced(10));
    
    auto lpArea = area.withSizeKeepingCentre(450, 450);
    
    int padSize = 40;
    int gap = 10;
    
    int startX = lpArea.getX();
    int startY = lpArea.getY();
    
    // Top Row
    for (int i = 0; i < 8; ++i) {
        topButtons[i]->setBounds(startX + i * (padSize + gap), startY, padSize, padSize);
    }
    
    // Grid
    startY += padSize + gap + 15;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            gridPads[row * 8 + col]->setBounds(startX + col * (padSize + gap), startY + row * (padSize + gap), padSize, padSize);
        }
        // Right Side buttons
        rightButtons[row]->setBounds(startX + 8 * (padSize + gap) + 10, startY + row * (padSize + gap), padSize, padSize);
    }
}
