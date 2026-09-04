#include "ChannelStripComponent.h"

ChannelStripComponent::ChannelStripComponent (ExtasisRhythmProcessor& p, int chIndex, juce::LookAndFeel* comboLaf, juce::LookAndFeel* knobLaf)
    : audioProcessor (p), channelIndex (chIndex)
{
    juce::String chStr = juce::String (channelIndex);
    juce::String labels[12] = { "KICK", "SNARE", "CLOSED HAT", "OPEN HAT", "CLAP", "RIMSHOT", "HI PERC", "MID PERC", "LOW PERC", "COWBELL", "CRASH", "RIDE" };
    channelLabel = labels[channelIndex];

    ledButton = std::make_unique<ChannelLedButton> (audioProcessor, channelIndex);
    addAndMakeVisible (*ledButton);
    setBufferedToImage(true);

    addAndMakeVisible (sampleSourceSelector);
    addAndMakeVisible (sampleVariantSelector);

    if (comboLaf) {
        sampleSourceSelector.setLookAndFeel(comboLaf);
        sampleVariantSelector.setLookAndFeel(comboLaf);
    }

    auto kitNames = audioProcessor.getDrumKitNames();
    sampleSourceSelector.addItemList (kitNames, 1);
    
    sampleSourceAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, "sampleSource_" + chStr, sampleSourceSelector);
    
    int initialChKit = (int) audioProcessor.apvts.getRawParameterValue ("sampleSource_" + chStr)->load();
    sampleSourceSelector.setSelectedId (initialChKit + 1, juce::dontSendNotification);
    
    auto initVariants = audioProcessor.getVariantsForChannel (initialChKit, channelIndex);
    for (int i = 0; i < initVariants.size(); ++i) {
        sampleVariantSelector.addItem(initVariants[i].upToLastOccurrenceOf(".wav", false, true).upToLastOccurrenceOf(".WAV", false, true), i + 1);
    }
    int initMatch = initVariants.indexOf (audioProcessor.currentSampleName[channelIndex]);
    sampleVariantSelector.setSelectedId (initMatch >= 0 ? initMatch + 1 : 1, juce::dontSendNotification);

    sampleSourceSelector.onChange = [this, chStr] {
        int kitIdx = sampleSourceSelector.getSelectedId() - 1;
        
        // 1. Force load a smart sample from the new folder so the sound immediately updates!
        audioProcessor.loadSmartSampleForChannel(channelIndex, kitIdx);
        
        // 2. Rebuild the GUI variant list without .wav
        sampleVariantSelector.clear (juce::dontSendNotification);
        auto variants = audioProcessor.getVariantsForChannel (kitIdx, channelIndex);
        for (int i = 0; i < variants.size(); ++i) {
            sampleVariantSelector.addItem(variants[i].upToLastOccurrenceOf(".wav", false, true).upToLastOccurrenceOf(".WAV", false, true), i + 1);
        }
        
        // 3. Select the correct newly loaded variant
        int match = variants.indexOf (audioProcessor.currentSampleName[channelIndex]);
        sampleVariantSelector.setSelectedId (match >= 0 ? match + 1 : 1, juce::dontSendNotification);
    };

    sampleVariantSelector.onChange = [this] {
        int kitIdx = sampleSourceSelector.getSelectedId() - 1;
        int varIdx = sampleVariantSelector.getSelectedId() - 1;
        auto variants = audioProcessor.getVariantsForChannel(kitIdx, channelIndex);
        if (varIdx >= 0 && varIdx < variants.size()) {
            audioProcessor.loadSampleForChannel(channelIndex, kitIdx, variants[varIdx]);
        }
    };

    auto setupKnob = [this, knobLaf](juce::Slider& sl, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& att, const juce::String& paramId, juce::Colour col) {
        addAndMakeVisible (sl);
        sl.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        sl.setColour(juce::Slider::rotarySliderFillColourId, col);
        if (knobLaf) sl.setLookAndFeel(knobLaf);
        att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, paramId, sl);
    };

    setupKnob (volSlider, volAtt, "gain" + chStr, juce::Colour(0xff3498db));
    setupKnob (panSlider, panAtt, "pan" + chStr, juce::Colour(0xff3498db));
    setupKnob (pitchSlider, pitchAtt, "pitch" + chStr, juce::Colour(0xff3498db));
    setupKnob (toneSlider, toneAtt, "tone" + chStr, juce::Colour(0xff3498db));
    setupKnob (attackSlider, attAtt, "attack" + chStr, juce::Colour(0xff3498db));
    setupKnob (decaySlider, decAtt, "decay" + chStr, juce::Colour(0xff3498db));
    setupKnob (springSendSlider, springSendAtt, "springSend" + chStr, juce::Colour(0xff556b2f));
    setupKnob (delaySendSlider, delaySendAtt, "delaySend" + chStr, juce::Colour(0xff00d2ff));

    auto setupBtn = [this](juce::TextButton& btn, std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>& att, const juce::String& paramId, const juce::String& text, juce::Colour onCol) {
        addAndMakeVisible (btn);
        btn.setButtonText (text);
        btn.setClickingTogglesState (true);
        btn.setColour (juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
        btn.setColour (juce::TextButton::buttonOnColourId, onCol);
        btn.setColour (juce::TextButton::textColourOffId, juce::Colours::grey);
        btn.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        att = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, paramId, btn);
    };

    setupBtn (muteButton, muteAtt, "mute" + chStr, "M", juce::Colour(0xffe74c3c));
    setupBtn (soloButton, soloAtt, "solo" + chStr, "S", juce::Colour(0xfff1c40f));
    setupBtn (envChannelButton, envChanAtt, "envChan_" + chStr, "ENV", juce::Colour(0xff3498db));
}

ChannelStripComponent::~ChannelStripComponent()
{
}

void ChannelStripComponent::paint (juce::Graphics& g)
{
    float s = (float)getWidth() / 92.0f;
    g.saveState();
    g.addTransform (juce::AffineTransform::scale (s));

    g.setColour (juce::Colours::black.withAlpha (0.15f)); 
    g.fillRoundedRectangle (3.0f, 19.0f, 92.0f, 258.0f, 4.0f);
    juce::ColourGradient cg (juce::Colour (0xfffcfcfc), 0.0f, 16.0f, juce::Colour (0xffe0e0e0), 0.0f, 272.0f, false);
    g.setGradientFill (cg); 
    g.fillRoundedRectangle (0.0f, 16.0f, 92.0f, 258.0f, 4.0f);
    
    g.setColour (juce::Colour (0xffb8b8b8)); 
    g.drawRoundedRectangle (0.0f, 16.0f, 92.0f, 258.0f, 4.0f, 1.0f);
    
    g.setColour (juce::Colours::white.withAlpha(0.9f)); 
    g.drawHorizontalLine (17, 2.0f, 90.0f);

    g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); 
    g.setColour (juce::Colours::black.withAlpha(0.4f)); 
    g.drawText (channelLabel, 5, 36, 82, 11, juce::Justification::centred);
    g.setColour (juce::Colour (0xff222222)); 
    g.drawText (channelLabel, 4, 35, 82, 11, juce::Justification::centred);

    g.setFont (juce::FontOptions (7.5f, juce::Font::bold)); 
    g.setColour (juce::Colour (0xff444444));
    g.drawText ("VOL",   4,  137, 40, 9, juce::Justification::centred);
    g.drawText ("PAN",   48, 137, 40, 9, juce::Justification::centred);
    g.drawText ("PITCH", 4,  178, 40, 9, juce::Justification::centred);
    g.drawText ("TONE",  48, 178, 40, 9, juce::Justification::centred);
    g.drawText ("ATT",   4,  219, 40, 9, juce::Justification::centred);
    g.drawText ("DEC",   48, 219, 40, 9, juce::Justification::centred);
    g.drawText ("SPRING",4,  260, 40, 9, juce::Justification::centred);
    g.drawText ("DELAY", 48, 260, 40, 9, juce::Justification::centred);
    
    g.restoreState();
}

void ChannelStripComponent::resized()
{
    float s = (float)getWidth() / 92.0f;
    auto sz = [s](int x, int y, int w, int h) { 
        return juce::Rectangle<int> ((int)(x*s), (int)(y*s), (int)(w*s), (int)(h*s)); 
    };

    ledButton->setBounds (sz(5, 17, 82, 16));
    
    muteButton.setBounds (sz(6, 48, 22, 15));
    soloButton.setBounds (sz(30, 48, 22, 15));
    envChannelButton.setBounds (sz(54, 48, 32, 15));
    
    sampleSourceSelector.setBounds (sz(6, 66, 80, 16));
    sampleVariantSelector.setBounds (sz(6, 84, 80, 16));
    
    int kSize = 34; // Set to original 34 to match text spacing
    
    volSlider.setBounds (sz(7, 103, kSize, kSize));
    panSlider.setBounds (sz(51, 103, kSize, kSize));
    
    pitchSlider.setBounds (sz(7, 144, kSize, kSize));
    toneSlider.setBounds (sz(51, 144, kSize, kSize));
    
    attackSlider.setBounds (sz(7, 185, kSize, kSize));
    decaySlider.setBounds (sz(51, 185, kSize, kSize));
    
    springSendSlider.setBounds (sz(7, 226, kSize, kSize));
    delaySendSlider.setBounds (sz(51, 226, kSize, kSize));
}

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
