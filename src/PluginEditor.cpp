#include "PluginProcessor.h"
#include "PluginEditor.h"

class CustomComboBoxLookAndFeel : public juce::LookAndFeel_V4 {
public:
    void drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box) override {
        juce::ignoreUnused (box);
        auto bounds = juce::Rectangle<int> (0, 0, width, height).toFloat();
        g.setColour (juce::Colour (0xffe0e0e0)); g.fillRoundedRectangle (bounds, 3.0f);
        g.setColour (juce::Colour (0xffb0b0b0)); g.drawRoundedRectangle (bounds, 3.0f, 1.0f);
        
        // Dibujar flechita del combo más a la derecha para dejar más espacio al texto
        juce::Path p; p.addTriangle ((float)width - 10.0f, height * 0.4f, (float)width - 4.0f, height * 0.4f, (float)width - 7.0f, height * 0.65f);
        g.setColour (juce::Colour (0xff555555)); g.fillPath (p);
    }
    
    // Tipografía un poco más chica para los menús desplegables para aprovechar el espacio
    juce::Font getComboBoxFont (juce::ComboBox&) override {
        return juce::FontOptions (10.5f, juce::Font::plain);
    }

    void drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const juce::String& text, const juce::String& shortcutKeyText, const juce::Drawable* icon, const juce::Colour* textColourToUse) override {
        if (isSeparator) { auto r = area.reduced (5, 0); g.setColour (juce::Colour (0xffcccccc)); g.fillRect (r.removeFromTop (1)); return; }
        auto textColour = juce::Colours::black; 
        if (isHighlighted) { g.setColour (juce::Colour (0xff3498db)); g.fillRect (area); textColour = juce::Colours::white; }
        g.setColour (textColour); g.setFont (getPopupMenuFont()); auto r = area.reduced (10, 0); g.drawFittedText (text, r, juce::Justification::centredLeft, 1);
    }
};

ExtasisRhythmEditor::ExtasisRhythmEditor (ExtasisRhythmProcessor& proc)
    : AudioProcessorEditor (&proc), audioProcessor (proc) {
    setResizable (true, true); getConstrainer()->setFixedAspectRatio (1192.0 / 720.0); getConstrainer()->setMinimumSize (900, 520); setWantsKeyboardFocus (true);
    static CustomComboBoxLookAndFeel safeComboBoxLAF;

    addAndMakeVisible (playButton); playButton.setButtonText ("PLAY"); playButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0)); playButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    playButton.onClick = [this] { if (!audioProcessor.isSyncedToHost.load()) { auto* pi = audioProcessor.apvts.getParameter ("isPlaying"); if (pi) { pi->beginChangeGesture(); pi->setValueNotifyingHost (pi->getValue() > 0.5f ? 0.0f : 1.0f); pi->endChangeGesture(); } } };

    addAndMakeVisible (stopButton); stopButton.setButtonText ("STOP"); stopButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0)); stopButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
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

    addAndMakeVisible (bpmSlider); bpmSlider.setSliderStyle (juce::Slider::LinearBar); bpmSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 55, 18); bpmSlider.setNumDecimalPlacesToDisplay(1); 
    bpmSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffff6600)); 
    bpmSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour(0xffeaeaea));
    bpmAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "bpm", bpmSlider);

    addAndMakeVisible (masterVolSlider); masterVolSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); masterVolSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0); masterVolSlider.setLookAndFeel (&simpleKnobLAF); masterVolSlider.setPopupDisplayEnabled (true, true, this); masterVolAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "masterVolume", masterVolSlider);
    addAndMakeVisible (masterClipperSlider); masterClipperSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); masterClipperSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0); masterClipperSlider.setLookAndFeel (&simpleKnobLAF); masterClipperSlider.setPopupDisplayEnabled (true, true, this); masterClipAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "masterClipper", masterClipperSlider);

    addAndMakeVisible (flangerOnButton); flangerOnButton.setClickingTogglesState (true); flangerOnButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff444444)); flangerOnButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff8e44ad)); flangerOnButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black); flangerOnButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    auto* flangerParam = audioProcessor.apvts.getRawParameterValue("flangerOn"); flangerOnButton.setButtonText ((flangerParam && flangerParam->load() > 0.5f) ? "ON" : "OFF");
    flangerOnButton.onClick = [this] { flangerOnButton.setButtonText (flangerOnButton.getToggleState() ? "ON" : "OFF"); }; flangerOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "flangerOn", flangerOnButton);

    addAndMakeVisible (saveKitButton); addAndMakeVisible (loadKitButton); addAndMakeVisible (resetButton); addAndMakeVisible (seqResetButton);
    saveKitButton.setButtonText ("SAVE"); loadKitButton.setButtonText ("LOAD"); resetButton.setButtonText ("RESET"); seqResetButton.setButtonText ("SEQ RST");
    saveKitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff2e8b57)); saveKitButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white); loadKitButton.setButtonText ("LOAD"); loadKitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffd2691e)); loadKitButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white); resetButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffff6600)); resetButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white); seqResetButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff9b59b6)); seqResetButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);

    resetButton.onClick = [this] { audioProcessor.resetAllParameters(); }; 
    seqResetButton.onClick = [this] { audioProcessor.resetSequencer(); };

    saveKitButton.onClick = [this] { saveChooser = std::make_unique<juce::FileChooser> ("Save Project", juce::File::getSpecialLocation(juce::File::userHomeDirectory), "*.xml"); saveChooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles, [this] (const juce::FileChooser& c) { if (c.getResult() != juce::File()) audioProcessor.saveCustomPreset(c.getResult()); }); };
    
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
            }
        }); 
    };

    addAndMakeVisible (globalKitSelector); globalKitSelector.setLookAndFeel (&safeComboBoxLAF); globalKitSelector.setColour (juce::ComboBox::textColourId, juce::Colours::black); globalKitSelector.addItemList (audioProcessor.getDrumKitNames(), 1); globalKitAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, "globalKitChoice", globalKitSelector);
    
    int initialKit = (int)audioProcessor.apvts.getRawParameterValue("globalKitChoice")->load();
    globalKitSelector.setSelectedId(initialKit + 1, juce::dontSendNotification);

    globalKitSelector.onChange = [this] { 
        int kitIdx = globalKitSelector.getSelectedId() - 1; 
        
        if (auto* kitParam = audioProcessor.apvts.getParameter("globalKitChoice")) {
            kitParam->beginChangeGesture();
            kitParam->setValueNotifyingHost(kitParam->convertTo0to1((float)kitIdx));
            kitParam->endChangeGesture();
        }

        audioProcessor.loadGlobalDrumKit (kitIdx);

        for (int i = 0; i < 12; ++i) {
            if (auto* sourceParam = audioProcessor.apvts.getParameter("sampleSource_" + juce::String(i))) {
                sourceParam->beginChangeGesture();
                sourceParam->setValueNotifyingHost(sourceParam->convertTo0to1((float)kitIdx));
                sourceParam->endChangeGesture();
            }

            sampleSourceSelectors[i].setSelectedId (kitIdx + 1, juce::dontSendNotification); 
            sampleVariantSelectors[i].clear (juce::dontSendNotification);
            
            auto variants = audioProcessor.getVariantsForChannel (kitIdx, i); 
            sampleVariantSelectors[i].addItemList (variants, 1);
            
            int matchIdx = variants.indexOf (audioProcessor.currentSampleName[i]); 
            sampleVariantSelectors[i].setSelectedId (matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
        }
    };

    juce::String pNames = "ABCDEFGH";
    for (int i = 0; i < 8; ++i) {
        addAndMakeVisible (patternButtons[i]); patternButtons[i].setButtonText (juce::String::charToString(pNames[i])); patternButtons[i].setColour (juce::TextButton::buttonColourId, i == 0 ? juce::Colour (0xff3498db) : juce::Colour (0xffcccccc)); patternButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black); patternButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        patternButtons[i].onClick = [this, i] {
            for (int b = 0; b < 8; ++b) { patternButtons[b].setColour (juce::TextButton::buttonColourId, b == i ? juce::Colour (0xff3498db) : juce::Colour (0xffcccccc)); patternButtons[b].setColour (juce::TextButton::textColourOffId, juce::Colours::black); }
            audioProcessor.changePattern (i); activePatternButton = i;
        };
    }

    auto mkKnob = [this] (juce::Slider& sl, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& at, const juce::String& id) {
        sl.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0); sl.setLookAndFeel (&simpleKnobLAF); sl.setPopupDisplayEnabled (true, true, this); addAndMakeVisible (sl); at = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, id, sl);
    };

    mkKnob (masterHpfSlider, hpfAtt, "masterHpf"); mkKnob (masterHpfResSlider, hpfResAtt, "masterHpfRes"); mkKnob (masterLpfSlider, lpfAtt, "masterLpf"); mkKnob (masterLpfResSlider, lpfResAtt, "masterLpfRes"); mkKnob (pcmBitsSlider, pcmBitsAtt, "pcmBits"); mkKnob (pcmRateSlider, pcmRateAtt, "pcmRate"); mkKnob (flangerRateSlider, flRateAtt, "flangerRate"); mkKnob (flangerFbSlider, flFbAtt, "flangerFeedback"); mkKnob (transAttackSlider, transAttAtt, "transientAttack"); mkKnob (transSustainSlider, transSusAtt, "transientSustain"); mkKnob (driveDistSlider, driveDistAtt, "driveDist"); mkKnob (driveFilterSlider, driveFilterAtt, "driveFilter"); mkKnob (driveVolSlider, driveVolAtt, "driveVol"); mkKnob (springDecaySlider, springDecAtt, "springDecay"); mkKnob (springToneSlider, springToneAtt, "springTone"); mkKnob (delayTimeSlider, delTimeAtt, "delayTime"); mkKnob (delayFbSlider, delFbAtt, "delayFb"); mkKnob (delayModRateSlider, delModRateAtt, "delayModRate"); mkKnob (delayModDepthSlider, delModDepthAtt, "delayModDepth");

    pcmBitsSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffff6600)); pcmRateSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffff6600)); flangerRateSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff8e44ad)); flangerFbSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff8e44ad)); transAttackSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffffb300)); transSustainSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffffb300)); driveDistSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffcc0000)); driveFilterSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffcc0000)); driveVolSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffcc0000)); springDecaySlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff556b2f)); springToneSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff556b2f)); delayTimeSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff00d2ff)); delayFbSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff00d2ff)); delayModRateSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff00d2ff)); delayModDepthSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff00d2ff)); masterVolSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffe74c3c)); masterClipperSlider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffe74c3c));

    addAndMakeVisible (tripletFillButton);
    tripletFillButton.setButtonText ("3L"); tripletFillButton.setClickingTogglesState (true);
    tripletFillButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); tripletFillButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff9b59b6)); tripletFillButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black); tripletFillButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    tripletFillButton.onClick = [this] { resized(); }; 
    tripletFillAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "tripletFill", tripletFillButton);

    for (int step = 0; step < 16; ++step) {
        addAndMakeVisible (fillStepButtons[step]);
        fillStepButtons[step].onClick = [this, step] {
            auto* par = audioProcessor.apvts.getParameter ("fill_step_" + juce::String(step));
            if (par) {
                int next = ((int)(par->getValue() * 2.0f + 0.5f) + 1) % 3;
                par->beginChangeGesture(); par->setValueNotifyingHost ((float)next / 2.0f); par->endChangeGesture();
                updateFillButtonVisuals (step);
            }
        };
        updateFillButtonVisuals (step);
    }

    auto kitNames = audioProcessor.getDrumKitNames();
    for (int i = 0; i < 12; ++i) {
        juce::String chStr = juce::String(i);
        
        addAndMakeVisible (sampleSourceSelectors[i]); sampleSourceSelectors[i].setLookAndFeel (&safeComboBoxLAF); sampleSourceSelectors[i].setColour (juce::ComboBox::textColourId, juce::Colours::black); sampleSourceSelectors[i].addItemList (kitNames, 1); sampleSourceAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, "sampleSource_" + chStr, sampleSourceSelectors[i]);
        addAndMakeVisible (sampleVariantSelectors[i]); sampleVariantSelectors[i].setLookAndFeel (&safeComboBoxLAF); sampleVariantSelectors[i].setColour (juce::ComboBox::textColourId, juce::Colours::black);
        
        int currentKit = (int)audioProcessor.apvts.getRawParameterValue("sampleSource_" + chStr)->load();
        sampleSourceSelectors[i].setSelectedId (currentKit + 1, juce::dontSendNotification);

        auto variants = audioProcessor.getVariantsForChannel (currentKit, i); sampleVariantSelectors[i].addItemList (variants, 1);
        int matchIdx = variants.indexOf(audioProcessor.currentSampleName[i]); sampleVariantSelectors[i].setSelectedId (matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
        
        sampleSourceSelectors[i].onChange = [this, i] { 
            int kitIdx = sampleSourceSelectors[i].getSelectedId() - 1; 
            if (kitIdx < 0) return;

            if (auto* sourceParam = audioProcessor.apvts.getParameter("sampleSource_" + juce::String(i))) {
                sourceParam->beginChangeGesture();
                sourceParam->setValueNotifyingHost(sourceParam->convertTo0to1((float)kitIdx));
                sourceParam->endChangeGesture();
            }

            audioProcessor.loadSmartSampleForChannel(i, kitIdx);

            sampleVariantSelectors[i].clear(juce::dontSendNotification);
            auto variants = audioProcessor.getVariantsForChannel (kitIdx, i); 
            sampleVariantSelectors[i].addItemList (variants, 1);
            
            int matchIdx = variants.indexOf(audioProcessor.currentSampleName[i]); 
            sampleVariantSelectors[i].setSelectedId (matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
        };

        sampleVariantSelectors[i].onChange = [this, i] {
            int kitIdx = sampleSourceSelectors[i].getSelectedId() - 1; auto variants = audioProcessor.getVariantsForChannel (kitIdx, i); int varIdx = sampleVariantSelectors[i].getSelectedId() - 1;
            if (varIdx >= 0 && varIdx < variants.size()) audioProcessor.loadSampleForChannel (i, kitIdx, variants[varIdx]);
        };

        addAndMakeVisible (muteButtons[i]); muteButtons[i].setButtonText ("M"); muteButtons[i].setClickingTogglesState (true); muteButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); muteButtons[i].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffff3333)); muteButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black); muteButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white); muteAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "mute" + chStr, muteButtons[i]);
        addAndMakeVisible (soloButtons[i]); soloButtons[i].setButtonText ("S"); soloButtons[i].setClickingTogglesState (true); soloButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); soloButtons[i].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff2ecc71)); soloButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black); soloButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white); soloAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "solo" + chStr, soloButtons[i]);
        addAndMakeVisible (tripletButtons[i]); tripletButtons[i].setButtonText ("3L"); tripletButtons[i].setClickingTogglesState (true); tripletButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); tripletButtons[i].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff9b59b6)); tripletButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black); tripletButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        tripletButtons[i].onClick = [this] { resized(); }; tripletAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "triplet" + chStr, tripletButtons[i]);

        mkKnob (volumeSliders[i], volAtts[i], "gain" + chStr); mkKnob (panSliders[i], panAtts[i], "pan" + chStr); mkKnob (pitchSliders[i], pitchAtts[i], "pitch" + chStr); mkKnob (toneSliders[i], toneAtts[i], "tone" + chStr); mkKnob (attackSliders[i], attAtts[i], "attack" + chStr); mkKnob (decaySliders[i], decAtts[i], "decay" + chStr); mkKnob (springSendSliders[i], springSendAtts[i], "springSend" + chStr); mkKnob (delaySendSliders[i], delaySendAtts[i], "delaySend" + chStr);
        delaySendSliders[i].setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff00d2ff)); springSendSliders[i].setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff556b2f));
        stepLengthAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "length" + chStr, hiddenLengthSliders[i]);
        
        addAndMakeVisible (minusButtons[i]); minusButtons[i].setButtonText ("-"); minusButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); minusButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black);
        minusButtons[i].onClick = [this, i] { double val = hiddenLengthSliders[i].getValue() - 1.0; if (val < 1.0) val = 1.0; hiddenLengthSliders[i].setValue (val, juce::sendNotification); updateLengthLabel (i); resized(); };
        addAndMakeVisible (plusButtons[i]); plusButtons[i].setButtonText ("+"); plusButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); plusButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black);
        plusButtons[i].onClick = [this, i] { double val = hiddenLengthSliders[i].getValue() + 1.0; if (val > 16.0) val = 16.0; hiddenLengthSliders[i].setValue (val, juce::sendNotification); updateLengthLabel (i); resized(); };
        
        addAndMakeVisible (lengthLabels[i]); lengthLabels[i].setColour (juce::Label::textColourId, juce::Colours::black); lengthLabels[i].setFont (juce::FontOptions (10.0f, juce::Font::bold)); lengthLabels[i].setJustificationType (juce::Justification::centred); updateLengthLabel (i);

        for (int step = 0; step < 16; ++step) {
            addAndMakeVisible (stepButtons[i][step]);
            stepButtons[i][step].onClick = [this, i, step] {
                auto* par = audioProcessor.apvts.getParameter ("step_" + juce::String(i) + "_" + juce::String(step));
                if (par) {
                    int next = ((int)(par->getValue() * 3.0f + 0.5f) + 1) % 4;
                    par->beginChangeGesture(); par->setValueNotifyingHost ((float)next / 3.0f); par->endChangeGesture();
                    updateStepButtonVisuals (i, step);
                }
            };
            updateStepButtonVisuals (i, step);
        }
    }

    cornerResizer = std::make_unique<juce::ResizableCornerComponent> (this, getConstrainer());
    addAndMakeVisible (cornerResizer.get()); setSize (1192, 720); startTimerHz (30);
}

ExtasisRhythmEditor::~ExtasisRhythmEditor() { stopTimer(); }

bool ExtasisRhythmEditor::keyPressed (const juce::KeyPress& key) {
    if (key == juce::KeyPress::spaceKey) { playButton.triggerClick(); return true; }
    return juce::AudioProcessorEditor::keyPressed (key);
}

void ExtasisRhythmEditor::mouseDown (const juce::MouseEvent& e) {
    float s = (float) getWidth() / 1192.0f; auto pos = e.getPosition().toFloat() / s;
    for (int i = 0; i < 12; ++i) { if (juce::Rectangle<float> ((float)(10 + i * 98), 115.0f, 92.0f, 45.0f).contains (pos)) { audioProcessor.triggerChannel (i, 1.0f); break; } }
}

void ExtasisRhythmEditor::updateStepButtonVisuals (int ch, int step) {
    auto* par = audioProcessor.apvts.getRawParameterValue ("step_" + juce::String(ch) + "_" + juce::String(step));
    if (par) { int v = (int)(*par + 0.5f); juce::Colour cols[4] = { juce::Colour(0xffe0e0e0), juce::Colour(0xffffcc00), juce::Colour(0xffff6600), juce::Colour(0xffff0000) }; stepButtons[ch][step].setColour (juce::TextButton::buttonColourId, cols[v]); }
}

void ExtasisRhythmEditor::updateFillButtonVisuals (int step) {
    auto* par = audioProcessor.apvts.getRawParameterValue ("fill_step_" + juce::String(step));
    if (par) { int v = (int)(*par + 0.5f); fillStepButtons[step].setButtonText (""); juce::Colour cols[3] = { juce::Colour(0xffe0e0e0), juce::Colour(0xff2ecc71), juce::Colour(0xff3498db) }; fillStepButtons[step].setColour (juce::TextButton::buttonColourId, cols[v]); }
}

void ExtasisRhythmEditor::updateLengthLabel (int i) {
    int len = (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(i))->load();
    if (len <= 0) len = 16; lengthLabels[i].setText (juce::String (len), juce::dontSendNotification);
}

void ExtasisRhythmEditor::paint (juce::Graphics& g) {
    float s = (float) getWidth() / 1192.0f; g.saveState(); g.addTransform (juce::AffineTransform::scale (s));
    g.fillAll (juce::Colour (0xffe6e6e6)); g.setColour (juce::Colour (0xffd4d4d4)); g.fillRect (0, 0, 1192, 98); 
    g.setColour (juce::Colours::black.withAlpha (0.15f)); g.fillRect (0, 98, 1192, 4);

    g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); g.setColour (juce::Colours::black.withAlpha(0.7f));
    g.drawText ("coded by @laurorobles", 18, 38, 150, 14, juce::Justification::left);

    g.setFont (juce::FontOptions (24.0f, juce::Font::bold)); g.setColour (juce::Colours::black.withAlpha(0.2f));
    g.drawText ("ExtasisRhythm", 17, 11, 180, 32, juce::Justification::centredLeft); g.setColour (juce::Colour (0xff3498db));
    g.drawText ("ExtasisRhythm", 15, 9, 180, 32, juce::Justification::centredLeft);

    g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); g.setColour (juce::Colour (0xff666666));
    g.drawText ("v1.5", 125, 21, 50, 15, juce::Justification::left);

    g.setColour (juce::Colours::white.withAlpha(0.5f)); g.drawHorizontalLine (99, 0.0f, 1192.0f); g.setColour (juce::Colours::black.withAlpha(0.3f)); g.drawHorizontalLine (97, 0.0f, 1192.0f);

    auto drawModuleBox = [&g] (int x, int y, int w, int h, const juce::String& title) {
        g.setColour (juce::Colour (0xffcccccc)); g.fillRoundedRectangle ((float)x, (float)y, (float)w, (float)h, 4.0f);
        g.setColour (juce::Colours::black.withAlpha(0.15f)); g.drawRoundedRectangle ((float)x, (float)y, (float)w, (float)h, 4.0f, 1.0f);
        g.setFont (juce::FontOptions (9.0f, juce::Font::bold)); g.setColour (juce::Colour (0xff222222)); g.drawText (title, x, y + 2, w, 12, juce::Justification::centred);
    };

    drawModuleBox (390, 2, 106, 94, "PATTERNS"); drawModuleBox (500, 2, 62, 94, "HPF"); drawModuleBox (566, 2, 62, 94, "LPF"); drawModuleBox (632, 2, 62, 94, "PCM"); drawModuleBox (698, 2, 62, 94, "FLANGER"); drawModuleBox (764, 2, 62, 94, "TRANS"); drawModuleBox (830, 2, 86, 94, "OVERDRIVE"); drawModuleBox (920, 2, 62, 94, "SPRING"); drawModuleBox (986, 2, 110, 94, "SDE DELAY"); drawModuleBox (1100, 2, 86, 94, "MASTER");

    g.setFont (juce::FontOptions (7.5f, juce::Font::bold)); g.setColour (juce::Colour (0xff444444));
    g.drawText ("CUT", 504, 54, 24, 10, juce::Justification::centred); g.drawText ("RES", 532, 54, 24, 10, juce::Justification::centred);
    g.drawText ("CUT", 570, 54, 24, 10, juce::Justification::centred); g.drawText ("RES", 598, 54, 24, 10, juce::Justification::centred);
    g.drawText ("BITS", 636, 54, 24, 10, juce::Justification::centred); g.drawText ("RATE", 664, 54, 24, 10, juce::Justification::centred);
    g.drawText ("RATE", 702, 16, 24, 10, juce::Justification::centred); g.drawText ("FB", 730, 16, 24, 10, juce::Justification::centred);
    g.drawText ("ATT", 768, 54, 24, 10, juce::Justification::centred); g.drawText ("SUS", 796, 54, 24, 10, juce::Justification::centred);
    g.drawText ("DIST", 835, 54, 24, 10, juce::Justification::centred); g.drawText ("FLTR", 861, 54, 24, 10, juce::Justification::centred); g.drawText ("VOL", 887, 54, 24, 10, juce::Justification::centred);
    g.drawText ("DEC", 924, 54, 24, 10, juce::Justification::centred); g.drawText ("TONE", 952, 54, 24, 10, juce::Justification::centred);
    g.drawText ("TIME", 990, 54, 24, 10, juce::Justification::centred); g.drawText ("FB", 1016, 54, 24, 10, juce::Justification::centred); g.drawText ("RATE", 1042, 54, 24, 10, juce::Justification::centred); g.drawText ("DEP", 1068, 54, 24, 10, juce::Justification::centred);
    g.drawText ("VOL", 1104, 56, 36, 10, juce::Justification::centred); g.drawText ("CLIP", 1146, 56, 36, 10, juce::Justification::centred);

    juce::String labels[12] = { "BD", "SD", "CH", "OH", "CP", "RS", "HT", "MT", "LT", "CB", "CR", "RD" };
    for (int i = 0; i < 12; ++i) {
        int x = 10 + i * 98; 
        
        g.setColour (juce::Colours::black.withAlpha (0.15f)); g.fillRoundedRectangle ((float)(x + 3), 103.0f, 92.0f, 255.0f, 4.0f);
        juce::ColourGradient cg (juce::Colour (0xfffcfcfc), (float)x, 100.0f, juce::Colour (0xffe0e0e0), (float)x, 355.0f, false);
        g.setGradientFill (cg); g.fillRoundedRectangle ((float)x, 100.0f, 92.0f, 255.0f, 4.0f);
        g.setColour (juce::Colour (0xffb8b8b8)); g.drawRoundedRectangle ((float)x, 100.0f, 92.0f, 255.0f, 4.0f, 1.0f);
        g.setColour (juce::Colours::white.withAlpha(0.9f)); g.drawHorizontalLine (101, (float)(x + 2), (float)(x + 90));

        g.setColour (juce::Colours::black.withAlpha(0.4f)); g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.drawText (labels[i], x + 36, 116, 20, 16, juce::Justification::centred);
        g.setColour (juce::Colour (0xff222222)); g.drawText (labels[i], x + 35, 115, 20, 16, juce::Justification::centred);

        bool isLit = audioProcessor.flashCounters[i].load() > 0;
        g.setColour (isLit ? juce::Colour (0xff3498db) : juce::Colour (0xff555555)); g.fillEllipse ((float)(x + 42), 104.0f, 8.0f, 8.0f);
        if (isLit) { g.setColour (juce::Colours::white.withAlpha(0.6f)); g.fillEllipse ((float)(x + 44), 106.0f, 3.0f, 3.0f); }

        g.setFont (juce::FontOptions (7.5f, juce::Font::bold)); g.setColour (juce::Colour (0xff444444));
        g.drawText ("VOL", x + 8, 192, 32, 10, juce::Justification::centred); g.drawText ("PAN", x + 52, 192, 32, 10, juce::Justification::centred);
        g.drawText ("PITCH", x + 8, 233, 32, 10, juce::Justification::centred); g.drawText ("TONE", x + 52, 233, 32, 10, juce::Justification::centred);
        g.drawText ("ATT", x + 8, 274, 32, 10, juce::Justification::centred); g.drawText ("DEC", x + 52, 274, 32, 10, juce::Justification::centred);
        g.drawText ("SPR", x + 8, 315, 32, 10, juce::Justification::centred); g.drawText ("DLY", x + 52, 315, 32, 10, juce::Justification::centred);
    }
    
    int seqX = 15; int seqY = 362; int seqW = 1162; int seqH = 345;
    g.setColour (juce::Colour (0xffd8d8d8)); g.fillRoundedRectangle ((float)seqX, (float)seqY, (float)seqW, (float)seqH, 6.0f);
    g.setColour (juce::Colours::black.withAlpha(0.2f)); g.drawRoundedRectangle ((float)seqX, (float)seqY, (float)seqW, (float)seqH, 6.0f, 2.0f);

    int stepsStartX = seqX + 152; int totalStepsWidth = seqW - 192;

    for (int i = 0; i < 12; ++i) {
        g.setColour (juce::Colours::black.withAlpha(0.3f)); g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        g.drawText (labels[i], seqX + 8, seqY + 8 + (i * 24), 22, 20, juce::Justification::centred);
        g.setColour (juce::Colour (0xff222222)); g.drawText (labels[i], seqX + 7, seqY + 7 + (i * 24), 22, 20, juce::Justification::centred);
    }

    g.setColour (juce::Colours::black.withAlpha(0.3f)); g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    g.drawText ("FILL", seqX + 8, seqY + 10 + (12 * 24), 30, 20, juce::Justification::centred);
    g.setColour (juce::Colour (0xff222222)); g.drawText ("FILL", seqX + 7, seqY + 9 + (12 * 24), 30, 20, juce::Justification::centred);

    float stepW16 = (float)totalStepsWidth / 16.0f;
    int metricSteps[4] = { 0, 4, 8, 12 }; 
    g.setFont (juce::FontOptions (9.0f, juce::Font::bold));
    
    for (int m = 0; m < 4; ++m) {
        int stepIndex = metricSteps[m];
        float stepXCenter = (float)(stepsStartX) + (stepIndex * stepW16) + (stepW16 * 0.5f); 
        float indicatorY = (float)(seqY + seqH - 20); 
        g.setColour (juce::Colour (0xff3498db)); g.fillEllipse (stepXCenter - 2.5f, indicatorY, 5.0f, 5.0f);
        g.setColour (juce::Colour (0xff333333)); g.drawText (juce::String (m + 1), (int)(stepXCenter - 15.0f), (int)(indicatorY + 4.0f), 30, 12, juce::Justification::centred);
    }

    int meterX = seqX + seqW - 32;
    int meterY = seqY + 8;
    int meterH = seqH - 28;
    g.setColour (juce::Colour (0xff111111)); g.fillRect (meterX, meterY, 24, meterH);
    g.setColour (juce::Colour (0xff333333)); g.drawRect ((float)meterX, (float)meterY, 24.0f, (float)meterH, 1.0f);

    float curRmsL = audioProcessor.outputLevelL.load(); float curRmsR = audioProcessor.outputLevelR.load();
    auto rmsToNormalizedDb = [] (float rms) { if (rms <= 0.00001f) return 0.0f; float db = 20.0f * std::log10 (rms); return juce::jlimit (0.0f, 1.0f, (db - (-48.0f)) / (3.0f - (-48.0f))); };
    float mL = rmsToNormalizedDb (curRmsL); float mR = rmsToNormalizedDb (curRmsR);

    int numSegments = 24; float segHeight = ((float)meterH - 8.0f) / (float)numSegments;
    for (int seg = 0; seg < numSegments; ++seg) {
        float threshold = 1.0f - ((float)seg / (float)numSegments);
        juce::Colour ledColor;
        if (threshold > 0.85f) ledColor = juce::Colour (0xffff3333); else if (threshold > 0.65f) ledColor = juce::Colour (0xffffcc00); else ledColor = juce::Colour (0xff2ecc71);
        float yP = (float)(meterY + 4) + (float)seg * segHeight;
        if (mL >= threshold) g.setColour (ledColor); else g.setColour (juce::Colour (0xff222222)); g.fillRect ((float)(meterX + 3), yP, 8.0f, segHeight - 2.0f);
        if (mR >= threshold) g.setColour (ledColor); else g.setColour (juce::Colour (0xff222222)); g.fillRect ((float)(meterX + 13), yP, 8.0f, segHeight - 2.0f);
    }

    // ALINEACIÓN PERFECTAMENTE MILIMÉTRICA DEL RECTÁNGULO DE REPRODUCCIÓN USANDO ENTEROS PUROS
    for (int i = 0; i < 12; ++i) {
        int act = audioProcessor.channelSteps[i].load(); 
        bool isTriplet = audioProcessor.apvts.getRawParameterValue ("triplet" + juce::String(i))->load() > 0.5f;
        int numSteps = isTriplet ? 12 : (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(i))->load();
        if (numSteps <= 0) numSteps = isTriplet ? 12 : 16;
        
        int stepW_int = totalStepsWidth / numSteps;
        int exactStepX = stepsStartX + (act * stepW_int);
        int exactBtnW = stepW_int - 6;
        
        g.setColour (juce::Colour (0xff3498db)); 
        g.drawRect ((float)(exactStepX - 1), (float)(seqY + 8 + (i * 24) - 1), (float)(exactBtnW + 2), 22.0f, 2.5f);
    }
    g.restoreState();
}

void ExtasisRhythmEditor::timerCallback() { 
    for (int i = 0; i < 12; ++i) { 
        for (int step = 0; step < 16; ++step) updateStepButtonVisuals(i, step);
        
        int currentKit = (int)audioProcessor.apvts.getRawParameterValue("sampleSource_" + juce::String(i))->load();
        if (sampleSourceSelectors[i].getSelectedId() != currentKit + 1)
            sampleSourceSelectors[i].setSelectedId(currentKit + 1, juce::dontSendNotification);

        auto variants = audioProcessor.getVariantsForChannel(currentKit, i);
        int matchIdx = variants.indexOf(audioProcessor.currentSampleName[i]);
        if (matchIdx >= 0 && sampleVariantSelectors[i].getSelectedId() != matchIdx + 1)
            sampleVariantSelectors[i].setSelectedId(matchIdx + 1, juce::dontSendNotification);
    }
    
    int globalKit = (int)audioProcessor.apvts.getRawParameterValue("globalKitChoice")->load();
    if (globalKitSelector.getSelectedId() != globalKit + 1)
        globalKitSelector.setSelectedId(globalKit + 1, juce::dontSendNotification);

    for (int step = 0; step < 16; ++step) updateFillButtonVisuals(step);

    if (audioProcessor.isSyncedToHost.load()) {
        double hb = audioProcessor.hostBpm.load();
        if (std::abs(bpmSlider.getValue() - hb) > 0.01) bpmSlider.setValue (hb, juce::dontSendNotification);
        if (audioProcessor.hostPlaying.load()) { playButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff2ecc71)); playButton.setButtonText ("HOST SYNC"); } 
        else { playButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0)); playButton.setButtonText ("HOST STOP"); }
    } else {
        auto* pi = audioProcessor.apvts.getParameter ("isPlaying"); bool isP = pi && pi->getValue() > 0.5f;
        playButton.setColour (juce::TextButton::buttonColourId, isP ? juce::Colour (0xff3498db) : juce::Colour (0xffb0b0b0)); playButton.setButtonText (isP ? "PLAYING" : "PLAY");
    }
    repaint(); 
}

void ExtasisRhythmEditor::resized() {
    float s = (float) getWidth() / 1192.0f;
    auto sz = [s](int x, int y, int w, int h) { return juce::Rectangle<int> ((int)((float)x*s), (int)((float)y*s), (int)((float)w*s), (int)((float)h*s)); };

    playButton.setBounds (sz(185, 14, 48, 20)); stopButton.setBounds (sz(237, 14, 48, 20));
    resetButton.setBounds (sz(185, 36, 48, 20)); seqResetButton.setBounds (sz(237, 36, 48, 20));
    bpmSlider.setBounds (sz(185, 58, 100, 24)); globalKitSelector.setBounds (sz(293, 14, 96, 20));

    for (int i = 0; i < 8; ++i) {
        int row = i / 4; int col = i % 4;
        patternButtons[i].setBounds (sz(396 + (col * 24), 14 + (row * 21), 22, 19)); patternButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    }
    saveKitButton.setBounds (sz(396, 58, 46, 18)); loadKitButton.setBounds (sz(446, 58, 46, 18));

    masterHpfSlider.setBounds (sz(504, 22, 24, 28)); masterHpfResSlider.setBounds (sz(532, 22, 24, 28)); masterLpfSlider.setBounds (sz(570, 22, 24, 28)); masterLpfResSlider.setBounds (sz(598, 22, 24, 28)); pcmBitsSlider.setBounds (sz(636, 22, 24, 28)); pcmRateSlider.setBounds (sz(664, 22, 24, 28)); 
    
    flangerRateSlider.setBounds (sz(702, 28, 24, 28)); flangerFbSlider.setBounds (sz(730, 28, 24, 28)); flangerOnButton.setBounds (sz(707, 62, 44, 20)); 
    
    transAttackSlider.setBounds (sz(768, 22, 24, 28)); transSustainSlider.setBounds (sz(796, 22, 24, 28)); driveDistSlider.setBounds (sz(835, 22, 24, 28)); driveFilterSlider.setBounds (sz(861, 22, 24, 28)); driveVolSlider.setBounds (sz(887, 22, 24, 28)); springDecaySlider.setBounds (sz(924, 22, 24, 28)); springToneSlider.setBounds (sz(952, 22, 24, 28)); delayTimeSlider.setBounds (sz(990, 22, 24, 28)); delayFbSlider.setBounds (sz(1016, 22, 24, 28)); delayModRateSlider.setBounds (sz(1042, 22, 24, 28)); delayModDepthSlider.setBounds (sz(1068, 22, 24, 28)); masterVolSlider.setBounds (sz(1104, 18, 36, 36)); masterClipperSlider.setBounds (sz(1146, 18, 36, 36));

    int seqX = 15; int seqY = 362; int seqW = 1162; int stepsStartX = seqX + 152; int totalStepsWidth = seqW - 192;

    for (int i = 0; i < 12; ++i) {
        int x = 10 + i * 98;
        bool isTriplet = audioProcessor.apvts.getRawParameterValue ("triplet" + juce::String(i))->load() > 0.5f;
        int maxLen = (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(i))->load();
        int numSteps = isTriplet ? 12 : (maxLen > 0 ? maxLen : 16);
        int stepW_int = totalStepsWidth / numSteps;
        int btnW_int = stepW_int - 6;
        
        muteButtons[i].setBounds (sz(x + 18, 134, 18, 16)); soloButtons[i].setBounds (sz(x + 56, 134, 18, 16));
        sampleSourceSelectors[i].setBounds (sz(x + 6, 153, 80, 16)); sampleVariantSelectors[i].setBounds (sz(x + 6, 171, 80, 16));
        sampleSourceSelectors[i].setColour(juce::ComboBox::textColourId, juce::Colours::black); sampleVariantSelectors[i].setColour(juce::ComboBox::textColourId, juce::Colours::black);

        volumeSliders[i].setBounds     (sz(x + 8, 202, 32, 28)); panSliders[i].setBounds        (sz(x + 52, 202, 32, 28));
        pitchSliders[i].setBounds      (sz(x + 8, 243, 32, 28)); toneSliders[i].setBounds       (sz(x + 52, 243, 32, 28));
        attackSliders[i].setBounds     (sz(x + 8, 284, 32, 28)); decaySliders[i].setBounds      (sz(x + 52, 284, 32, 28));
        springSendSliders[i].setBounds (sz(x + 8, 325, 32, 28)); delaySendSliders[i].setBounds  (sz(x + 52, 325, 32, 28));

        tripletButtons[i].setBounds (sz(seqX + 32, seqY + 8 + (i * 24), 28, 20)); 
        minusButtons[i].setBounds   (sz(seqX + 63, seqY + 8 + (i * 24), 16, 20)); 
        lengthLabels[i].setBounds   (sz(seqX + 81, seqY + 8 + (i * 24), 22, 20)); 
        plusButtons[i].setBounds    (sz(seqX + 105, seqY + 8 + (i * 24), 16, 20));

        for (int step = 0; step < 16; ++step) {
            if (step < numSteps) { 
                stepButtons[i][step].setVisible (true); 
                int exactStepX = stepsStartX + (step * stepW_int);
                stepButtons[i][step].setBounds (sz(exactStepX, seqY + 8 + (i * 24), btnW_int, 20)); 
            } 
            else {
                stepButtons[i][step].setVisible (false);
            }
        }
    }

    bool fillIsTriplet = audioProcessor.apvts.getRawParameterValue ("tripletFill")->load() > 0.5f;
    int numFillSteps = fillIsTriplet ? 12 : 16;
    int fillStepW_int = totalStepsWidth / numFillSteps;
    
    tripletFillButton.setBounds (sz(seqX + 32, seqY + 10 + (12 * 24), 28, 20));

    for (int step = 0; step < 16; ++step) {
        if (step < numFillSteps) {
            fillStepButtons[step].setVisible(true);
            int exactFillX = stepsStartX + (step * fillStepW_int);
            fillStepButtons[step].setBounds (sz(exactFillX, seqY + 10 + (12 * 24), fillStepW_int - 6, 20));
        } else {
            fillStepButtons[step].setVisible(false);
        }
    }

    if (cornerResizer) cornerResizer->setBounds (getWidth() - 20, getHeight() - 20, 20, 20);
}