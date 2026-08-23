#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <BinaryData.h>

class CustomComboBoxLookAndFeel : public juce::LookAndFeel_V4 
{
public:
    CustomComboBoxLookAndFeel() 
    {
        setColour (juce::PopupMenu::backgroundColourId, juce::Colour(0xfff5f5f5)); 
        setColour (juce::PopupMenu::textColourId, juce::Colour(0xff222222));
        setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff3498db));
        setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
        setColour (juce::ComboBox::backgroundColourId, juce::Colour(0xffe6e6e6));
        setColour (juce::ComboBox::textColourId, juce::Colours::black);
        setColour (juce::ComboBox::outlineColourId, juce::Colour(0xffb0b0b0));
        setColour (juce::ComboBox::arrowColourId, juce::Colour(0xff555555));
    }

    void drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box) override 
    {
        auto bounds = juce::Rectangle<int> (0, 0, width, height).toFloat();
        g.setColour (box.findColour(juce::ComboBox::backgroundColourId)); 
        g.fillRoundedRectangle (bounds, 3.0f);
        g.setColour (box.findColour(juce::ComboBox::outlineColourId)); 
        g.drawRoundedRectangle (bounds, 3.0f, 1.0f);
        
        juce::Path p; 
        p.addTriangle ((float)width - 10.0f, height * 0.4f, (float)width - 4.0f, height * 0.4f, (float)width - 7.0f, height * 0.65f);
        g.setColour (box.findColour(juce::ComboBox::arrowColourId)); 
        g.fillPath (p);
    }
    
    juce::Font getComboBoxFont (juce::ComboBox&) override 
    {
        return juce::FontOptions (10.5f, juce::Font::plain);
    }
};

class CompactButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override
    {
        return juce::FontOptions (juce::jlimit (8.5f, 11.0f, (float)buttonHeight * 0.58f), juce::Font::bold);
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool, bool) override
    {
        auto font = getTextButtonFont (button, button.getHeight());
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                                : juce::TextButton::textColourOffId)
                            .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

        g.drawFittedText (button.getButtonText(),
                          1, 0, button.getWidth() - 2, button.getHeight(),
                          juce::Justification::centred, 1);
    }
};

ExtasisRhythmEditor::ExtasisRhythmEditor (ExtasisRhythmProcessor& proc)
    : AudioProcessorEditor (&proc), audioProcessor (proc) 
{
    setResizable (true, true); 
    getConstrainer()->setFixedAspectRatio (1192.0f / 812.0f);
    getConstrainer()->setMinimumSize (900, 612);
    getConstrainer()->setMaximumSize (1788, 1218);
    setWantsKeyboardFocus (true);
    
    static EffectKnobLookAndFeel effectKnobLAF;
    static MasterChannelKnobLookAndFeel masterChannelKnobLAF;
    static CustomComboBoxLookAndFeel safeComboBoxLAF;
    static CompactButtonLookAndFeel compactBtnLAF;

    logoImage = juce::ImageFileFormat::loadFrom (BinaryData::logo_png, (size_t) BinaryData::logo_pngSize);

    addAndMakeVisible (playButton); 
    playButton.setButtonText ("PLAY"); 
    playButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0)); 
    playButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    playButton.onClick = [this] { 
        if (!audioProcessor.isSyncedToHost.load()) { 
            auto* pi = audioProcessor.apvts.getParameter ("isPlaying"); 
            if (pi) { 
                pi->beginChangeGesture(); 
                pi->setValueNotifyingHost (pi->getValue() > 0.5f ? 0.0f : 1.0f); 
                pi->endChangeGesture(); 
            } 
        } 
    };

    addAndMakeVisible (seqToggleViewButton);
    seqToggleViewButton.setButtonText ("SEQ");
    seqToggleViewButton.setClickingTogglesState (true);
    seqToggleViewButton.setToggleState (true, juce::dontSendNotification);
    seqToggleViewButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0));
    seqToggleViewButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff3498db));
    seqToggleViewButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    seqToggleViewButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    
    seqToggleViewButton.onClick = [this] {
        isSequencerVisible = seqToggleViewButton.getToggleState();
        
        getConstrainer()->setFixedAspectRatio (isSequencerVisible ? (1192.0f / 812.0f) : (1192.0f / 452.0f));
        
        if (isSequencerVisible) {
            getConstrainer()->setMinimumSize (900, 612);
            getConstrainer()->setMaximumSize (1788, 1218);
            int newHeight = (int)((float)getWidth() * (812.0f / 1192.0f));
            setSize (getWidth(), newHeight);
        } else {
            getConstrainer()->setMinimumSize (900, 341);
            getConstrainer()->setMaximumSize (1788, 678);
            int newHeight = (int)((float)getWidth() * (452.0f / 1192.0f));
            setSize (getWidth(), newHeight);
        }
    };

    addAndMakeVisible (stopButton); 
    stopButton.setButtonText ("STOP"); 
    stopButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0)); 
    stopButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    stopButton.onClick = [this] { 
        if (!audioProcessor.isSyncedToHost.load()) { 
            if (auto* pi = audioProcessor.apvts.getParameter ("isPlaying")) { 
                pi->beginChangeGesture(); 
                pi->setValueNotifyingHost (0.0f); 
                pi->endChangeGesture(); 
            } 
        } 
        for (int i = 0; i < 12; ++i) { 
            audioProcessor.channelSteps[i] = 0; 
            audioProcessor.samplePositions[i] = -1.0; 
        } 
        audioProcessor.killAllAudio();
    };

    addAndMakeVisible (bpmSlider); 
    bpmSlider.setSliderStyle (juce::Slider::LinearBar); 
    bpmSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 55, 18); 
    bpmSlider.setNumDecimalPlacesToDisplay(1); 
    bpmSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffff6600)); 
    bpmSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour(0xffeaeaea));
    bpmAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "bpm", bpmSlider);

    auto setupMasterBtn = [this](juce::TextButton& btn, const juce::String& txt, bool isRadio) {
        addAndMakeVisible(btn);
        btn.setLookAndFeel(&compactBtnLAF);
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
        if (auto* p = audioProcessor.apvts.getParameter("pcmBits")) {
            p->beginChangeGesture();
            p->setValueNotifyingHost(p->convertTo0to1(bits));
            p->endChangeGesture();
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

    auto setupFxBtn = [this](juce::TextButton& btn, const juce::String& txt, juce::Colour col) {
        addAndMakeVisible (btn); 
        btn.setLookAndFeel (&compactBtnLAF);
        btn.setButtonText (txt); 
        btn.setClickingTogglesState (true);
        btn.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff444444)); 
        btn.setColour (juce::TextButton::buttonOnColourId, col);
        btn.setColour (juce::TextButton::textColourOffId, juce::Colours::white); 
        btn.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    };

    setupFxBtn(flangerOnButton, "OFF", juce::Colour (0xff8e44ad)); 
    flangerOnButton.onClick = [this] { flangerOnButton.setButtonText (flangerOnButton.getToggleState() ? "ON" : "OFF"); }; 
    flangerOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "flangerOn", flangerOnButton);
    
    setupFxBtn(chorusOnButton, "OFF", juce::Colour (0xff2ecc71)); 
    chorusOnButton.onClick = [this] { chorusOnButton.setButtonText (chorusOnButton.getToggleState() ? "ON" : "OFF"); }; 
    chorusOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "chorusOn", chorusOnButton);

    setupFxBtn(delaySyncButton, "SYNC", juce::Colour (0xff00d2ff)); 
    delaySyncAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "delaySync", delaySyncButton);

    addAndMakeVisible (saveKitButton); 
    addAndMakeVisible (loadKitButton); 
    addAndMakeVisible (resetButton); 
    addAndMakeVisible (seqResetButton);
    
    saveKitButton.setButtonText ("SAVE"); 
    loadKitButton.setButtonText ("LOAD"); 
    resetButton.setButtonText ("RESET"); 
    seqResetButton.setButtonText ("SEQ RST");
    
    saveKitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff2e8b57)); 
    saveKitButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white); 
    
    loadKitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffd2691e)); 
    loadKitButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white); 
    
    resetButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffff6600)); 
    resetButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white); 
    
    seqResetButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff9b59b6)); 
    seqResetButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);

    resetButton.onClick = [this] { 
        audioProcessor.resetAllParameters(); 
        for (int i = 0; i < 12; ++i) {
            fitButtons[i].setToggleState (false, juce::dontSendNotification);
            fitButtons[i].setButtonText ("FIX");
            updateLengthLabel (i);
        }
        fillFitButton.setToggleState (false, juce::dontSendNotification);
        fillFitButton.setButtonText ("FIX");
        updateFillLengthLabel();
        resized();
    }; 

    seqResetButton.onClick = [this] { 
        audioProcessor.resetSequencer(); 
        fillFitButton.setToggleState (false, juce::dontSendNotification);
        fillFitButton.setButtonText ("FIX");
        updateFillLengthLabel();
        resized();
    };

    saveKitButton.onClick = [this] { 
        saveChooser = std::make_unique<juce::FileChooser> ("Save Project", juce::File::getSpecialLocation(juce::File::userHomeDirectory), "*.xml"); 
        saveChooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles, [this] (const juce::FileChooser& c) { 
            if (c.getResult() != juce::File()) audioProcessor.saveCustomPreset(c.getResult()); 
        }); 
    };
    
    loadKitButton.onClick = [this] { 
        loadChooser = std::make_unique<juce::FileChooser> ("Load Project", juce::File::getSpecialLocation(juce::File::userHomeDirectory), "*.xml"); 
        loadChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this] (const juce::FileChooser& c) { 
            if (c.getResult() != juce::File()) {
                audioProcessor.loadCustomPreset(c.getResult());
                int globalKit = (int)audioProcessor.apvts.getRawParameterValue("globalKitChoice")->load();
                globalKitSelector.setSelectedId(globalKit + 1, juce::dontSendNotification);
                for (int i = 0; i < 12; ++i) {
                    int chKit = (int)audioProcessor.apvts.getRawParameterValue("sampleSource_" + juce::String(i))->load();
                    sampleSourceSelectors[i].setSelectedId(chKit + 1, juce::dontSendNotification);
                    sampleVariantSelectors[i].clear(juce::dontSendNotification);
                    auto variants = audioProcessor.getVariantsForChannel(chKit, i);
                    sampleVariantSelectors[i].addItemList(variants, 1);
                    int matchIdx = variants.indexOf(audioProcessor.currentSampleName[i]);
                    sampleVariantSelectors[i].setSelectedId(matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
                }
                updateFillLengthLabel();
            }
        }); 
    };

    addAndMakeVisible (globalKitSelector); 
    globalKitSelector.setLookAndFeel (&safeComboBoxLAF); 
    globalKitSelector.addItemList (audioProcessor.getDrumKitNames(), 1); 
    globalKitAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, "globalKitChoice", globalKitSelector);
    
    int initialKit = (int)audioProcessor.apvts.getRawParameterValue("globalKitChoice")->load();
    globalKitSelector.setSelectedId(initialKit + 1, juce::dontSendNotification);

    globalKitSelector.onChange = [this] { 
        int kitIdx = globalKitSelector.getSelectedId() - 1; 
        if (kitIdx < 0) return;
        
        for (int i = 0; i < 12; ++i) {
            auto variants = audioProcessor.getVariantsForChannel(kitIdx, i);
            bool isValid = variants.contains(audioProcessor.currentSampleName[i]);
            
            if (!isValid) {
                audioProcessor.loadSmartSampleForChannel(i, kitIdx);
                variants = audioProcessor.getVariantsForChannel (kitIdx, i);
            }

            if (auto* sourceParam = audioProcessor.apvts.getParameter("sampleSource_" + juce::String(i))) {
                float normVal = sourceParam->convertTo0to1((float)kitIdx);
                if (std::abs(sourceParam->getValue() - normVal) > 0.001f) {
                    sourceParam->beginChangeGesture();
                    sourceParam->setValueNotifyingHost(normVal);
                    sourceParam->endChangeGesture();
                }
            }

            sampleSourceSelectors[i].setSelectedId (kitIdx + 1, juce::dontSendNotification); 
            sampleVariantSelectors[i].clear (juce::dontSendNotification);
            sampleVariantSelectors[i].addItemList (variants, 1);
            
            int matchIdx = variants.indexOf (audioProcessor.currentSampleName[i]); 
            if (matchIdx >= 0) {
                sampleVariantSelectors[i].setSelectedId (matchIdx + 1, juce::dontSendNotification);
            } else {
                sampleVariantSelectors[i].setText (audioProcessor.currentSampleName[i], juce::dontSendNotification);
            }
        }
    };

    addAndMakeVisible (randomKitButton); 
    randomKitButton.setButtonText ("RANDOM KIT"); 
    randomKitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff8e44ad)); 
    randomKitButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    randomKitButton.onClick = [this] {
        audioProcessor.randomizeKit();
        int kitIdx = (int)audioProcessor.apvts.getRawParameterValue("globalKitChoice")->load();
        for (int i = 0; i < 12; ++i) {
            sampleVariantSelectors[i].clear(juce::dontSendNotification);
            auto variants = audioProcessor.getVariantsForChannel(kitIdx, i); 
            sampleVariantSelectors[i].addItemList(variants, 1);
            int matchIdx = variants.indexOf(audioProcessor.currentSampleName[i]); 
            sampleVariantSelectors[i].setSelectedId(matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
        }
    };

    addAndMakeVisible (browseFolderButton);
    browseFolderButton.setButtonText (juce::CharPointer_UTF8 ("\xf0\x9f\x93\x81")); // 📁
    browseFolderButton.setTooltip ("Select Custom Samples Folder (Default: Documents/ExtasisRhythm_Samples)");
    browseFolderButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff34495e));
    browseFolderButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    browseFolderButton.onClick = [this] {
        folderChooser = std::make_unique<juce::FileChooser> (
            "Select Custom Samples Folder",
            audioProcessor.samplesFolder.isDirectory() ? audioProcessor.samplesFolder : juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
            "*"
        );
        folderChooser->launchAsync (
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
            [this] (const juce::FileChooser& fc) {
                auto result = fc.getResult();
                if (result.isDirectory()) {
                    audioProcessor.setSamplesFolder (result);
                    refreshKitSelectors();
                }
            }
        );
    };

    for (int p = 0; p < 4; ++p) {
        addAndMakeVisible (patternPageButtons[p]);
        patternPageButtons[p].setButtonText (juce::String (p + 1));
        patternPageButtons[p].setClickingTogglesState (true);
        patternPageButtons[p].setRadioGroupId(200);
        patternPageButtons[p].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
        patternPageButtons[p].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff3498db));
        patternPageButtons[p].setColour (juce::TextButton::textColourOffId, juce::Colours::black);
        patternPageButtons[p].setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        patternPageButtons[p].onClick = [this, p] { 
            currentPatternPage = p; 
            audioProcessor.changePattern (activePatternButton + (currentPatternPage * 8)); 
        };
    }
    patternPageButtons[0].setToggleState(true, juce::dontSendNotification);

    juce::String pNames = "ABCDEFGH";
    for (int i = 0; i < 8; ++i) {
        addAndMakeVisible (patternButtons[i]); 
        patternButtons[i].setButtonText (juce::String::charToString(pNames[i])); 
        patternButtons[i].setColour (juce::TextButton::buttonColourId, i == 0 ? juce::Colour (0xff3498db) : juce::Colour (0xffcccccc)); 
        patternButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black); 
        patternButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        patternButtons[i].onClick = [this, i] {
            for (int b = 0; b < 8; ++b) { 
                patternButtons[b].setColour (juce::TextButton::buttonColourId, b == i ? juce::Colour (0xff3498db) : juce::Colour (0xffcccccc)); 
                patternButtons[b].setColour (juce::TextButton::textColourOffId, juce::Colours::black); 
            }
            audioProcessor.changePattern (i + (currentPatternPage * 8)); 
            activePatternButton = i;
        };
    }

    auto mkChannelKnob = [this] (juce::Slider& sl, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& at, const juce::String& id, juce::Colour col) {
        sl.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); 
        sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0); 
        sl.setLookAndFeel (&masterChannelKnobLAF); 
        sl.setPopupDisplayEnabled (true, true, this); 
        sl.setNumDecimalPlacesToDisplay (2);
        addAndMakeVisible (sl); 
        sl.setColour(juce::Slider::rotarySliderFillColourId, col);
        at = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, id, sl);
    };

    auto mkEffectKnob = [this] (juce::Slider& sl, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& at, const juce::String& id, juce::Colour col) {
        sl.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); 
        sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0); 
        sl.setLookAndFeel (&effectKnobLAF); 
        sl.setPopupDisplayEnabled (true, true, this); 
        sl.setNumDecimalPlacesToDisplay (2);
        addAndMakeVisible (sl); 
        sl.setColour(juce::Slider::rotarySliderFillColourId, col);
        at = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, id, sl);
    };

    mkChannelKnob (masterVolSlider, masterVolAtt, "masterVolume", juce::Colour (0xffe74c3c)); 
    mkChannelKnob (masterClipperSlider, masterClipAtt, "masterClipper", juce::Colour (0xffe74c3c));

    mkEffectKnob (masterHpfSlider, hpfAtt, "masterHpf", juce::Colour (0xff3498db)); 
    mkEffectKnob (masterHpfResSlider, hpfResAtt, "masterHpfRes", juce::Colour (0xff3498db)); 
    mkEffectKnob (masterLpfSlider, lpfAtt, "masterLpf", juce::Colour (0xff3498db)); 
    mkEffectKnob (masterLpfResSlider, lpfResAtt, "masterLpfRes", juce::Colour (0xff3498db));
    
    mkEffectKnob (pcmBitsSlider, pcmBitsAtt, "pcmBits", juce::Colour (0xffff6600)); 
    mkEffectKnob (pcmRateSlider, pcmRateAtt, "pcmRate", juce::Colour (0xffff6600)); 
    
    mkEffectKnob (driveDistSlider, driveDistAtt, "driveDist", juce::Colour (0xffcc0000)); 
    mkEffectKnob (driveFilterSlider, driveFilterAtt, "driveFilter", juce::Colour (0xffcc0000)); 
    mkEffectKnob (driveVolSlider, driveVolAtt, "driveVol", juce::Colour (0xffcc0000)); 
    
    mkEffectKnob (flangerRateSlider, flRateAtt, "flangerRate", juce::Colour (0xff8e44ad)); 
    mkEffectKnob (flangerFbSlider, flFbAtt, "flangerFeedback", juce::Colour (0xff8e44ad)); 
    
    mkEffectKnob (springDecaySlider, springDecAtt, "springDecay", juce::Colour (0xff556b2f)); 
    mkEffectKnob (springToneSlider, springToneAtt, "springTone", juce::Colour (0xff556b2f)); 
    
    mkEffectKnob (pumpThrSlider, pumpThrAtt, "pumpThr", juce::Colour (0xffcc0000)); 
    mkEffectKnob (pumpAmtSlider, pumpAmtAtt, "pumpAmt", juce::Colour (0xffcc0000));
    
    mkEffectKnob (envFilterCutSlider, envCutAtt, "envFilterCut", juce::Colour (0xffff6600)); 
    mkEffectKnob (envFilterResSlider, envResAtt, "envFilterRes", juce::Colour (0xffff6600));
    
    mkEffectKnob (transAttackSlider, transAttAtt, "transientAttack", juce::Colour (0xffffb300)); 
    mkEffectKnob (transSustainSlider, transSusAtt, "transientSustain", juce::Colour (0xffffb300)); 
    
    mkEffectKnob (delayTimeSlider, delTimeAtt, "delayTime", juce::Colour (0xff00d2ff)); 
    mkEffectKnob (delayFbSlider, delFbAtt, "delayFb", juce::Colour (0xff00d2ff)); 
    mkEffectKnob (delayModRateSlider, delModRateAtt, "delayModRate", juce::Colour (0xff00d2ff)); 
    mkEffectKnob (delayModDepthSlider, delModDepthAtt, "delayModDepth", juce::Colour (0xff00d2ff)); 
    
    mkEffectKnob (chorusRateSlider, chorusRateAtt, "chorusRate", juce::Colour (0xff2ecc71)); 
    mkEffectKnob (chorusDepthSlider, chorusDepthAtt, "chorusDepth", juce::Colour (0xff2ecc71));

    addAndMakeVisible (fillFitButton);
    fillFitButton.setLookAndFeel (&compactBtnLAF);
    fillFitButton.setButtonText ("FIX");
    fillFitButton.setClickingTogglesState (true);
    fillFitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
    fillFitButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffe67e22));
    fillFitButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    fillFitButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    fillFitAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "fillFit", fillFitButton);
    fillFitButton.onClick = [this]() {
        bool isFit = fillFitButton.getToggleState();
        fillFitButton.setButtonText (isFit ? "FIT" : "FIX");
        resized();
    };

    addAndMakeVisible (fillSeqModeButton);
    fillSeqModeButton.setLookAndFeel (&compactBtnLAF);
    fillSeqModeButton.setButtonText ("FWD");
    fillSeqModeButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff336699));
    fillSeqModeButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    fillSeqModeButton.onClick = [this]() {
        fillSeqModeState = (fillSeqModeState + 1) % 4;
        audioProcessor.fillSeqMode.store (fillSeqModeState);
        static const char* labels[] = { "FWD", "REV", "RND", "PNB" };
        fillSeqModeButton.setButtonText (labels[fillSeqModeState]);
    };

    addAndMakeVisible (fillMinusButton);
    fillMinusButton.setLookAndFeel (&compactBtnLAF);
    fillMinusButton.setButtonText ("-");
    fillMinusButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
    fillMinusButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);

    addAndMakeVisible (fillPlusButton);
    fillPlusButton.setLookAndFeel (&compactBtnLAF);
    fillPlusButton.setButtonText ("+");
    fillPlusButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
    fillPlusButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);

    addAndMakeVisible (fillLengthLabel);
    fillLengthLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    fillLengthLabel.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    fillLengthLabel.setJustificationType (juce::Justification::centred);

    fillLengthAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "fillLength", hiddenFillLengthSlider);

    fillMinusButton.onClick = [this]() {
        int curLen = (int) audioProcessor.apvts.getRawParameterValue ("fillLength")->load();
        if (curLen <= 0) curLen = 16;
        int newLen = juce::jlimit (1, 16, curLen - 1);
        if (auto* p = audioProcessor.apvts.getParameter ("fillLength")) {
            p->beginChangeGesture();
            p->setValueNotifyingHost (p->convertTo0to1 ((float)newLen));
            p->endChangeGesture();
        }
        updateFillLengthLabel();
        resized();
    };

    fillPlusButton.onClick = [this]() {
        int curLen = (int) audioProcessor.apvts.getRawParameterValue ("fillLength")->load();
        if (curLen <= 0) curLen = 16;
        int newLen = juce::jlimit (1, 16, curLen + 1);
        if (auto* p = audioProcessor.apvts.getParameter ("fillLength")) {
            p->beginChangeGesture();
            p->setValueNotifyingHost (p->convertTo0to1 ((float)newLen));
            p->endChangeGesture();
        }
        updateFillLengthLabel();
        resized();
    };

    addAndMakeVisible (fillShiftLeftButton);
    fillShiftLeftButton.setLookAndFeel (&compactBtnLAF);
    fillShiftLeftButton.setButtonText ("<");
    fillShiftLeftButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
    fillShiftLeftButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    fillShiftLeftButton.onClick = [this]() {
        int numSteps = juce::jlimit (1, 16, (int) audioProcessor.apvts.getRawParameterValue ("fillLength")->load());
        if (numSteps <= 1) return;
        std::vector<int> vals (numSteps);
        for (int s = 0; s < numSteps; ++s) {
            auto* p = audioProcessor.apvts.getRawParameterValue ("fill_step_" + juce::String(s));
            vals[s] = p ? (int)(*p + 0.5f) : 0;
        }
        int firstVal = vals[0];
        for (int s = 0; s < numSteps - 1; ++s) vals[s] = vals[s + 1];
        vals[numSteps - 1] = firstVal;
        for (int s = 0; s < numSteps; ++s) {
            auto* p = audioProcessor.apvts.getParameter ("fill_step_" + juce::String(s));
            if (p) {
                p->beginChangeGesture();
                p->setValueNotifyingHost (p->convertTo0to1 ((float)vals[s]));
                p->endChangeGesture();
            }
        }
    };

    addAndMakeVisible (fillShiftRightButton);
    fillShiftRightButton.setLookAndFeel (&compactBtnLAF);
    fillShiftRightButton.setButtonText (">");
    fillShiftRightButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
    fillShiftRightButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    fillShiftRightButton.onClick = [this]() {
        int numSteps = juce::jlimit (1, 16, (int) audioProcessor.apvts.getRawParameterValue ("fillLength")->load());
        if (numSteps <= 1) return;
        std::vector<int> vals (numSteps);
        for (int s = 0; s < numSteps; ++s) {
            auto* p = audioProcessor.apvts.getRawParameterValue ("fill_step_" + juce::String(s));
            vals[s] = p ? (int)(*p + 0.5f) : 0;
        }
        int lastVal = vals[numSteps - 1];
        for (int s = numSteps - 1; s > 0; --s) vals[s] = vals[s - 1];
        vals[0] = lastVal;
        for (int s = 0; s < numSteps; ++s) {
            auto* p = audioProcessor.apvts.getParameter ("fill_step_" + juce::String(s));
            if (p) {
                p->beginChangeGesture();
                p->setValueNotifyingHost (p->convertTo0to1 ((float)vals[s]));
                p->endChangeGesture();
            }
        }
    };
    updateFillLengthLabel();

    for (int step = 0; step < 16; ++step) { 
        addAndMakeVisible (fillStepButtons[step]); 
        fillStepButtons[step].setWantsKeyboardFocus(false);
        updateFillButtonVisuals (step); 
        
        fillStepButtons[step].onClick = [this, step]() {
            auto paramName = "fill_step_" + juce::String(step);
            if (auto* p = audioProcessor.apvts.getParameter(paramName)) {
                int rawVal = (int) audioProcessor.apvts.getRawParameterValue(paramName)->load();
                int nextVal = (rawVal + 1) % 3; 
                p->beginChangeGesture();
                p->setValueNotifyingHost(p->convertTo0to1((float)nextVal));
                p->endChangeGesture();
                updateFillButtonVisuals(step);
            }
        };
    }

    auto kitNames = audioProcessor.getDrumKitNames();
    for (int i = 0; i < 12; ++i) {
        juce::String chStr = juce::String(i);
        
        channelLedButtons.push_back (std::make_unique<ChannelLedButton> (audioProcessor, i));
        addAndMakeVisible (*channelLedButtons[i]);

        addAndMakeVisible (sampleSourceSelectors[i]); 
        sampleSourceSelectors[i].setLookAndFeel (&safeComboBoxLAF); 
        sampleSourceSelectors[i].addItemList (kitNames, 1); 
        sampleSourceAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, "sampleSource_" + chStr, sampleSourceSelectors[i]);
        
        addAndMakeVisible (sampleVariantSelectors[i]); 
        sampleVariantSelectors[i].setLookAndFeel (&safeComboBoxLAF);

        int initialChKit = (int) audioProcessor.apvts.getRawParameterValue ("sampleSource_" + chStr)->load();
        sampleSourceSelectors[i].setSelectedId (initialChKit + 1, juce::dontSendNotification);
        auto initVariants = audioProcessor.getVariantsForChannel (initialChKit, i);
        sampleVariantSelectors[i].addItemList (initVariants, 1);
        int initMatch = initVariants.indexOf (audioProcessor.currentSampleName[i]);
        sampleVariantSelectors[i].setSelectedId (initMatch >= 0 ? initMatch + 1 : 1, juce::dontSendNotification);

        sampleSourceSelectors[i].onChange = [this, i]() {
            int kitIdx = sampleSourceSelectors[i].getSelectedId() - 1;
            if (kitIdx < 0) return;
            audioProcessor.loadSmartSampleForChannel (i, kitIdx);

            auto variants = audioProcessor.getVariantsForChannel (kitIdx, i);
            sampleVariantSelectors[i].clear (juce::dontSendNotification);
            sampleVariantSelectors[i].addItemList (variants, 1);
            int matchIdx = variants.indexOf (audioProcessor.currentSampleName[i]);
            sampleVariantSelectors[i].setSelectedId (matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
        };

        sampleVariantSelectors[i].onChange = [this, i]() {
            juce::String fileName = sampleVariantSelectors[i].getText();
            if (fileName.isEmpty()) return;
            int kitIdx = sampleSourceSelectors[i].getSelectedId() - 1;
            if (kitIdx < 0) return;
            audioProcessor.loadSampleForChannel (i, kitIdx, fileName);
        };
        
        addAndMakeVisible (muteButtons[i]); 
        muteButtons[i].setLookAndFeel (&compactBtnLAF);
        muteButtons[i].setButtonText ("M"); 
        muteButtons[i].setClickingTogglesState (true); 
        muteButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); 
        muteButtons[i].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffff3333)); 
        muteButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black); 
        muteButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white); 
        muteAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "mute" + chStr, muteButtons[i]);
        
        addAndMakeVisible (soloButtons[i]); 
        soloButtons[i].setLookAndFeel (&compactBtnLAF);
        soloButtons[i].setButtonText ("S"); 
        soloButtons[i].setClickingTogglesState (true); 
        soloButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); 
        soloButtons[i].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff2ecc71)); 
        soloButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black); 
        soloButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white); 
        soloAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "solo" + chStr, soloButtons[i]);
        
        addAndMakeVisible (envChannelButtons[i]); 
        envChannelButtons[i].setLookAndFeel (&compactBtnLAF);
        envChannelButtons[i].setButtonText ("ENV"); 
        envChannelButtons[i].setClickingTogglesState (true); 
        envChannelButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); 
        envChannelButtons[i].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffff8c00)); 
        envChannelButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black); 
        envChannelButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white); 
        envChanAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "envChan_" + chStr, envChannelButtons[i]);
        addAndMakeVisible (fitButtons[i]); 
        fitButtons[i].setLookAndFeel (&compactBtnLAF);
        fitButtons[i].setButtonText ("FIX"); 
        fitButtons[i].setClickingTogglesState (true); 
        fitButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); 
        fitButtons[i].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffe67e22)); 
        fitButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black); 
        fitButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        fitAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "fit" + chStr, fitButtons[i]);

        fitButtons[i].onClick = [this, i]() {
            bool isFit = fitButtons[i].getToggleState();
            fitButtons[i].setButtonText (isFit ? "FIT" : "FIX");
            if (!isFit) {
                int currentLen = (int) audioProcessor.apvts.getRawParameterValue("length" + juce::String(i))->load();
                if (currentLen > 16) {
                    if (auto* lenParam = audioProcessor.apvts.getParameter("length" + juce::String(i))) {
                        lenParam->beginChangeGesture();
                        lenParam->setValueNotifyingHost(lenParam->convertTo0to1(16.0f));
                        lenParam->endChangeGesture();
                    }
                }
            }
            updateLengthLabel(i);
            resized(); 
        };

        mkChannelKnob (volumeSliders[i], volAtts[i], "gain" + chStr, juce::Colour(0xff3498db)); 
        mkChannelKnob (panSliders[i], panAtts[i], "pan" + chStr, juce::Colour(0xff3498db));
        mkChannelKnob (pitchSliders[i], pitchAtts[i], "pitch" + chStr, juce::Colour(0xff3498db)); 
        mkChannelKnob (toneSliders[i], toneAtts[i], "tone" + chStr, juce::Colour(0xff3498db));
        mkChannelKnob (attackSliders[i], attAtts[i], "attack" + chStr, juce::Colour(0xff3498db)); 
        mkChannelKnob (decaySliders[i], decAtts[i], "decay" + chStr, juce::Colour(0xff3498db));
        mkChannelKnob (springSendSliders[i], springSendAtts[i], "springSend" + chStr, juce::Colour(0xff556b2f)); 
        mkChannelKnob (delaySendSliders[i], delaySendAtts[i], "delaySend" + chStr, juce::Colour(0xff00d2ff));
        
        stepLengthAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "length" + chStr, hiddenLengthSliders[i]);
        
        addAndMakeVisible (minusButtons[i]); 
        minusButtons[i].setLookAndFeel (&compactBtnLAF);
        minusButtons[i].setButtonText ("-"); 
        minusButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); 
        minusButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black);
        
        addAndMakeVisible (plusButtons[i]); 
        plusButtons[i].setLookAndFeel (&compactBtnLAF);
        plusButtons[i].setButtonText ("+"); 
        plusButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); 
        plusButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black);

        addAndMakeVisible (shiftLeftButtons[i]);
        shiftLeftButtons[i].setLookAndFeel (&compactBtnLAF);
        shiftLeftButtons[i].setButtonText ("<");
        shiftLeftButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
        shiftLeftButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black);
        
        shiftLeftButtons[i].onClick = [this, i]() {
            int maxLen = (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(i))->load();
            bool isFit = audioProcessor.apvts.getRawParameterValue ("fit" + juce::String(i))->load() > 0.5f;
            int numSteps = juce::jlimit (1, isFit ? 32 : 16, maxLen > 0 ? maxLen : 16);
            if (numSteps <= 1) return;

            std::vector<int> vals(numSteps);
            std::vector<int> gvals(numSteps);
            std::vector<int> nvals(numSteps);
            int pat = audioProcessor.getCurrentPattern();
            for (int s = 0; s < numSteps; ++s) {
                auto* p = audioProcessor.apvts.getRawParameterValue ("step_" + juce::String(i) + "_" + juce::String(s));
                vals[s] = p ? (int)(*p + 0.5f) : 0;
                gvals[s] = audioProcessor.savedGlides[pat][i][s];
                nvals[s] = audioProcessor.savedNotes[pat][i][s];
            }

            int firstVal = vals[0];
            int firstG = gvals[0];
            int firstN = nvals[0];
            for (int s = 0; s < numSteps - 1; ++s) {
                vals[s] = vals[s + 1];
                gvals[s] = gvals[s + 1];
                nvals[s] = nvals[s + 1];
            }
            vals[numSteps - 1] = firstVal;
            gvals[numSteps - 1] = firstG;
            nvals[numSteps - 1] = firstN;

            for (int s = 0; s < numSteps; ++s) {
                audioProcessor.savedGlides[pat][i][s] = gvals[s];
                audioProcessor.savedNotes[pat][i][s] = nvals[s];
                auto* p = audioProcessor.apvts.getParameter ("step_" + juce::String(i) + "_" + juce::String(s));
                if (p) {
                    p->beginChangeGesture();
                    p->setValueNotifyingHost (p->convertTo0to1((float)vals[s]));
                    p->endChangeGesture();
                }
            }
        };

        addAndMakeVisible (shiftRightButtons[i]);
        shiftRightButtons[i].setLookAndFeel (&compactBtnLAF);
        shiftRightButtons[i].setButtonText (">");
        shiftRightButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
        shiftRightButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black);

        shiftRightButtons[i].onClick = [this, i]() {
            int maxLen = (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(i))->load();
            bool isFit = audioProcessor.apvts.getRawParameterValue ("fit" + juce::String(i))->load() > 0.5f;
            int numSteps = juce::jlimit (1, isFit ? 32 : 16, maxLen > 0 ? maxLen : 16);
            if (numSteps <= 1) return;

            std::vector<int> vals(numSteps);
            std::vector<int> gvals(numSteps);
            std::vector<int> nvals(numSteps);
            int pat = audioProcessor.getCurrentPattern();
            for (int s = 0; s < numSteps; ++s) {
                auto* p = audioProcessor.apvts.getRawParameterValue ("step_" + juce::String(i) + "_" + juce::String(s));
                vals[s] = p ? (int)(*p + 0.5f) : 0;
                gvals[s] = audioProcessor.savedGlides[pat][i][s];
                nvals[s] = audioProcessor.savedNotes[pat][i][s];
            }

            int lastVal = vals[numSteps - 1];
            int lastG = gvals[numSteps - 1];
            int lastN = nvals[numSteps - 1];
            for (int s = numSteps - 1; s > 0; --s) {
                vals[s] = vals[s - 1];
                gvals[s] = gvals[s - 1];
                nvals[s] = nvals[s - 1];
            }
            vals[0] = lastVal;
            gvals[0] = lastG;
            nvals[0] = lastN;

            for (int s = 0; s < numSteps; ++s) {
                audioProcessor.savedGlides[pat][i][s] = gvals[s];
                audioProcessor.savedNotes[pat][i][s] = nvals[s];
                auto* p = audioProcessor.apvts.getParameter ("step_" + juce::String(i) + "_" + juce::String(s));
                if (p) {
                    p->beginChangeGesture();
                    p->setValueNotifyingHost (p->convertTo0to1((float)vals[s]));
                    p->endChangeGesture();
                }
            }
        };

        addAndMakeVisible (seqModeButtons[i]);
        seqModeButtons[i].setLookAndFeel (&compactBtnLAF);
        seqModeButtons[i].setButtonText ("FWD");
        seqModeButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xff336699));
        seqModeButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::white);
        seqModeButtons[i].onClick = [this, i]() {
            seqModes[i] = (seqModes[i] + 1) % 4;
            audioProcessor.seqModes[i].store (seqModes[i]);
            static const char* labels[] = { "FWD", "REV", "RND", "PNB" };
            seqModeButtons[i].setButtonText (labels[seqModes[i]]);
        };
        
        addAndMakeVisible (lengthLabels[i]); 
        lengthLabels[i].setColour (juce::Label::textColourId, juce::Colours::black); 
        lengthLabels[i].setFont (juce::FontOptions (10.0f, juce::Font::bold)); 
        lengthLabels[i].setJustificationType (juce::Justification::centred); 
        updateLengthLabel (i);

        minusButtons[i].onClick = [this, i]() {
            bool isFit = fitButtons[i].getToggleState();
            int maxAllowed = isFit ? 32 : 16;
            int currentLen = (int) audioProcessor.apvts.getRawParameterValue("length" + juce::String(i))->load();
            if (currentLen <= 0) currentLen = 16;
            int newLen = juce::jlimit(1, maxAllowed, currentLen - 1);
            if (auto* lenParam = audioProcessor.apvts.getParameter("length" + juce::String(i))) {
                lenParam->beginChangeGesture();
                lenParam->setValueNotifyingHost(lenParam->convertTo0to1((float)newLen));
                lenParam->endChangeGesture();
            }
            updateLengthLabel(i);
            resized(); 
        };

        plusButtons[i].onClick = [this, i]() {
            bool isFit = fitButtons[i].getToggleState();
            int maxAllowed = isFit ? 32 : 16;
            int currentLen = (int) audioProcessor.apvts.getRawParameterValue("length" + juce::String(i))->load();
            if (currentLen <= 0) currentLen = 16;
            int newLen = juce::jlimit(1, maxAllowed, currentLen + 1);
            if (auto* lenParam = audioProcessor.apvts.getParameter("length" + juce::String(i))) {
                lenParam->beginChangeGesture();
                lenParam->setValueNotifyingHost(lenParam->convertTo0to1((float)newLen));
                lenParam->endChangeGesture();
            }
            updateLengthLabel(i);
            resized(); 
        };

        for (int step = 0; step < 32; ++step) { 
            addAndMakeVisible (stepButtons[i][step]); 
            stepButtons[i][step].setWantsKeyboardFocus(false); 
            
            int pat = audioProcessor.getCurrentPattern();
            stepButtons[i][step].semitoneOffset = audioProcessor.savedNotes[pat][i][step];
            
            updateStepButtonVisuals (i, step); 
            
            stepButtons[i][step].onClick = [this, i, step]() {
                auto paramName = "step_" + juce::String(i) + "_" + juce::String(step);
                if (auto* p = audioProcessor.apvts.getParameter(paramName)) {
                    int rawVal = (int) audioProcessor.apvts.getRawParameterValue(paramName)->load();
                    int nextVal = (rawVal + 1) % 4; 
                    p->beginChangeGesture();
                    p->setValueNotifyingHost(p->convertTo0to1((float)nextVal));
                    p->endChangeGesture();
                    updateStepButtonVisuals(i, step);
                }
            };

            stepButtons[i][step].onNoteChanged = [this, i, step](int newOffset) {
                int pat = audioProcessor.getCurrentPattern();
                audioProcessor.savedNotes[pat][i][step] = newOffset;
                updateStepButtonVisuals(i, step);
            };

            stepButtons[i][step].onRightClick = [this, i, step]() {
                int pat = audioProcessor.getCurrentPattern();
                audioProcessor.savedGlides[pat][i][step] = audioProcessor.savedGlides[pat][i][step] ? 0 : 1;
                stepButtons[i][step].hasGlide = (audioProcessor.savedGlides[pat][i][step] != 0);
                stepButtons[i][step].repaint();
            };
        }
    }

    cornerResizer = std::make_unique<juce::ResizableCornerComponent> (this, getConstrainer());
    addAndMakeVisible (cornerResizer.get()); 

    // License & Registration UI
    addAndMakeVisible (licenseBadgeButton);
    licenseBadgeButton.setLookAndFeel (&compactBtnLAF);
    updateLicenseState();
    licenseBadgeButton.onClick = [this]() {
        showActivationModal = !showActivationModal;
        activationOverlay.statusLabel.setText ("", juce::dontSendNotification);
        resized();
        repaint();
    };

    addChildComponent (activationOverlay);
    activationOverlay.onActivate = [this](const juce::String& key) {
        if (LicenseManager::saveLicense (key))
        {
            updateLicenseState();
            activationOverlay.statusLabel.setText ("License Activated Successfully! Welcome to Extasis Rhythm.", juce::dontSendNotification);
            activationOverlay.statusLabel.setColour (juce::Label::textColourId, juce::Colour (0xff2ecc71));
            juce::Timer::callAfterDelay (1000, [this]() {
                showActivationModal = false;
                resized();
                repaint();
            });
        }
        else
        {
            activationOverlay.statusLabel.setText ("Invalid Serial Key. Please check and try again.", juce::dontSendNotification);
            activationOverlay.statusLabel.setColour (juce::Label::textColourId, juce::Colour (0xffff4444));
        }
    };
    activationOverlay.onContinueDemo = [this]() {
        showActivationModal = false;
        resized();
        repaint();
    };

    if (!isActivated)
    {
        showActivationModal = true;
    }

    setSize (1192, 812); 
    startTimerHz (30);
}

ExtasisRhythmEditor::~ExtasisRhythmEditor() = default;

void ExtasisRhythmEditor::updateLicenseState()
{
    isActivated = LicenseManager::isLicensed();
    audioProcessor.updateLicenseStatus();
    licenseBadgeButton.setVisible (!isActivated);
    if (!isActivated)
    {
        licenseBadgeButton.setButtonText ("ACTIVATE");
        licenseBadgeButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffe67e22));
        licenseBadgeButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    }
}

void ExtasisRhythmEditor::refreshKitSelectors()
{
    globalKitSelector.clear (juce::dontSendNotification);
    auto kitNames = audioProcessor.getDrumKitNames();
    globalKitSelector.addItemList (kitNames, 1);
    globalKitSelector.setSelectedId (1, juce::dontSendNotification);

    for (int i = 0; i < 12; ++i) {
        sampleSourceSelectors[i].clear (juce::dontSendNotification);
        sampleSourceSelectors[i].addItemList (kitNames, 1);
        sampleSourceSelectors[i].setSelectedId (1, juce::dontSendNotification);

        sampleVariantSelectors[i].clear (juce::dontSendNotification);
        auto variants = audioProcessor.getVariantsForChannel (0, i);
        sampleVariantSelectors[i].addItemList (variants, 1);
        int matchIdx = variants.indexOf (audioProcessor.currentSampleName[i]);
        sampleVariantSelectors[i].setSelectedId (matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
    }
}

void ExtasisRhythmEditor::updateStepButtonVisuals (int ch, int step) 
{ 
    auto* par = audioProcessor.apvts.getRawParameterValue ("step_" + juce::String(ch) + "_" + juce::String(step)); 
    if (par) { 
        int v = (int)(*par + 0.5f); 
        juce::Colour cols[4] = { 
            juce::Colour(0xffe0e0e0), 
            juce::Colour(0xffffcc00), 
            juce::Colour(0xffff6600), 
            juce::Colour(0xffff0000) 
        }; 
        stepButtons[ch][step].setColour (juce::TextButton::buttonColourId, cols[v]); 
        
        int pat = audioProcessor.getCurrentPattern();
        bool hasGlide = (audioProcessor.savedGlides[pat][ch][step] != 0);
        int semitone = audioProcessor.savedNotes[pat][ch][step];
        
        stepButtons[ch][step].hasGlide = hasGlide;
        stepButtons[ch][step].semitoneOffset = semitone;

        juce::String labels[12] = { "KICK", "SNARE", "CLOSED HAT", "OPEN HAT", "CLAP", "RIMSHOT", "HI PERC", "MID PERC", "LOW PERC", "COWBELL", "CRASH", "RIDE" };
        
        float basePitchParam = audioProcessor.apvts.getRawParameterValue ("pitch" + juce::String(ch))->load();
        int rootMidiNote = 60; 
        int totalSemitones = (int)std::round(basePitchParam) + semitone;
        int finalMidiNote = juce::jlimit(0, 127, rootMidiNote + totalSemitones);

        static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        int octave = (finalMidiNote / 12) - 1;
        juce::String midiNoteName = juce::String(noteNames[finalMidiNote % 12]) + juce::String(octave);

        stepButtons[ch][step].customNoteText = midiNoteName;

        juce::String offsetStr = (semitone == 0 ? "0 st" : (semitone > 0 ? "+" + juce::String(semitone) + " st" : juce::String(semitone) + " st"));
        juce::String tooltipText = labels[ch] + " | Paso " + juce::String (step + 1) + 
                                   " | Nota: " + midiNoteName + " (" + offsetStr + ")" +
                                   (hasGlide ? " | Glide: ON" : "");
        
        stepButtons[ch][step].setTooltip (tooltipText);
        stepButtons[ch][step].repaint();
    } 
}

void ExtasisRhythmEditor::updateFillButtonVisuals (int step) 
{ 
    auto* par = audioProcessor.apvts.getRawParameterValue ("fill_step_" + juce::String(step)); 
    if (par) { 
        int v = (int)(*par + 0.5f); 
        fillStepButtons[step].setButtonText (""); 
        juce::Colour cols[3] = { 
            juce::Colour(0xffe0e0e0), 
            juce::Colour(0xff2ecc71), 
            juce::Colour(0xff3498db) 
        }; 
        fillStepButtons[step].setColour (juce::TextButton::buttonColourId, cols[v]); 
    } 
}

void ExtasisRhythmEditor::updateLengthLabel (int i) 
{ 
    int len = (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(i))->load(); 
    if (len <= 0) len = 16; 
    lengthLabels[i].setText (juce::String (len), juce::dontSendNotification); 
}

void ExtasisRhythmEditor::updateFillLengthLabel()
{
    int len = (int) audioProcessor.apvts.getRawParameterValue ("fillLength")->load();
    if (len <= 0) len = 16;
    fillLengthLabel.setText (juce::String (len), juce::dontSendNotification);
}

void ExtasisRhythmEditor::timerCallback() 
{ 
    bool isPlaying = (audioProcessor.isSyncedToHost.load() ? audioProcessor.hostPlaying.load() 
                                                           : (audioProcessor.apvts.getRawParameterValue("isPlaying")->load() > 0.5f));

    for (int i = 0; i < 12; ++i) { 
        if (audioProcessor.flashCounters[i].load() > 0)
            channelLedButtons[i]->repaint();
    }

    float currentBits = audioProcessor.apvts.getRawParameterValue("pcmBits")->load();
    bit16Button.setToggleState(currentBits >= 15.0f, juce::dontSendNotification);
    bit12Button.setToggleState(currentBits >= 11.0f && currentBits < 15.0f, juce::dontSendNotification);
    bit8Button.setToggleState(currentBits < 11.0f, juce::dontSendNotification);

    if (audioProcessor.isSyncedToHost.load()) {
        double hb = audioProcessor.hostBpm.load(); 
        if (std::abs(bpmSlider.getValue() - hb) > 0.01) {
            bpmSlider.setValue (hb, juce::dontSendNotification);
        }
        if (audioProcessor.hostPlaying.load()) { 
            playButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff2ecc71)); 
            playButton.setButtonText ("HOST SYNC"); 
        } else { 
            playButton.setColour (juce::TextButton::buttonColourId, isPlaying ? juce::Colour (0xff3498db) : juce::Colour (0xffb0b0b0)); 
            playButton.setButtonText (isPlaying ? "PLAYING" : "PLAY");
        }
    } else {
        playButton.setColour (juce::TextButton::buttonColourId, isPlaying ? juce::Colour (0xff3498db) : juce::Colour (0xffb0b0b0)); 
        playButton.setButtonText (isPlaying ? "PLAYING" : "PLAY");
    }

    if (!isActivated)
    {
        if (audioProcessor.demoExpired.load())
        {
            if (!showActivationModal || !activationOverlay.isExpired)
            {
                showActivationModal = true;
                activationOverlay.isExpired = true;
                activationOverlay.statusLabel.setText ("Demo time limit reached (10 min). Enter license key to continue.", juce::dontSendNotification);
                activationOverlay.statusLabel.setColour (juce::Label::textColourId, juce::Colour (0xffff4444));
                activationOverlay.setVisible (true);
                activationOverlay.toFront (true);
                resized();
            }
        }
    }

    repaint(); 
}

void ExtasisRhythmEditor::paint (juce::Graphics& g)
{
    float s = (float) getWidth() / 1192.0f; 
    g.saveState(); 
    g.addTransform (juce::AffineTransform::scale (s));
    
    g.fillAll (juce::Colour (0xffe6e6e6)); 
    g.setColour (juce::Colour (0xffd4d4d4)); 
    g.fillRect (0, 0, 1192, 180); 
    g.setColour (juce::Colours::black.withAlpha (0.15f)); 
    g.fillRect (0, 180, 1192, 4); 

    g.setFont (juce::FontOptions (25.0f, juce::Font::bold)); 
    g.setColour (juce::Colours::black.withAlpha(0.18f)); 
    g.drawText ("ExtasisRhythm", 17, 14, 180, 30, juce::Justification::left); 
    g.setColour (juce::Colour (0xff00d2ff)); 
    g.drawText ("ExtasisRhythm", 15, 12, 180, 30, juce::Justification::left);

    g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); 
    g.setColour (juce::Colour (0xff666666)); 
    g.drawText ("v2.0", 200, 20, 35, 14, juce::Justification::left);
    
    g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); 
    g.setColour (juce::Colour (0xff333333)); 
    g.drawText ("coded by @laurorobles", 16, 45, 180, 14, juce::Justification::left);
    
    g.setFont (juce::FontOptions (9.5f, juce::Font::plain)); 
    g.setColour (juce::Colour (0xff666666)); 
    g.drawText ("extasisrecords.bandcamp.com", 16, 60, 200, 14, juce::Justification::left);

    if (logoImage.isValid())
    {
        auto logoRect = juce::Rectangle<float> (35.0f, 78.0f, 120.0f, 88.0f);
        auto placement = juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize;

        // 1. Black drop shadow strictly in the background (offset by +2px)
        g.setColour (juce::Colours::black.withAlpha (0.45f));
        g.drawImage (logoImage, logoRect.translated (2.0f, 2.0f), placement, true);

        // 2. Solid pure white backing mask directly under the logo
        // (Prevents the black shadow or gray background from bleeding through semi-transparent logo pixels)
        g.setColour (juce::Colours::white);
        g.drawImage (logoImage, logoRect, placement, true);

        // 3. 100% pure, bright, original color logo in the foreground
        g.drawImage (logoImage, logoRect, placement, false);
    }

    g.setColour (juce::Colours::white.withAlpha(0.5f)); 
    g.drawHorizontalLine (181, 0.0f, 1192.0f); 
    g.setColour (juce::Colours::black.withAlpha(0.3f)); 
    g.drawHorizontalLine (179, 0.0f, 1192.0f);

    auto drawModuleBox = [&g] (int x, int y, int w, int h, const juce::String& title, juce::Colour boxTint, bool showLed = false, bool ledState = false) {
        g.setColour (boxTint); 
        g.fillRoundedRectangle ((float)x, (float)y, (float)w, (float)h, 4.0f);
        g.setColour (juce::Colours::black.withAlpha(0.2f)); 
        g.drawRoundedRectangle ((float)x, (float)y, (float)w, (float)h, 4.0f, 1.0f);
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); 
        g.setColour (juce::Colour (0xff111111)); 
        g.drawText (title, x, y + 3, w, 14, juce::Justification::centred);
        if (showLed) { 
            g.setColour (ledState ? juce::Colour (0xffcc0000) : juce::Colour (0xff555555)); 
            g.fillEllipse ((float)(x + 7), (float)(y + 7), 5.5f, 5.5f); 
        }
    };

    auto getModuleLayout = [](int mh, int numKnobs, int customKnobSize, bool hasTopButton = false, int rowGap = 12) {
        int cols = juce::jmin(numKnobs, 2);
        int knobSize = customKnobSize;  
        int rows = (numKnobs + cols - 1) / cols;
        int totalContentHeight = rows * (knobSize + rowGap) - 2; 
        int titleOffset = hasTopButton ? 36 : 18; 
        int availableHeight = mh - titleOffset;
        int topMargin = titleOffset + (availableHeight - totalContentHeight) / 2;
        return std::make_pair(knobSize, topMargin);
    };

    auto drawModLabels = [&](int mx, int my, int mw, int mh, std::vector<juce::String> texts, int customKnobSize, bool hasTopButton = false, int rowGap = 12) {
        g.setFont (juce::FontOptions (8.5f, juce::Font::bold)); 
        g.setColour (juce::Colour (0xff222222));
        int numKnobs = (int)texts.size();
        auto layout = getModuleLayout(mh, numKnobs, customKnobSize, hasTopButton, rowGap);
        int knobSize = layout.first;
        int topMargin = layout.second;
        int cols = juce::jmin(numKnobs, 2);

        for(int i = 0; i < numKnobs; ++i) {
            int r = i / cols; 
            int c = i % cols;
            float relX = (cols == 1) ? 0.5f : (c == 0 ? 0.33f : 0.67f);
            if (numKnobs == 3 && i == 2) relX = 0.5f; 
            int cx = mx + (int)(mw * relX);
            int cy = my + topMargin + (r * (knobSize + rowGap));
            g.drawText (texts[i], cx - 22, cy + knobSize - 3, 44, 10, juce::Justification::centred);
        }
    };

    int rightMargin = 16;
    int masterW = 146;
    int fxW = 96;
    int patternW = 124;
    int gap = 2; 

    int masterX = 1192 - rightMargin - masterW;                
    int fxStartX = masterX - (fxW * 5) - (gap * 5);            
    int patternX = fxStartX - patternW - gap;                  

    drawModuleBox (patternX, 2, patternW, 176, "PATTERNS", juce::Colour (0xffd8d8d8)); 

    drawModuleBox (fxStartX,         2, fxW, 86, "FILTER", juce::Colour (0xffd8d8d8));       
    drawModLabels (fxStartX,         2, fxW, 86, {"HPF", "H.RES", "LPF", "L.RES"}, 26, false, 7);
    
    drawModuleBox (fxStartX + fxW+gap,  2, fxW, 86, "PCM", juce::Colour (0xffd8d8d8));          
    drawModLabels (fxStartX + fxW+gap,  2, fxW, 86, {"BITS", "RATE"}, 28, false, 12);
    
    drawModuleBox (fxStartX + (fxW+gap)*2, 2, fxW, 86, "OVERDRIVE", juce::Colour (0xffd8d8d8));   
    drawModLabels (fxStartX + (fxW+gap)*2, 2, fxW, 86, {"DIST", "FLTR", "VOL"}, 26, false, 7); 
    
    drawModuleBox (fxStartX + (fxW+gap)*3, 2, fxW, 86, "TRANS", juce::Colour (0xffd8d8d8));      
    drawModLabels (fxStartX + (fxW+gap)*3, 2, fxW, 86, {"ATT", "SUS"}, 28, false, 12);
    
    drawModuleBox (fxStartX + (fxW+gap)*4, 2, fxW, 86, "ENV FILTER", juce::Colour (0xffd8d8d8));  
    drawModLabels (fxStartX + (fxW+gap)*4, 2, fxW, 86, {"CUT", "RES"}, 28, false, 12);

    drawModuleBox (fxStartX,         90, fxW, 88, "PUMP", juce::Colour (0xffd8d8d8), true, pumpButton.getToggleState());       
    drawModLabels (fxStartX,         90, fxW, 88, {"THR", "AMT"}, 28, false, 12);
    
    drawModuleBox (fxStartX + fxW+gap,  90, fxW, 88, "FLANGER", juce::Colour (0xffd8d8d8), true, flangerOnButton.getToggleState()); 
    drawModLabels (fxStartX + fxW+gap,  90, fxW, 88, {"RATE", "FB"}, 28, true, 12);
    
    drawModuleBox (fxStartX + (fxW+gap)*2, 90, fxW, 88, "CE CHORUS", juce::Colour (0xffd8d8d8), true, chorusOnButton.getToggleState()); 
    drawModLabels (fxStartX + (fxW+gap)*2, 90, fxW, 88, {"RATE", "DEPTH"}, 28, true, 12);
    
    drawModuleBox (fxStartX + (fxW+gap)*3, 90, fxW, 88, "DELAY", juce::Colour (0xffd8d8d8), true, false); 
    drawModLabels (fxStartX + (fxW+gap)*3, 90, fxW, 88, {"TIME", "FB", "MOD"}, 26, false, 7); 
    
    drawModuleBox (fxStartX + (fxW+gap)*4, 90, fxW, 88, "SPRING", juce::Colour (0xffd8d8d8));      
    drawModLabels (fxStartX + (fxW+gap)*4, 90, fxW, 88, {"DEC", "TONE"}, 28, false, 12);

    drawModuleBox (masterX, 2, masterW, 176, "MASTER BUS", juce::Colour (0xffd0d0d0));
    g.drawText ("VOL", masterX + 17, 66, 42, 10, juce::Justification::centred); 
    g.drawText ("CLIP", masterX + 71, 66, 42, 10, juce::Justification::centred);

    g.setFont (juce::FontOptions (8.5f, juce::Font::bold));
    g.setColour (juce::Colour(0xff333333));
    float currentCpu = audioProcessor.cpuLoad.load();
    g.drawText ("CPU: " + juce::String (currentCpu, 1) + "%", masterX + 18, 161, 94, 12, juce::Justification::centred);

    int meterX = masterX + masterW - 19; 
    int meterY = 22; 
    g.setColour (juce::Colour (0xff111111)); 
    g.fillRect (meterX, meterY, 15, 134); // Reducido para dar espacio al medidor CPU
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

    juce::String labels[12] = { "KICK", "SNARE", "CLOSED HAT", "OPEN HAT", "CLAP", "RIMSHOT", "HI PERC", "MID PERC", "LOW PERC", "COWBELL", "CRASH", "RIDE" };
    
    for (int i = 0; i < 12; ++i) {
        int x = 10 + i * 98; 
        g.setColour (juce::Colours::black.withAlpha (0.15f)); 
        g.fillRoundedRectangle ((float)(x + 3), 189.0f, 92.0f, 256.0f, 4.0f);
        juce::ColourGradient cg (juce::Colour (0xfffcfcfc), (float)x, 186.0f, juce::Colour (0xffe0e0e0), (float)x, 442.0f, false);
        g.setGradientFill (cg); 
        g.fillRoundedRectangle ((float)x, 186.0f, 92.0f, 256.0f, 4.0f);
        g.setColour (juce::Colour (0xffb8b8b8)); 
        g.drawRoundedRectangle ((float)x, 186.0f, 92.0f, 256.0f, 4.0f, 1.0f);
        g.setColour (juce::Colours::white.withAlpha(0.9f)); 
        g.drawHorizontalLine (187, (float)(x + 2), (float)(x + 90));

        g.setColour (juce::Colours::black.withAlpha(0.4f)); 
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); 
        g.drawText (labels[i], x + 5, 204, 82, 11, juce::Justification::centred);
        g.setColour (juce::Colour (0xff222222)); 
        g.drawText (labels[i], x + 4, 203, 82, 11, juce::Justification::centred);

        g.setFont (juce::FontOptions (7.5f, juce::Font::bold)); 
        g.setColour (juce::Colour (0xff444444));
        g.drawText ("VOL",   x + 4,  305, 40, 9, juce::Justification::centred); 
        g.drawText ("PAN",   x + 48, 305, 40, 9, juce::Justification::centred);
        g.drawText ("PITCH", x + 4,  346, 40, 9, juce::Justification::centred); 
        g.drawText ("TONE",  x + 48, 346, 40, 9, juce::Justification::centred);
        g.drawText ("ATT",   x + 4,  387, 40, 9, juce::Justification::centred); 
        g.drawText ("DEC",   x + 48, 387, 40, 9, juce::Justification::centred);
        g.drawText ("SPR",   x + 4,  428, 40, 9, juce::Justification::centred); 
        g.drawText ("DLY",   x + 48, 428, 40, 9, juce::Justification::centred);
    }
    
    if (isSequencerVisible) {
        int seqX = 15; 
        int seqY = 452; 
        int seqW = 1162; 
        int seqH = 345;
        g.setColour (juce::Colour (0xffd8d8d8)); 
        g.fillRoundedRectangle ((float)seqX, (float)seqY, (float)seqW, (float)seqH, 6.0f);
        g.setColour (juce::Colours::black.withAlpha(0.2f)); 
        g.drawRoundedRectangle ((float)seqX, (float)seqY, (float)seqW, (float)seqH, 6.0f, 2.0f);

        int controlsAreaWidth = (int)((float)seqW * 0.20f); 
        int stepsStartX = seqX + controlsAreaWidth + 8;
        int totalStepsWidth = seqW - controlsAreaWidth - 16;
        
        for (int i = 0; i < 12; ++i) {
            g.setColour (juce::Colours::black.withAlpha(0.4f)); 
            g.setFont (juce::FontOptions (9.5f, juce::Font::bold)); 
            g.drawText (labels[i], seqX + 6, seqY + 8 + (i * 24), 74, 20, juce::Justification::centredLeft);
        }
        
        g.setColour (juce::Colours::black.withAlpha(0.4f)); 
        g.setFont (juce::FontOptions (9.5f, juce::Font::bold)); 
        g.drawText ("FILL", seqX + 6, seqY + 10 + (12 * 24), 74, 20, juce::Justification::centredLeft);

        int stepW_int = totalStepsWidth / 16; 
        int metricSteps[4] = { 0, 4, 8, 12 }; 
        g.setFont (juce::FontOptions (9.0f, juce::Font::bold));
        for (int m = 0; m < 4; ++m) {
            float stepXCenter = (float)(stepsStartX + (metricSteps[m] * stepW_int)) + ((float)stepW_int * 0.5f); 
            float indicatorY = (float)(seqY + 325); 
            g.setColour (juce::Colour (0xff3498db)); 
            g.fillEllipse (stepXCenter - 2.5f, indicatorY, 5.0f, 5.0f);
            g.setColour (juce::Colour (0xff333333)); 
            g.drawText (juce::String (m + 1), (int)(stepXCenter - 15.0f), (int)(indicatorY + 4.0f), 30, 12, juce::Justification::centred);
        }

        for (int i = 0; i < 12; ++i) {
            int act = audioProcessor.channelSteps[i].load(); 
            bool isFitMode = fitButtons[i].getToggleState();
            int maxAllowed = isFitMode ? 32 : 16;
            int maxLen = (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(i))->load(); 
            int numSteps = juce::jlimit (1, maxAllowed, maxLen > 0 ? maxLen : 16);

            int stepW_act = isFitMode ? (totalStepsWidth / numSteps) : (totalStepsWidth / 16);
            int btnGap = (numSteps > 20) ? 2 : 4;
            int btnW_act = juce::jmax (2, stepW_act - btnGap);

            int exactStepX = stepsStartX + (act * stepW_act);
            auto curStepBounds = juce::Rectangle<float> ((float)(exactStepX), (float)(seqY + 8 + (i * 24)), (float)(btnW_act), 20.0f);
            g.setColour (juce::Colour (0x663498db)); 
            g.fillRoundedRectangle (curStepBounds, 3.0f);
            g.setColour (juce::Colour (0xff3498db)); 
            g.drawRoundedRectangle (curStepBounds, 3.0f, 2.0f);
        }

        int fillAct = audioProcessor.fillSeqPos;
        int fillNumSteps = juce::jlimit(1, 16, (int) audioProcessor.apvts.getRawParameterValue("fillLength")->load());
        bool isFillFit = fillFitButton.getToggleState();
        int fillStepW = isFillFit ? (totalStepsWidth / fillNumSteps) : (totalStepsWidth / 16);
        int fillBtnW = juce::jmax(2, fillStepW - 4);
        int exactFillX = stepsStartX + (fillAct * fillStepW);
        auto curFillBounds = juce::Rectangle<float> ((float)(exactFillX), (float)(seqY + 10 + (12 * 24)), (float)(fillBtnW), 20.0f);
        g.setColour (juce::Colour (0x662ecc71));
        g.fillRoundedRectangle (curFillBounds, 3.0f);
        g.setColour (juce::Colour (0xff2ecc71));
        g.drawRoundedRectangle (curFillBounds, 3.0f, 2.0f);
    }
    
    g.restoreState();
}

void ExtasisRhythmEditor::resized() 
{
    float s = (float) getWidth() / 1192.0f;
    auto sz = [s](int x, int y, int w, int h) { 
        return juce::Rectangle<int> ((int)((float)x*s), (int)((float)y*s), (int)((float)w*s), (int)((float)h*s)); 
    };

    licenseBadgeButton.setBounds (sz (180, 42, 68, 16));

    activationOverlay.setBounds (0, 0, getWidth(), getHeight());
    activationOverlay.setVisible (showActivationModal);
    if (showActivationModal)
        activationOverlay.toFront (true);

    int rightMargin = 16;
    int masterW = 146;
    int fxW = 96;
    int patternW = 124;
    int controlsW = 136; 
    int gap = 2; 

    int masterX = 1192 - rightMargin - masterW;                
    int fxStartX = masterX - (fxW * 5) - (gap * 5);            
    int patternX = fxStartX - patternW - gap;                  
    int controlsX = patternX - controlsW - gap;                

    playButton.setBounds (sz(controlsX, 14, 60, 24)); 
    stopButton.setBounds (sz(controlsX + 64, 14, 60, 24));
    resetButton.setBounds (sz(controlsX, 42, 60, 24)); 
    seqResetButton.setBounds (sz(controlsX + 64, 42, 60, 24));
    bpmSlider.setBounds (sz(controlsX, 70, 124, 24)); 
    globalKitSelector.setBounds (sz(controlsX, 100, 98, 22)); 
    browseFolderButton.setBounds (sz(controlsX + 101, 100, 23, 22));
    randomKitButton.setBounds (sz(controlsX, 128, 70, 24));
    seqToggleViewButton.setBounds (sz(controlsX + 74, 128, 50, 24));

    for (int pIdx = 0; pIdx < 8; ++pIdx) {
        int row = pIdx / 4; 
        int col = pIdx % 4;
        patternButtons[pIdx].setBounds (sz(patternX + 8 + (col * 27), 24 + (row * 24), 22, 20)); 
        patternButtons[pIdx].setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    }
    
    for (int p = 0; p < 4; ++p) { 
        patternPageButtons[p].setBounds (sz(patternX + 8 + (p * 27), 74, 22, 20)); 
    }
    
    saveKitButton.setBounds (sz(patternX + 8, 102, 51, 22)); 
    loadKitButton.setBounds (sz(patternX + 63, 102, 51, 22));

    auto getModuleLayout = [](int mh, int numKnobs, int customKnobSize, bool hasTopButton = false, int rowGap = 12) {
        int cols = juce::jmin(numKnobs, 2);
        int knobSize = customKnobSize;  
        int rows = (numKnobs + cols - 1) / cols;
        int totalContentHeight = rows * (knobSize + rowGap) - 2; 
        int titleOffset = hasTopButton ? 36 : 18; 
        int availableHeight = mh - titleOffset;
        int topMargin = titleOffset + (availableHeight - totalContentHeight) / 2;
        return std::make_pair(knobSize, topMargin);
    };

    auto layoutModuleSmart = [&](int mx, int my, int mw, int mh, std::vector<juce::Slider*> knobs, juce::Button* btn = nullptr, bool useTopRightButton = false, int customKnobSize = 26, int rowGap = 12) {
        int numKnobs = (int)knobs.size();
        if (numKnobs == 0) return;
        bool hasTopButton = (btn != nullptr && !useTopRightButton);
        auto layout = getModuleLayout(mh, numKnobs, customKnobSize, hasTopButton, rowGap);
        int knobSize = layout.first;
        int topMargin = layout.second;
        int cols = juce::jmin(numKnobs, 2);

        for (int k = 0; k < numKnobs; ++k) {
            int r = k / cols; 
            int c = k % cols;
            float relX = (cols == 1) ? 0.5f : (c == 0 ? 0.33f : 0.67f);
            if (numKnobs == 3 && k == 2) relX = 0.5f; 
            int cx = mx + (int)(mw * relX);
            int cy = my + topMargin + (r * (knobSize + rowGap)); 
            knobs[k]->setBounds(sz(cx - (knobSize / 2), cy, knobSize, knobSize));
        }
        if (btn) { 
            if (useTopRightButton) {
                int btnW = 20; int btnH = 12;
                btn->setBounds(sz(mx + mw - btnW - 6, my + 5, btnW, btnH)); 
            } else {
                int btnW = 50; int btnH = 16;
                btn->setBounds(sz(mx + (mw - btnW) / 2, my + 20, btnW, btnH)); 
            }
        }
    };

    int gapFx = 2;
    layoutModuleSmart (fxStartX,         2, fxW, 86, { &masterHpfSlider, &masterHpfResSlider, &masterLpfSlider, &masterLpfResSlider }, nullptr, false, 26, 7);
    layoutModuleSmart (fxStartX + fxW+gapFx,  2, fxW, 86, { &pcmBitsSlider, &pcmRateSlider }, nullptr, false, 28, 12);
    layoutModuleSmart (fxStartX + (fxW+gapFx)*2, 2, fxW, 86, { &driveDistSlider, &driveFilterSlider, &driveVolSlider }, nullptr, false, 26, 7); 
    layoutModuleSmart (fxStartX + (fxW+gapFx)*3, 2, fxW, 86, { &transAttackSlider, &transSustainSlider }, nullptr, false, 28, 12);
    layoutModuleSmart (fxStartX + (fxW+gapFx)*4, 2, fxW, 86, { &envFilterCutSlider, &envFilterResSlider }, nullptr, false, 28, 12);

    layoutModuleSmart (fxStartX,         90, fxW, 88, { &pumpThrSlider, &pumpAmtSlider }, nullptr, false, 28, 12);
    layoutModuleSmart (fxStartX + fxW+gapFx,  90, fxW, 88, { &flangerRateSlider, &flangerFbSlider }, &flangerOnButton, false, 28, 12);
    layoutModuleSmart (fxStartX + (fxW+gapFx)*2, 90, fxW, 88, { &chorusRateSlider, &chorusDepthSlider }, &chorusOnButton, false, 28, 12);
    layoutModuleSmart (fxStartX + (fxW+gapFx)*3, 90, fxW, 88, { &delayTimeSlider, &delayFbSlider, &delayModRateSlider }, &delaySyncButton, true, 26, 7); 
    layoutModuleSmart (fxStartX + (fxW+gapFx)*4, 90, fxW, 88, { &springDecaySlider, &springToneSlider }, nullptr, false, 28, 12);

    masterVolSlider.setBounds (sz(masterX + 17, 22, 42, 42)); 
    masterClipperSlider.setBounds (sz(masterX + 71, 22, 42, 42));

    bit16Button.setBounds (sz(masterX + 19, 82, 28, 20));
    bit12Button.setBounds (sz(masterX + 51, 82, 28, 20));
    bit8Button.setBounds  (sz(masterX + 83, 82, 28, 20));
    analogButton.setBounds      (sz(masterX + 18, 104, 44, 17)); 
    vinylMasterButton.setBounds (sz(masterX + 68, 104, 44, 17));
    pumpButton.setBounds        (sz(masterX + 18, 123, 44, 17)); 
    antiAliasButton.setBounds   (sz(masterX + 68, 123, 44, 17));
    limiterButton.setBounds     (sz(masterX + 18, 142, 94, 17));

    for (int cIdx = 0; cIdx < 12; ++cIdx) {
        int x = 10 + cIdx * 98;
        channelLedButtons[cIdx]->setBounds (sz(x + 5, 187, 82, 28));
        muteButtons[cIdx].setBounds (sz(x + 6, 218, 18, 16)); 
        soloButtons[cIdx].setBounds (sz(x + 27, 218, 18, 16)); 
        envChannelButtons[cIdx].setBounds (sz(x + 48, 218, 38, 16));
        sampleSourceSelectors[cIdx].setBounds (sz(x + 6, 237, 80, 17)); 
        sampleVariantSelectors[cIdx].setBounds (sz(x + 6, 256, 80, 17));
        
        int kSize = 34; 
        volumeSliders[cIdx].setBounds     (sz(x + 7, 273, kSize, kSize)); 
        panSliders[cIdx].setBounds        (sz(x + 51, 273, kSize, kSize));
        pitchSliders[cIdx].setBounds      (sz(x + 7, 314, kSize, kSize)); 
        toneSliders[cIdx].setBounds       (sz(x + 51, 314, kSize, kSize));
        attackSliders[cIdx].setBounds     (sz(x + 7, 355, kSize, kSize)); 
        decaySliders[cIdx].setBounds      (sz(x + 51, 355, kSize, kSize));
        springSendSliders[cIdx].setBounds (sz(x + 7, 396, kSize, kSize)); 
        delaySendSliders[cIdx].setBounds  (sz(x + 51, 396, kSize, kSize));
    }

    int seqX = 15; 
    int seqY = 452; 
    int seqTotalW = 1162; 
    
    int controlsAreaWidth = (int)((float)seqTotalW * 0.20f); 
    int gridAreaWidth = seqTotalW - controlsAreaWidth - 16;
    int stepsStartX = seqX + controlsAreaWidth + 8;

    for (int seqIdx = 0; seqIdx < 12; ++seqIdx) {
        bool isFitMode = fitButtons[seqIdx].getToggleState();
        int maxAllowed = isFitMode ? 32 : 16;
        int curLen = (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(seqIdx))->load();
        int numSteps = juce::jlimit (1, maxAllowed, curLen > 0 ? curLen : 16);

        int stepW_int = isFitMode ? (gridAreaWidth / numSteps) : (gridAreaWidth / 16);
        int btnGap = (numSteps > 20) ? 2 : 4;
        int btnW_int = juce::jmax (2, stepW_int - btnGap);

        fitButtons[seqIdx].setVisible (isSequencerVisible);
        seqModeButtons[seqIdx].setVisible (isSequencerVisible);
        minusButtons[seqIdx].setVisible (isSequencerVisible);
        lengthLabels[seqIdx].setVisible (isSequencerVisible);
        plusButtons[seqIdx].setVisible (isSequencerVisible);
        shiftLeftButtons[seqIdx].setVisible (isSequencerVisible);
        shiftRightButtons[seqIdx].setVisible (isSequencerVisible);

        if (isSequencerVisible) {
            int bx = seqX + 68;
            fitButtons[seqIdx].setBounds     (sz(bx,        seqY + 8 + (seqIdx * 24), 34, 20)); 
            seqModeButtons[seqIdx].setBounds (sz(bx + 36,   seqY + 8 + (seqIdx * 24), 34, 20)); 
            minusButtons[seqIdx].setBounds   (sz(bx + 72,   seqY + 8 + (seqIdx * 24), 18, 20)); 
            lengthLabels[seqIdx].setBounds   (sz(bx + 92,   seqY + 8 + (seqIdx * 24), 20, 20)); 
            plusButtons[seqIdx].setBounds    (sz(bx + 114,  seqY + 8 + (seqIdx * 24), 18, 20)); 
            shiftLeftButtons[seqIdx].setBounds(sz(bx + 134, seqY + 8 + (seqIdx * 24), 18, 20)); 
            shiftRightButtons[seqIdx].setBounds(sz(bx + 154,seqY + 8 + (seqIdx * 24), 18, 20)); 
        }

        for (int step = 0; step < 32; ++step) {
            bool shouldBeVisible = isSequencerVisible && (step < numSteps);
            stepButtons[seqIdx][step].setVisible (shouldBeVisible);
            if (shouldBeVisible) { 
                stepButtons[seqIdx][step].setBounds (sz(stepsStartX + (step * stepW_int), seqY + 8 + (seqIdx * 24), btnW_int, 20)); 
            }
        }
    }

    int fillLen = (int) audioProcessor.apvts.getRawParameterValue ("fillLength")->load();
    bool isFillFit = fillFitButton.getToggleState();
    int fixedStepWidthFill = gridAreaWidth / 16;
    int fillBtnW = juce::jmax (2, fixedStepWidthFill - 5);

    fillFitButton.setVisible (isSequencerVisible);
    fillSeqModeButton.setVisible (isSequencerVisible);
    fillMinusButton.setVisible (isSequencerVisible);
    fillLengthLabel.setVisible (isSequencerVisible);
    fillPlusButton.setVisible (isSequencerVisible);
    fillShiftLeftButton.setVisible (isSequencerVisible);
    fillShiftRightButton.setVisible (isSequencerVisible);

    if (isSequencerVisible) {
        int bx = seqX + 68;
        int by = seqY + 10 + (12 * 24);
        fillFitButton.setBounds       (sz(bx,       by, 34, 20));
        fillSeqModeButton.setBounds   (sz(bx + 36,  by, 34, 20));
        fillMinusButton.setBounds     (sz(bx + 72,  by, 18, 20));
        fillLengthLabel.setBounds     (sz(bx + 92,  by, 20, 20));
        fillPlusButton.setBounds      (sz(bx + 114, by, 18, 20));
        fillShiftLeftButton.setBounds (sz(bx + 134, by, 18, 20));
        fillShiftRightButton.setBounds(sz(bx + 154, by, 18, 20));
    }

    for (int step = 0; step < 16; ++step) {
        bool shouldBeVisible = isSequencerVisible && (step < fillLen); 
        fillStepButtons[step].setVisible (shouldBeVisible);
        if (shouldBeVisible) { 
            fillStepButtons[step].setBounds (sz(stepsStartX + (step * fixedStepWidthFill), seqY + 10 + (12 * 24), fillBtnW, 20)); 
        }
    }

    if (cornerResizer) {
        cornerResizer->setBounds (getWidth() - 20, getHeight() - 20, 20, 20);
    }
}

bool ExtasisRhythmEditor::keyPressed (const juce::KeyPress& key)
{
    if (key == juce::KeyPress::spaceKey)
    {
        if (!audioProcessor.isSyncedToHost.load()) { 
            auto* pi = audioProcessor.apvts.getParameter ("isPlaying"); 
            if (pi) { 
                pi->beginChangeGesture(); 
                pi->setValueNotifyingHost (pi->getValue() > 0.5f ? 0.0f : 1.0f); 
                pi->endChangeGesture(); 
            } 
        }
        return true;
    }
    return false;
}

void ExtasisRhythmEditor::mouseDown (const juce::MouseEvent& e)
{
    juce::ignoreUnused (e);
}