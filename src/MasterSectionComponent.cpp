#include "MasterSectionComponent.h"

MasterSectionComponent::MasterSectionComponent(ExtasisRhythmProcessor& p, juce::LookAndFeel_V4* kLaf, juce::LookAndFeel_V4* bLaf)
    : audioProcessor(p), knobLaf(kLaf), btnLaf(bLaf)
{
    auto setupMasterBtn = [this](juce::TextButton& btn, const juce::String& txt, bool isRadio) {
        addAndMakeVisible(btn);
        btn.setLookAndFeel(btnLaf);
        btn.setButtonText(txt);
        btn.setClickingTogglesState(true);
        if (isRadio) btn.setRadioGroupId(100);
        btn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffd4d4d4));
        btn.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xffe74c3c));
        btn.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        btn.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    };

    setupMasterBtn(bit16Button, "16B", true); 
    setupMasterBtn(bit12Button, "12B", true); 
    setupMasterBtn(bit8Button, "8B", true); 
    bit16Button.setToggleState(true, juce::dontSendNotification);

    auto setBits = [this](float bits) {
        if (auto* pParam = audioProcessor.apvts.getParameter("pcmBits")) {
            pParam->beginChangeGesture();
            pParam->setValueNotifyingHost(pParam->convertTo0to1(bits));
            pParam->endChangeGesture();
        }
    };

    bit16Button.onClick = [this, setBits] { setBits(16.0f); };
    bit12Button.onClick = [this, setBits] { setBits(12.0f); };
    bit8Button.onClick  = [this, setBits] { setBits(8.0f); };
    
    setupMasterBtn(analogButton, "ANLG", false); 
    setupMasterBtn(vinylMasterButton, "VNYL", false);
    setupMasterBtn(pumpButton, "PUMP", false); 
    setupMasterBtn(antiAliasButton, "ANTI", false);
    setupMasterBtn(limiterButton, "LIMIT", false);

    analogAtt     = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "masterAnalog", analogButton);
    vinylAtt      = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "masterVinyl", vinylMasterButton);
    pumpMasterAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "pumpOn", pumpButton);
    antiAtt       = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "masterAnti", antiAliasButton);
    limiterAtt    = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "masterLimiter", limiterButton);

    auto mkChannelKnob = [this] (juce::Slider& sl, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& at, const juce::String& id, juce::Colour col) {
        sl.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); 
        sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0); 
        sl.setLookAndFeel (knobLaf); 
        sl.setPopupDisplayEnabled (true, true, this); 
        sl.setNumDecimalPlacesToDisplay (2);
        addAndMakeVisible (sl); 
        sl.setColour(juce::Slider::rotarySliderFillColourId, col);
        at = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, id, sl);
    };

    mkChannelKnob (masterVolSlider, masterVolAtt, "masterVolume", juce::Colour (0xffe74c3c)); 
    mkChannelKnob (masterClipperSlider, masterClipAtt, "masterClipper", juce::Colour (0xffe74c3c));

    startTimerHz(30);
}

MasterSectionComponent::~MasterSectionComponent()
{
    masterVolSlider.setLookAndFeel(nullptr);
    masterClipperSlider.setLookAndFeel(nullptr);
    bit16Button.setLookAndFeel(nullptr);
    bit12Button.setLookAndFeel(nullptr);
    bit8Button.setLookAndFeel(nullptr);
    analogButton.setLookAndFeel(nullptr);
    vinylMasterButton.setLookAndFeel(nullptr);
    pumpButton.setLookAndFeel(nullptr);
    antiAliasButton.setLookAndFeel(nullptr);
    limiterButton.setLookAndFeel(nullptr);
}

void MasterSectionComponent::timerCallback()
{
    float currentBits = audioProcessor.apvts.getRawParameterValue("pcmBits")->load();
    bit16Button.setToggleState(currentBits >= 15.0f, juce::dontSendNotification);
    bit12Button.setToggleState(currentBits >= 11.0f && currentBits < 15.0f, juce::dontSendNotification);
    bit8Button.setToggleState(currentBits < 11.0f, juce::dontSendNotification);

    repaint();
}

void MasterSectionComponent::paint(juce::Graphics& g)
{
    int masterW = getWidth();
    int masterH = getHeight();
    
    g.setColour (juce::Colour (0xffd0d0d0));
    g.fillRoundedRectangle (0.0f, 0.0f, (float)masterW, (float)masterH, 4.0f);
    g.setColour (juce::Colours::black.withAlpha(0.2f)); 
    g.drawRoundedRectangle (0.0f, 0.0f, (float)masterW, (float)masterH, 4.0f, 1.0f);
    g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); 
    g.setColour (juce::Colour (0xff111111)); 
    g.drawText ("MASTER BUS", 0, 3, masterW, 14, juce::Justification::centred);

    g.setFont (juce::FontOptions (8.5f, juce::Font::bold));
    g.drawText ("VOL", 17, 64, 42, 10, juce::Justification::centred); 
    g.drawText ("CLIP", 71, 64, 42, 10, juce::Justification::centred);

    g.setColour (juce::Colour(0xff333333));
    float currentCpu = audioProcessor.cpuLoad.load();
    g.drawText ("CPU: " + juce::String (currentCpu, 1) + "%", 18, 159, 94, 12, juce::Justification::centred);

    int meterX = masterW - 19; 
    int meterY = 20; 
    g.setColour (juce::Colour (0xff111111)); 
    g.fillRect (meterX, meterY, 15, 134);
    g.setColour (juce::Colour (0xff333333)); 
    g.drawRect ((float)meterX, (float)meterY, 15.0f, 134.0f, 1.0f);

    float curRmsL = audioProcessor.outputLevelL.load(); 
    float curRmsR = audioProcessor.outputLevelR.load();
    auto rmsToNormalizedDb = [] (float rms) { 
        if (rms <= 0.00001f) return 0.0f; 
        float db = 20.0f * std::log10 (rms); 
        return juce::jlimit (0.0f, 1.0f, (db - (-48.0f)) / (3.0f - (-48.0f))); 
    };
    float mL = rmsToNormalizedDb (curRmsL); 
    float mR = rmsToNormalizedDb (curRmsR);
    int numSegments = 14; 
    float segHeight = (134.0f - 4.0f) / (float)numSegments;
    for (int seg = 0; seg < numSegments; ++seg) {
        float threshold = 1.0f - ((float)seg / (float)numSegments);
        juce::Colour ledColor; 
        if (threshold > 0.85f) ledColor = juce::Colour (0xffff3333); 
        else if (threshold > 0.65f) ledColor = juce::Colour (0xffffcc00); 
        else ledColor = juce::Colour (0xff2ecc71);
        float yP = (float)(meterY + 2) + (float)seg * segHeight;
        if (mL >= threshold) g.setColour (ledColor); else g.setColour (juce::Colour (0xff222222)); 
        g.fillRect ((float)(meterX + 2), yP, 5.0f, segHeight - 1.5f);
        if (mR >= threshold) g.setColour (ledColor); else g.setColour (juce::Colour (0xff222222)); 
        g.fillRect ((float)(meterX + 8), yP, 5.0f, segHeight - 1.5f);
    }
}

void MasterSectionComponent::resized()
{
    masterVolSlider.setBounds (17, 20, 42, 42); 
    masterClipperSlider.setBounds (71, 20, 42, 42);

    bit16Button.setBounds (19, 80, 28, 20);
    bit12Button.setBounds (51, 80, 28, 20);
    bit8Button.setBounds  (83, 80, 28, 20);
    analogButton.setBounds      (18, 102, 44, 17); 
    vinylMasterButton.setBounds (68, 102, 44, 17);
    pumpButton.setBounds        (18, 121, 44, 17); 
    antiAliasButton.setBounds   (68, 121, 44, 17);
    limiterButton.setBounds     (18, 140, 94, 17);
}
