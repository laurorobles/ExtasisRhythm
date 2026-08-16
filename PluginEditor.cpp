#include "PluginProcessor.h"
#include "PluginEditor.h"

ExtasisRhythmEditor::ExtasisRhythmEditor (ExtasisRhythmProcessor& proc)
    : AudioProcessorEditor (&proc), audioProcessor (proc) {
    setLookAndFeel (&tr909Feel);
    setResizable (true, true);
    getConstrainer()->setFixedAspectRatio (1000.0 / 680.0);
    getConstrainer()->setMinimumSize (800, 544);
    getConstrainer()->setMaximumSize (1600, 1088);

    addAndMakeVisible (playButton);
    playButton.setButtonText ("PLAY"); playButton.setClickingTogglesState (true);
    playButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff2ecc71));
    playButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0));
    playAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "isPlaying", playButton);

    addAndMakeVisible (stopButton);
    stopButton.setButtonText ("STOP"); stopButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0));
    stopButton.onClick = [this] {
        if (auto* pi = audioProcessor.apvts.getParameter ("isPlaying")) { pi->beginChangeGesture(); pi->setValueNotifyingHost (0.0f); pi->endChangeGesture(); }
        for (int i = 0; i < 10; ++i) audioProcessor.channelSteps[i] = 0;
    };

    addAndMakeVisible (bpmSlider); bpmSlider.setSliderStyle (juce::Slider::LinearBar); bpmSlider.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 40, 18);
    bpmAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "bpm", bpmSlider);

    addAndMakeVisible (masterVolSlider); masterVolSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); masterVolSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    masterVolAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "masterVolume", masterVolSlider);

    addAndMakeVisible (clipperButton);
    clipperButton.setButtonText ("CLIP"); clipperButton.setClickingTogglesState (true);
    clipperButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffff6600));
    clipperButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
    clipperButton.setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
    clipperAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "masterClipper", clipperButton);

    // Botón de Flanger debajo de sus perillas
    addAndMakeVisible (flangerOnButton);
    flangerOnButton.setButtonText (""); flangerOnButton.setClickingTogglesState (true);
    flangerOnButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff3498db));
    flangerOnButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffb0b0b0));
    flangerOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "flangerOn", flangerOnButton);

    addAndMakeVisible (saveKitButton); addAndMakeVisible (loadKitButton); addAndMakeVisible (resetButton);
    saveKitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); saveKitButton.setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
    loadKitButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc)); loadKitButton.setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
    resetButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffff6600)); resetButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    resetButton.onClick = [this] { audioProcessor.resetAllParameters(); };

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
    globalKitSelector.addItemList (audioProcessor.getDrumKitNames(), 1);
    globalKitAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, "globalKitChoice", globalKitSelector);
    globalKitSelector.onChange = [this] { audioProcessor.loadGlobalDrumKit (globalKitSelector.getSelectedId() - 1); };

    auto mkKnob = [this] (juce::Slider& sl, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& at, const juce::String& id) {
        sl.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible (sl); at = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, id, sl);
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
    for (int i = 0; i < 10; ++i) {
        juce::String chStr = juce::String(i);
        addAndMakeVisible (sampleSourceSelectors[i]); sampleSourceSelectors[i].addItemList (kitNames, 1);
        sampleSourceAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (audioProcessor.apvts, "sampleSource_" + chStr, sampleSourceSelectors[i]);
        sampleSourceSelectors[i].onChange = [this, i] { audioProcessor.loadSampleForChannel (i, sampleSourceSelectors[i].getSelectedId() - 1); };

        mkKnob (volumeSliders[i], volAtts[i], "gain" + chStr); mkKnob (panSliders[i], panAtts[i], "pan" + chStr);
        mkKnob (pitchSliders[i], pitchAtts[i], "pitch" + chStr); mkKnob (toneSliders[i], toneAtts[i], "tone" + chStr);
        mkKnob (attackSliders[i], attAtts[i], "attack" + chStr); mkKnob (decaySliders[i], decAtts[i], "decay" + chStr);
        mkKnob (sendSliders[i], sendAtts[i], "springSend" + chStr);

        addAndMakeVisible (stepLengthSliders[i]);
        stepLengthSliders[i].setSliderStyle (juce::Slider::IncDecButtons); stepLengthSliders[i].setTextBoxStyle (juce::Slider::TextBoxLeft, false, 32, 20);
        stepLengthAtts[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "length" + chStr, stepLengthSliders[i]);

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

    resizableCorner = std::make_unique<juce::ResizableCornerComponent> (this, getConstrainer());
    addAndMakeVisible (resizableCorner.get());
    setSize (1000, 680); startTimerHz (30);
}

ExtasisRhythmEditor::~ExtasisRhythmEditor() { setLookAndFeel (nullptr); }

void ExtasisRhythmEditor::mouseDown (const juce::MouseEvent& e) {
    float s = (float) getWidth() / 1000.0f; auto pos = e.getPosition().toFloat() / s;
    for (int i = 0; i < 10; ++i) {
        if (juce::Rectangle<float> ((float)(10 + i * 96), 115.0f, 92.0f, 45.0f).contains (pos)) {
            audioProcessor.triggerChannel (i, 1.0f); break;
        }
    }
}

void ExtasisRhythmEditor::updateStepButtonVisuals (int ch, int step) {
    auto* par = audioProcessor.apvts.getRawParameterValue ("step_" + juce::String(ch) + "_" + juce::String(step));
    if (par) {
        int v = (int)(*par + 0.5f);
        juce::Colour cols[4] = { juce::Colour(0xffe0e0e0), juce::Colour(0xffff6600), juce::Colour(0xffff4400), juce::Colour(0xffff0000) };
        stepButtons[ch][step].setColour (juce::TextButton::buttonColourId, cols[v]);
    }
}

void ExtasisRhythmEditor::updateFillButtonVisuals (int step) {
    auto* par = audioProcessor.apvts.getRawParameterValue ("fill_step_" + juce::String(step));
    if (par) {
        int v = (int)(*par + 0.5f);
        fillStepButtons[step].setButtonText ("");
        juce::Colour cols[3] = { juce::Colour(0xffe0e0e0), juce::Colour(0xff252525), juce::Colour(0xff000000) };
        fillStepButtons[step].setColour (juce::TextButton::buttonColourId, cols[v]);
    }
}

void ExtasisRhythmEditor::paint (juce::Graphics& g) {
    float s = (float) getWidth() / 1000.0f; g.saveState(); g.addTransform (juce::AffineTransform::scale (s));
    g.fillAll (juce::Colour (0xffe6e6e6));
    g.setColour (juce::Colour (0xffd4d4d4)); g.fillRect (0, 0, 1000, 115);

    g.setColour (juce::Colour (0xff3498db)); g.setFont (juce::FontOptions (24.0f, juce::Font::bold));
    g.drawText ("ExtasisRhythm", 15, 0, 180, 115, juce::Justification::centredLeft);

    // ==========================================
    // TÍTULOS DE MÓDULOS (7 MÓDULOS PERFECTAMENTE DISTRIBUIDOS)
    // ==========================================
    g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); g.setColour (juce::Colour (0xff111111));
    g.drawText ("HPF", 420, 2, 54, 12, juce::Justification::centred);
    g.drawText ("LPF", 494, 2, 54, 12, juce::Justification::centred);
    g.drawText ("PCM", 568, 2, 54, 12, juce::Justification::centred);
    g.drawText ("FLANGER", 642, 2, 64, 12, juce::Justification::centred);
    g.drawText ("TRANS", 726, 2, 54, 12, juce::Justification::centred);
    g.drawText ("SPRING", 800, 2, 54, 12, juce::Justification::centred);
    g.drawText ("MASTER", 884, 2, 78, 12, juce::Justification::centred);

    // ==========================================
    // ETIQUETAS DE CADA KNOB MAESTRO (CENTRADAS)
    // ==========================================
    g.setFont (juce::FontOptions (8.0f, juce::Font::bold)); g.setColour (juce::Colour (0xff444444));
    g.drawText ("CUT", 420, 64, 26, 12, juce::Justification::centred);
    g.drawText ("RES", 448, 64, 26, 12, juce::Justification::centred);
    g.drawText ("CUT", 494, 64, 26, 12, juce::Justification::centred);
    g.drawText ("RES", 522, 64, 26, 12, juce::Justification::centred);
    g.drawText ("BITS", 568, 64, 26, 12, juce::Justification::centred);
    g.drawText ("RATE", 596, 64, 26, 12, juce::Justification::centred);
    g.drawText ("RATE", 642, 64, 30, 12, juce::Justification::centred);
    g.drawText ("FB", 676, 64, 26, 12, juce::Justification::centred);
    g.drawText ("ATT", 726, 64, 26, 12, juce::Justification::centred);
    g.drawText ("SUS", 754, 64, 26, 12, juce::Justification::centred);
    g.drawText ("DEC", 800, 64, 26, 12, juce::Justification::centred);
    g.drawText ("TONE", 828, 64, 26, 12, juce::Justification::centred);
    g.drawText ("VOL", 884, 64, 30, 12, juce::Justification::centred);
    g.drawText ("CLIP", 924, 64, 36, 12, juce::Justification::centred);

    // ==========================================
    // ETIQUETAS DE CANALES DE INSTRUMENTOS
    // ==========================================
    juce::String labels[10] = { "BD", "SD", "CH", "OH", "CP", "CB", "RS", "HT", "MT", "LT" };
    for (int i = 0; i < 10; ++i) {
        int x = 10 + i * 96;
        g.setColour (juce::Colour (0xffe0e0e0)); g.fillRect (x, 115, 92, 255);
        g.setColour (juce::Colour (0xffcccccc)); g.drawRect ((float)x, 115.0f, 92.0f, 255.0f, 1.0f);
        g.setColour (audioProcessor.flashCounters[i].load() > 0 ? juce::Colour (0xff3498db) : juce::Colour (0xffaaaaaa));
        g.fillEllipse ((float)(x + 41), 122.0f, 10.0f, 10.0f);
        g.setColour (juce::Colour (0xff222222)); g.setFont (juce::FontOptions (12.0f, juce::Font::bold)); g.drawText (labels[i], x, 134, 92, 16, juce::Justification::centred);
        g.drawText (labels[i], 2, 385 + (i * 24), 30, 20, juce::Justification::centredRight);

        g.setFont (juce::FontOptions (8.0f, juce::Font::bold)); g.setColour (juce::Colour (0xff555555));
        g.drawText ("VOL", x + 8, 202, 30, 10, juce::Justification::centred);
        g.drawText ("PAN", x + 54, 202, 30, 10, juce::Justification::centred);
        g.drawText ("PITCH", x + 8, 252, 30, 10, juce::Justification::centred);
        g.drawText ("TONE", x + 54, 252, 30, 10, juce::Justification::centred);
        g.drawText ("ATT", x + 8, 302, 30, 10, juce::Justification::centred);
        g.drawText ("DEC", x + 54, 302, 30, 10, juce::Justification::centred);
        g.drawText ("SEND", x + 31, 352, 30, 10, juce::Justification::centred);
    }
    
    g.setColour (juce::Colour (0xff222222)); g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText ("FILL", 52, 385 + (10 * 24) + 5, 30, 20, juce::Justification::centredLeft);

    for (int i = 0; i < 10; ++i) for (int step = 0; step < 16; ++step) updateStepButtonVisuals (i, step);
    for (int step = 0; step < 16; ++step) updateFillButtonVisuals (step);

    // VU Meter con rango -20 dB a +3 dB
    g.setColour (juce::Colour (0xff1a1a1a)); g.fillRect (972, 385, 10, 265); g.fillRect (986, 385, 10, 265);
    
    auto rmsToNormalizedDb = [] (float rms) {
        if (rms <= 0.00001f) return 0.0f;
        float db = 20.0f * std::log10 (rms);
        return juce::jlimit (0.0f, 1.0f, (db - (-20.0f)) / (3.0f - (-20.0f)));
    };

    float mL = rmsToNormalizedDb (audioProcessor.outputLevelL.load());
    float mR = rmsToNormalizedDb (audioProcessor.outputLevelR.load());
    
    juce::ColourGradient grad (juce::Colours::red, 0, 385, juce::Colours::green, 0, 650, false); grad.addColour (0.3, juce::Colours::yellow);
    g.setGradientFill (grad);
    if (mL > 0.0f) g.fillRect (973, 650 - (int)(265.0f * mL), 8, (int)(265.0f * mL));
    if (mR > 0.0f) g.fillRect (987, 650 - (int)(265.0f * mR), 8, (int)(265.0f * mR));

    for (int i = 0; i < 10; ++i) {
        int act = audioProcessor.channelSteps[i].load(); g.setColour (juce::Colour (0xff3498db));
        g.drawRect (88.0f + ((float)act * 54.0f), 385.0f + ((float)i * 24.0f), 52.0f, 20.0f, 2.0f);
    }
    g.restoreState();
}

void ExtasisRhythmEditor::resized() {
    float s = (float) getWidth() / 1000.0f;
    auto sz = [s](int x, int y, int w, int h) { return juce::Rectangle<int> ((int)((float)x*s), (int)((float)y*s), (int)((float)w*s), (int)((float)h*s)); };

    playButton.setBounds (sz(190, 38, 42, 24)); stopButton.setBounds (sz(240, 38, 42, 24));
    bpmSlider.setBounds (sz(190, 68, 92, 24));
    
    globalKitSelector.setBounds (sz(300, 38, 110, 20));
    saveKitButton.setBounds (sz(300, 68, 52, 20)); loadKitButton.setBounds (sz(358, 68, 52, 20));
    resetButton.setBounds (sz(330, 93, 80, 18));

    // Perillas maestras posicionadas de manera limpia y simétrica
    masterHpfSlider.setBounds (sz(418, 32, 24, 28)); masterHpfResSlider.setBounds (sz(448, 32, 24, 28));
    masterLpfSlider.setBounds (sz(492, 32, 24, 28)); masterLpfResSlider.setBounds (sz(522, 32, 24, 28));
    pcmBitsSlider.setBounds (sz(566, 32, 24, 28)); pcmRateSlider.setBounds (sz(596, 32, 24, 28));
    
    flangerRateSlider.setBounds (sz(640, 32, 24, 28)); flangerFbSlider.setBounds (sz(674, 32, 24, 28));
    flangerOnButton.setBounds (sz(660, 64, 16, 16)); // Botón LED On/Off debajo de los knobs de flanger

    transAttackSlider.setBounds (sz(724, 32, 24, 28)); transSustainSlider.setBounds (sz(754, 32, 24, 28));
    springDecaySlider.setBounds (sz(798, 32, 24, 28)); springToneSlider.setBounds (sz(828, 32, 24, 28));
    
    masterVolSlider.setBounds (sz(884, 30, 32, 32));
    clipperButton.setBounds (sz(924, 34, 38, 24));

    for (int i = 0; i < 10; ++i) {
        int x = 10 + i * 96;
        sampleSourceSelectors[i].setBounds (sz(x + 10, 146, 72, 16));
        volumeSliders[i].setBounds (sz(x + 10, 175, 26, 26)); panSliders[i].setBounds (sz(x + 56, 175, 26, 26));
        pitchSliders[i].setBounds (sz(x + 10, 225, 26, 26)); toneSliders[i].setBounds (sz(x + 56, 225, 26, 26));
        attackSliders[i].setBounds (sz(x + 10, 275, 26, 26)); decaySliders[i].setBounds (sz(x + 56, 275, 26, 26));
        sendSliders[i].setBounds (sz(x + 33, 325, 26, 26));
        stepLengthSliders[i].setBounds (sz(34, 385 + (i * 24), 52, 20));

        int maxLen = (int) audioProcessor.apvts.getRawParameterValue ("length" + juce::String(i))->load();
        for (int step = 0; step < 16; ++step) {
            if (step < maxLen) {
                stepButtons[i][step].setVisible (true);
                stepButtons[i][step].setBounds (sz(88 + step * 54, 385 + i * 24, 52, 20));
            } else {
                stepButtons[i][step].setVisible (false);
            }
        }
    }

    for (int step = 0; step < 16; ++step)
        fillStepButtons[step].setBounds (sz(88 + step * 54, 385 + 240, 52, 20));

    if (resizableCorner) resizableCorner->setBounds (getWidth() - 20, getHeight() - 20, 20, 20);
}

void ExtasisRhythmEditor::timerCallback() { repaint(); }
