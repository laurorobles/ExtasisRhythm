#include "PluginProcessor.h"
#include "PluginEditor.h"

class CustomComboBoxLookAndFeel : public juce::LookAndFeel_V4 {
public:
    void drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box) override {
        juce::ignoreUnused (box);
        auto bounds = juce::Rectangle<int> (0, 0, width, height).toFloat();
        g.setColour (juce::Colour (0xffe0e0e0));
        g.fillRoundedRectangle (bounds, 3.0f);
        g.setColour (juce::Colour (0xffb0b0b0));
        g.drawRoundedRectangle (bounds, 3.0f, 1.0f);

        juce::Path p;
        p.addTriangle (width - 12.0f, height * 0.4f, width - 6.0f, height * 0.4f, width - 9.0f, height * 0.65f);
        g.setColour (juce::Colour (0xff555555));
        g.fillPath (p);
    }
};

static CustomComboBoxLookAndFeel customComboBoxLAF;

ExtasisRhythmEditor::ExtasisRhythmEditor (ExtasisRhythmProcessor& proc)
    : AudioProcessorEditor (&proc), audioProcessor (proc) {
    setResizable (true, true);
    getConstrainer()->setFixedAspectRatio (1192.0 / 740.0);
    getConstrainer()->setMinimumSize (900, 560);

    addAndMakeVisible (playButton);
    playButton.setButtonText ("PLAY"); 
    playButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0));
    playButton.setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
    playButton.onClick = [this] {
        auto* pi = audioProcessor.apvts.getParameter ("isPlaying");
        if (pi) {
            bool current = pi->getValue() > 0.5f;
            pi->beginChangeGesture(); 
            pi->setValueNotifyingHost (current ? 0.0f : 1.0f); 
            pi->endChangeGesture();
        }
    };

    addAndMakeVisible (stopButton);
    stopButton.setButtonText ("STOP"); 
    stopButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0));
    stopButton.setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
    stopButton.onClick = [this] {
        if (auto* pi = audioProcessor.apvts.getParameter ("isPlaying")) { pi->beginChangeGesture(); pi->setValueNotifyingHost (0.0f); pi->endChangeGesture(); }
        for (int i = 0; i < 12; ++i) audioProcessor.channelSteps[i] = 0;
    };

    addAndMakeVisible (bpmSlider); 
    bpmSlider.setSliderStyle (juce::Slider::LinearBar); 
    bpmSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 45, 18);
    bpmAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "bpm", bpmSlider);

    addAndMakeVisible (masterVolSlider); masterVolSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); masterVolSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    masterVolSlider.setLookAndFeel (&simpleKnobLAF);
    masterVolAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "masterVolume", masterVolSlider);

    addAndMakeVisible (masterClipperSlider); masterClipperSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); masterClipperSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    masterClipperSlider.setLookAndFeel (&simpleKnobLAF);
    masterClipAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "masterClipper", masterClipperSlider);

    addAndMakeVisible (flangerOnButton);
    flangerOnButton.setClickingTogglesState (true);
    flangerOnButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff444444));
    flangerOnButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff3498db));
    flangerOnButton.setColour (juce::TextButton::textColourOffId, juce::Colours::grey);
    flangerOnButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    
    auto* flangerParam = audioProcessor.apvts.getRawParameterValue("flangerOn");
    flangerOnButton.setButtonText ((flangerParam && flangerParam->load() > 0.5f) ? "ON" : "OFF");

    flangerOnButton.onClick = [this] {
        flangerOnButton.setButtonText (flangerOnButton.getToggleState() ? "ON" : "OFF");
    };
    flangerOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "flangerOn", flangerOnButton);

    addAndMakeVisible (saveKitButton); addAndMakeVisible (loadKitButton); 
    addAndMakeVisible (resetButton); addAndMakeVisible (seqResetButton);

    saveKitButton.setButtonText ("SAVE"); loadKitButton.setButtonText ("LOAD");
    resetButton.setButtonText ("RESET"); seqResetButton.setButtonText ("SEQ RST");

    saveKitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff2e8b57)); saveKitButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    loadKitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffd2691e)); loadKitButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    resetButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffff6600)); resetButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    seqResetButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff9b59b6)); seqResetButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);

    resetButton.onClick = [this] { audioProcessor.resetAllParameters(); audioProcessor.resetSequencer(); };
    seqResetButton.onClick = [this] { audioProcessor.resetSequencer(); };

    saveKitButton.onClick = [this] {
        saveChooser = std::make_unique<juce::FileChooser> ("Save", juce::File::getSpecialLocation(juce::File::userHomeDirectory), "*.xml");
        saveChooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles, [this] (const juce::FileChooser& c) {
            if (c.getResult() != juce::File()) audioProcessor.saveCustomPreset(c.getResult());
        });
    };
    loadKitButton.onClick = [this] {
        loadChooser = std::make_unique<juce::FileChooser> ("Load", juce::File::getSpecialLocation(juce::File::userHomeDirectory), "*.xml");
        loadChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this] (const juce::FileChooser& c) {
            if (c.getResult() != juce::File()) audioProcessor.loadCustomPreset(c.getResult());
        });
    };

    addAndMakeVisible (globalKitSelector);
    globalKitSelector.setLookAndFeel (&customComboBoxLAF);
    globalKitSelector.addItemList (audioProcessor.getDrumKitNames(), 1);
    globalKitAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, "globalKitChoice", globalKitSelector);
    globalKitSelector.onChange = [this] { 
        int kitIdx = globalKitSelector.getSelectedId() - 1;
        audioProcessor.loadGlobalDrumKit (kitIdx);
        for (int i = 0; i < 12; ++i) {
            sampleVariantSelectors[i].clear(juce::dontSendNotification);
            sampleVariantSelectors[i].addItemList (audioProcessor.getVariantsForChannel(kitIdx, i), 1);
            sampleVariantSelectors[i].setSelectedId (1, juce::dontSendNotification);
        }
    };

    juce::String pNames = "ABCDEFGH";
    for (int i = 0; i < 8; ++i) {
        addAndMakeVisible (patternButtons[i]);
        patternButtons[i].setButtonText (juce::String::charToString(pNames[i]));
        patternButtons[i].setColour (juce::TextButton::buttonColourId, i == 0 ? juce::Colour (0xff3498db) : juce::Colour (0xffcccccc));
        patternButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colours::white);
        patternButtons[i].onClick = [this, i] {
            for (int b = 0; b < 8; ++b) patternButtons[b].setColour (juce::TextButton::buttonColourId, b == i ? juce::Colour (0xff3498db) : juce::Colour (0xffcccccc));
            audioProcessor.changePattern (i);
            activePatternButton = i;
        };
    }

    auto mkKnob = [this] (juce::Slider& sl, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& at, const juce::String& id) {
        sl.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); 
        sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        sl.setLookAndFeel (&simpleKnobLAF);
        addAndMakeVisible (sl); 
        at = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, id, sl);
    };

    mkKnob (masterHpfSlider, hpfAtt, "masterHpf"); mkKnob (masterHpfResSlider, hpfResAtt, "masterHpfRes");
    mkKnob (masterLpfSlider, lpfAtt, "masterLpf"); mkKnob (masterLpfResSlider, lpfResAtt, "masterLpfRes");
    mkKnob (pcmBitsSlider, pcmBitsAtt, "pcmBits"); mkKnob (pcmRateSlider, pcmRateAtt, "pcmRate");
    mkKnob (flangerRateSlider, flRateAtt, "flangerRate"); mkKnob (flangerFbSlider, flFbAtt, "flangerFeedback");
    mkKnob (transAttackSlider, transAttAtt, "transientAttack"); mkKnob (transSustainSlider, transSusAtt, "transientSustain");
    mkKnob (springDecaySlider, springDecAtt, "springDecay"); mkKnob (springToneSlider, springToneAtt, "springTone");

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
        
        addAndMakeVisible (sampleSourceSelectors[i]); 
        sampleSourceSelectors[i].setLookAndFeel (&customComboBoxLAF);
        sampleSourceSelectors[i].addItemList (kitNames, 1);
        sampleSourceAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, "sampleSource_" + chStr, sampleSourceSelectors[i]);
        
        addAndMakeVisible (sampleVariantSelectors[i]);
        sampleVariantSelectors[i].setLookAndFeel (&customComboBoxLAF);
        int currentKit = sampleSourceSelectors[i].getSelectedId() - 1;
        if (currentKit < 0) currentKit = 0;
        sampleVariantSelectors[i].addItemList (audioProcessor.getVariantsForChannel (currentKit, i), 1);
        
        sampleSourceSelectors[i].onChange = [this, i] { 
            int kitIdx = sampleSourceSelectors[i].getSelectedId() - 1;
            sampleVariantSelectors[i].clear(juce::dontSendNotification);
            auto variants = audioProcessor.getVariantsForChannel (kitIdx, i);
            sampleVariantSelectors[i].addItemList (variants, 1);
            sampleVariantSelectors[i].setSelectedId (1, juce::dontSendNotification);
            if (!variants.isEmpty()) audioProcessor.loadSampleForChannel (i, kitIdx, variants[0]);
        };

        sampleVariantSelectors[i].onChange = [this, i] {
            int kitIdx = sampleSourceSelectors[i].getSelectedId() - 1;
            auto variants = audioProcessor.getVariantsForChannel (kitIdx, i);
            int varIdx = sampleVariantSelectors[i].getSelectedId() - 1;
            if (varIdx >= 0 && varIdx < variants.size()) {
                audioProcessor.loadSampleForChannel (i, kitIdx, variants[varIdx]);
            }
        };

        addAndMakeVisible (muteButtons[i]); 
        muteButtons[i].setButtonText ("M"); 
        muteButtons[i].setClickingTogglesState (true);
        muteButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
        muteButtons[i].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffff3333));
        muteButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
        muteButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        muteAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "mute" + chStr, muteButtons[i]);

        addAndMakeVisible (soloButtons[i]); 
        soloButtons[i].setButtonText ("S"); 
        soloButtons[i].setClickingTogglesState (true);
        soloButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
        soloButtons[i].setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff2ecc71));
        soloButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
        soloButtons[i].setColour (juce::TextButton::textColourOnId, juce::Colours::white);
        soloAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "solo" + chStr, soloButtons[i]);

        mkKnob (volumeSliders[i], volAtts[i], "gain" + chStr); mkKnob (panSliders[i], panAtts[i], "pan" + chStr);
        mkKnob (pitchSliders[i], pitchAtts[i], "pitch" + chStr); mkKnob (toneSliders[i], toneAtts[i], "tone" + chStr);
        mkKnob (attackSliders[i], attAtts[i], "attack" + chStr); mkKnob (decaySliders[i], decAtts[i], "decay" + chStr);
        mkKnob (sendSliders[i], sendAtts[i], "springSend" + chStr);

        stepLengthAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "length" + chStr, hiddenLengthSliders[i]);
        
        addAndMakeVisible (minusButtons[i]);
        minusButtons[i].setButtonText ("-");
        minusButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
        minusButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
        minusButtons[i].onClick = [this, i] {
            double val = hiddenLengthSliders[i].getValue() - 1.0;
            if (val < 1.0) val = 1.0;
            hiddenLengthSliders[i].setValue (val, juce::sendNotification);
            updateLengthLabel (i);
            resized();
        };

        addAndMakeVisible (plusButtons[i]);
        plusButtons[i].setButtonText ("+");
        plusButtons[i].setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
        plusButtons[i].setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
        plusButtons[i].onClick = [this, i] {
            double val = hiddenLengthSliders[i].getValue() + 1.0;
            if (val > 16.0) val = 16.0;
            hiddenLengthSliders[i].setValue (val, juce::sendNotification);
            updateLengthLabel (i);
            resized();
        };

        addAndMakeVisible (lengthLabels[i]);
        lengthLabels[i].setColour (juce::Label::textColourId, juce::Colours::black);
        lengthLabels[i].setFont (juce::FontOptions (11.0f, juce::Font::bold));
        lengthLabels[i].setJustificationType (juce::Justification::centred);
        updateLengthLabel (i);

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
    addAndMakeVisible (cornerResizer.get());
    setSize (1192, 740); startTimerHz (30);
}

ExtasisRhythmEditor::~ExtasisRhythmEditor() {
    stopTimer();
    masterVolSlider.setLookAndFeel (nullptr);
    masterClipperSlider.setLookAndFeel (nullptr);
    masterHpfSlider.setLookAndFeel (nullptr); masterHpfResSlider.setLookAndFeel (nullptr);
    masterLpfSlider.setLookAndFeel (nullptr); masterLpfResSlider.setLookAndFeel (nullptr);
    pcmBitsSlider.setLookAndFeel (nullptr); pcmRateSlider.setLookAndFeel (nullptr);
    flangerRateSlider.setLookAndFeel (nullptr); flangerFbSlider.setLookAndFeel (nullptr);
    transAttackSlider.setLookAndFeel (nullptr); transSustainSlider.setLookAndFeel (nullptr);
    springDecaySlider.setLookAndFeel (nullptr); springToneSlider.setLookAndFeel (nullptr);
    globalKitSelector.setLookAndFeel (nullptr);
    for (int i = 0; i < 12; ++i) {
        sampleSourceSelectors[i].setLookAndFeel (nullptr);
        sampleVariantSelectors[i].setLookAndFeel (nullptr);
        volumeSliders[i].setLookAndFeel (nullptr); panSliders[i].setLookAndFeel (nullptr);
        pitchSliders[i].setLookAndFeel (nullptr); toneSliders[i].setLookAndFeel (nullptr);
        attackSliders[i].setLookAndFeel (nullptr); decaySliders[i].setLookAndFeel (nullptr);
        sendSliders[i].setLookAndFeel (nullptr);
    }
}

void ExtasisRhythmEditor::mouseDown (const juce::MouseEvent& e) {
    float s = (float) getWidth() / 1192.0f; auto pos = e.getPosition().toFloat() / s;
    for (int i = 0; i < 12; ++i) {
        if (juce::Rectangle<float> ((float)(10 + i * 98), 115.0f, 92.0f, 45.0f).contains (pos)) {
            audioProcessor.triggerChannel (i, 1.0f); break;
        }
    }
}

void ExtasisRhythmEditor::updateStepButtonVisuals (int ch, int step) {
    auto* par = audioProcessor.apvts.getRawParameterValue ("step_" + juce::String(ch) + "_" + juce::String(step));
    if (par) {
        int v = (int)(*par + 0.5f);
        juce::Colour cols[4] = { juce::Colour(0xffe0e0e0), juce::Colour(0xffffcc00), juce::Colour(0xffff6600), juce::Colour(0xffff0000) };
        stepButtons[ch][step].setColour (juce::TextButton::buttonColourId, cols[v]);
    }
}

void ExtasisRhythmEditor::updateFillButtonVisuals (int step) {
    auto* par = audioProcessor.apvts.getRawParameterValue ("fill_step_" + juce::String(step));
    if (par) {
        int v = (int)(*par + 0.5f);
        fillStepButtons[step].setButtonText ("");
        juce::Colour cols[3] = { juce::Colour(0xffe0e0e0), juce::Colour(0xff2ecc71), juce::Colour(0xff3498db) };
        fillStepButtons[step].setColour (juce::TextButton::buttonColourId, cols[v]);
    }
}

void ExtasisRhythmEditor::updateLengthLabel (int i) {
    int len = (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(i))->load();
    if (len <= 0) len = 16;
    lengthLabels[i].setText (juce::String (len), juce::dontSendNotification);
}

void ExtasisRhythmEditor::paint (juce::Graphics& g) {
    float s = (float) getWidth() / 1192.0f; g.saveState(); g.addTransform (juce::AffineTransform::scale (s));
    g.fillAll (juce::Colour (0xffe6e6e6));
    g.setColour (juce::Colour (0xffd4d4d4)); g.fillRect (0, 0, 1192, 115);

    g.setColour (juce::Colours::black.withAlpha (0.15f));
    g.fillRect (0, 115, 1192, 4);

    g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    g.setColour (juce::Colours::black.withAlpha(0.7f));
    g.drawText ("coded by @laurorobles", 18, 42, 150, 14, juce::Justification::left);

    g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.setColour (juce::Colours::black.withAlpha(0.2f));
    g.drawText ("ExtasisRhythm", 17, 13, 180, 32, juce::Justification::centredLeft);
    g.setColour (juce::Colour (0xff3498db));
    g.drawText ("ExtasisRhythm", 15, 11, 180, 32, juce::Justification::centredLeft);

    // Texto eliminado por completo de la esquina superior derecha

    g.setColour (juce::Colours::white.withAlpha(0.5f));
    g.drawHorizontalLine (116, 0.0f, 1192.0f);
    g.setColour (juce::Colours::black.withAlpha(0.3f));
    g.drawHorizontalLine (114, 0.0f, 1192.0f);

    auto drawModuleBox = [&g] (int x, int y, int w, int h, const juce::String& title) {
        g.setColour (juce::Colour (0xffcccccc));
        g.fillRoundedRectangle ((float)x, (float)y, (float)w, (float)h, 4.0f);
        g.setColour (juce::Colours::black.withAlpha(0.15f));
        g.drawRoundedRectangle ((float)x, (float)y, (float)w, (float)h, 4.0f, 1.0f);
        g.setFont (juce::FontOptions (9.0f, juce::Font::bold));
        g.setColour (juce::Colour (0xff222222));
        g.drawText (title, x, y + 2, w, 12, juce::Justification::centred);
    };

    drawModuleBox (397, 2, 106, 108, "PATTERNS");
    drawModuleBox (511, 2, 62, 108, "HPF");
    drawModuleBox (577, 2, 62, 108, "LPF");
    drawModuleBox (643, 2, 62, 108, "PCM");
    drawModuleBox (709, 2, 62, 108, "FLANGER");
    drawModuleBox (775, 2, 62, 108, "TRANS");
    drawModuleBox (841, 2, 62, 108, "SPRING");
    drawModuleBox (907, 2, 126, 108, "MASTER");

    g.setFont (juce::FontOptions (7.5f, juce::Font::bold)); g.setColour (juce::Colour (0xff444444));
    g.drawText ("CUT", 515, 60, 30, 10, juce::Justification::centred);
    g.drawText ("RES", 543, 60, 30, 10, juce::Justification::centred);
    g.drawText ("CUT", 581, 60, 30, 10, juce::Justification::centred);
    g.drawText ("RES", 609, 60, 30, 10, juce::Justification::centred);
    g.drawText ("BITS", 647, 60, 30, 10, juce::Justification::centred);
    g.drawText ("RATE", 675, 60, 30, 10, juce::Justification::centred);
    g.drawText ("RATE", 713, 60, 30, 10, juce::Justification::centred);
    g.drawText ("FB", 741, 60, 30, 10, juce::Justification::centred);
    g.drawText ("ATT", 779, 60, 30, 10, juce::Justification::centred);
    g.drawText ("SUS", 807, 60, 30, 10, juce::Justification::centred);
    g.drawText ("DEC", 845, 60, 30, 10, juce::Justification::centred);
    g.drawText ("TONE", 873, 60, 30, 10, juce::Justification::centred);
    g.drawText ("VOL", 927, 60, 36, 10, juce::Justification::centred);
    g.drawText ("CLIP", 975, 60, 36, 10, juce::Justification::centred);

    juce::String labels[12] = { "BD", "SD", "CH", "OH", "CP", "RS", "HT", "MT", "LT", "CB", "CR", "RD" };
    for (int i = 0; i < 12; ++i) {
        int x = 10 + i * 98; 
        
        g.setColour (juce::Colours::black.withAlpha (0.15f));
        g.fillRoundedRectangle ((float)(x + 3), 118.0f, 92.0f, 255.0f, 4.0f);

        juce::ColourGradient cg (juce::Colour (0xfffcfcfc), (float)x, 115.0f, juce::Colour (0xffe0e0e0), (float)x, 370.0f, false);
        g.setGradientFill (cg);
        g.fillRoundedRectangle ((float)x, 115.0f, 92.0f, 255.0f, 4.0f);
        
        g.setColour (juce::Colour (0xffb8b8b8)); 
        g.drawRoundedRectangle ((float)x, 115.0f, 92.0f, 255.0f, 4.0f, 1.0f);
        
        g.setColour (juce::Colours::white.withAlpha(0.9f));
        g.drawHorizontalLine (116, (float)(x + 2), (float)(x + 90));

        g.setColour (juce::Colours::black.withAlpha(0.4f));
        g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        g.drawText (labels[i], x + 36, 134, 20, 16, juce::Justification::centred);
        g.setColour (juce::Colour (0xff222222)); 
        g.drawText (labels[i], x + 35, 133, 20, 16, juce::Justification::centred);

        bool isLit = audioProcessor.flashCounters[i].load() > 0;
        g.setColour (isLit ? juce::Colour (0xff3498db) : juce::Colour (0xff555555));
        g.fillEllipse ((float)(x + 42), 120.0f, 8.0f, 8.0f);
        if (isLit) {
            g.setColour (juce::Colours::white.withAlpha(0.6f));
            g.fillEllipse ((float)(x + 44), 122.0f, 3.0f, 3.0f);
        }

        g.setFont (juce::FontOptions (7.5f, juce::Font::bold)); g.setColour (juce::Colour (0xff444444));
        g.drawText ("VOL", x + 12, 240, 28, 10, juce::Justification::centred);
        g.drawText ("PAN", x + 52, 240, 28, 10, juce::Justification::centred);
        g.drawText ("PITCH", x + 12, 280, 28, 10, juce::Justification::centred);
        g.drawText ("TONE", x + 52, 280, 28, 10, juce::Justification::centred);
        g.drawText ("ATT", x + 12, 320, 28, 10, juce::Justification::centred);
        g.drawText ("DEC", x + 52, 320, 28, 10, juce::Justification::centred);
        g.drawText ("SPRING", x + 31, 360, 30, 10, juce::Justification::centred);
    }
    
    int seqX = 40;
    int seqY = 380;
    int seqW = 1112;
    int seqH = 340;

    g.setColour (juce::Colour (0xffd8d8d8));
    g.fillRoundedRectangle ((float)seqX, (float)seqY, (float)seqW, (float)seqH, 6.0f);
    
    g.setColour (juce::Colours::black.withAlpha(0.2f));
    g.drawRoundedRectangle ((float)seqX, (float)seqY, (float)seqW, (float)seqH, 6.0f, 2.0f);

    for (int i = 0; i < 12; ++i) {
        g.setColour (juce::Colours::black.withAlpha(0.3f));
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        g.drawText (labels[i], seqX + 11, 386 + (i * 24), 22, 20, juce::Justification::centred);
        g.setColour (juce::Colour (0xff222222));
        g.drawText (labels[i], seqX + 10, 385 + (i * 24), 22, 20, juce::Justification::centred);

        int act = audioProcessor.channelSteps[i].load();
        g.setColour (juce::Colour (0xff3498db).withAlpha(0.05f));
        g.fillRect (seqX + 104 + (act * 63), seqY + 5 + (i * 24), 60, 20);
    }

    g.setColour (juce::Colours::black.withAlpha(0.3f));
    g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    g.drawText ("FILL", seqX + 76, 388 + (12 * 24), 30, 20, juce::Justification::centred);
    g.setColour (juce::Colour (0xff222222));
    g.drawText ("FILL", seqX + 75, 387 + (12 * 24), 30, 20, juce::Justification::centred);

    for (int i = 0; i < 12; ++i) {
        updateLengthLabel(i);
        for (int step = 0; step < 16; ++step) updateStepButtonVisuals (i, step);
    }
    for (int step = 0; step < 16; ++step) updateFillButtonVisuals (step);

    g.setColour (juce::Colour (0xff111111)); 
    g.fillRect (1158, 383, 24, 335);
    g.setColour (juce::Colour (0xff333333));
    g.drawRect (1158.0f, 383.0f, 24.0f, 335.0f, 1.0f);

    auto rmsToNormalizedDb = [] (float rms) {
        if (rms <= 0.00001f) return 0.0f;
        float db = 20.0f * std::log10 (rms);
        return juce::jlimit (0.0f, 1.0f, (db - (-48.0f)) / (3.0f - (-48.0f)));
    };

    float mL = rmsToNormalizedDb (audioProcessor.outputLevelL.load());
    float mR = rmsToNormalizedDb (audioProcessor.outputLevelR.load());

    int numSegments = 24;
    float segHeight = 327.0f / (float)numSegments;

    for (int seg = 0; seg < numSegments; ++seg) {
        float threshold = 1.0f - ((float)seg / (float)numSegments);
        juce::Colour ledColor;
        if (threshold > 0.85f) ledColor = juce::Colour (0xffff3333);
        else if (threshold > 0.65f) ledColor = juce::Colour (0xffffcc00);
        else ledColor = juce::Colour (0xff2ecc71);

        float yPos = 387.0f + (float)seg * segHeight;

        if (mL >= threshold) g.setColour (ledColor); else g.setColour (juce::Colour (0xff222222));
        g.fillRect (1161.0f, yPos, 8.0f, segHeight - 2.0f);

        if (mR >= threshold) g.setColour (ledColor); else g.setColour (juce::Colour (0xff222222));
        g.fillRect (1171.0f, yPos, 8.0f, segHeight - 2.0f);
    }

    for (int i = 0; i < 12; ++i) {
        int act = audioProcessor.channelSteps[i].load(); g.setColour (juce::Colour (0xff3498db));
        g.drawRect (144.0f + ((float)act * 63.0f), 385.0f + ((float)i * 24.0f), 52.0f, 20.0f, 2.0f);
    }
    g.restoreState();
}

void ExtasisRhythmEditor::resized() {
    float s = (float) getWidth() / 1192.0f;
    auto sz = [s](int x, int y, int w, int h) { return juce::Rectangle<int> ((int)((float)x*s), (int)((float)y*s), (int)((float)w*s), (int)((float)h*s)); };

    playButton.setBounds (sz(185, 18, 48, 21)); stopButton.setBounds (sz(237, 18, 48, 21));
    resetButton.setBounds (sz(185, 43, 48, 21)); seqResetButton.setBounds (sz(237, 43, 48, 21));

    bpmSlider.setBounds (sz(185, 68, 100, 24));
    globalKitSelector.setBounds (sz(293, 22, 96, 22));

    for (int i = 0; i < 8; ++i) {
        int row = i / 4;
        int col = i % 4;
        patternButtons[i].setBounds (sz(403 + (col * 24), 18 + (row * 22), 22, 20));
    }
    saveKitButton.setBounds (sz(403, 68, 46, 18));
    loadKitButton.setBounds (sz(453, 68, 46, 18));

    masterHpfSlider.setBounds (sz(515, 26, 24, 28)); masterHpfResSlider.setBounds (sz(543, 26, 24, 28));
    masterLpfSlider.setBounds (sz(581, 26, 24, 28)); masterLpfResSlider.setBounds (sz(609, 26, 24, 28));
    pcmBitsSlider.setBounds (sz(647, 26, 24, 28)); pcmRateSlider.setBounds (sz(675, 26, 24, 28));
    
    flangerRateSlider.setBounds (sz(713, 22, 24, 28)); 
    flangerFbSlider.setBounds   (sz(741, 22, 24, 28));
    flangerOnButton.setBounds   (sz(718, 62, 44, 20));

    transAttackSlider.setBounds (sz(779, 26, 24, 28)); transSustainSlider.setBounds (sz(807, 26, 24, 28));
    springDecaySlider.setBounds (sz(845, 26, 24, 28)); springToneSlider.setBounds (sz(873, 26, 24, 28));
    
    masterVolSlider.setBounds (sz(919, 22, 36, 36));
    masterClipperSlider.setBounds (sz(967, 22, 36, 36));

    for (int i = 0; i < 12; ++i) {
        int x = 10 + i * 98;
        int maxLen = (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(i))->load();
        
        muteButtons[i].setBounds   (sz(x + 18, 154, 18, 16));
        soloButtons[i].setBounds   (sz(x + 56, 154, 18, 16));
        sampleSourceSelectors[i].setBounds  (sz(x + 6, 174, 80, 16));
        sampleVariantSelectors[i].setBounds(sz(x + 6, 192, 80, 16));

        volumeSliders[i].setBounds (sz(x + 12, 210, 28, 28)); 
        panSliders[i].setBounds    (sz(x + 52, 210, 28, 28));
        pitchSliders[i].setBounds  (sz(x + 12, 250, 28, 28)); 
        toneSliders[i].setBounds   (sz(x + 52, 250, 28, 28));
        attackSliders[i].setBounds (sz(x + 12, 290, 28, 28)); 
        decaySliders[i].setBounds  (sz(x + 52, 290, 28, 28));
        sendSliders[i].setBounds   (sz(x + 32, 330, 28, 28));

        minusButtons[i].setBounds  (sz(72, 385 + (i * 24), 18, 20));
        lengthLabels[i].setBounds  (sz(92, 385 + (i * 24), 22, 20));
        plusButtons[i].setBounds   (sz(116, 385 + (i * 24), 18, 20));

        for (int step = 0; step < 16; ++step) {
            if (step < maxLen) {
                stepButtons[i][step].setVisible (true);
                stepButtons[i][step].setBounds (sz(144 + step * 63, 385 + i * 24, 52, 20));
            } else {
                stepButtons[i][step].setVisible (false);
            }
        }
    }

    for (int step = 0; step < 16; ++step)
        fillStepButtons[step].setBounds (sz(144 + step * 63, 385 + (12 * 24) + 5, 52, 20));

    if (cornerResizer) cornerResizer->setBounds (getWidth() - 20, getHeight() - 20, 20, 20);
}

void ExtasisRhythmEditor::timerCallback() { repaint(); }
