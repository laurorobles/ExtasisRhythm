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
    clipperButton.setButtonText ("LIMIT"); clipperButton.setClickingTogglesState (true);
    clipperButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffff6600));
    clipperButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
    clipperButton.setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
    clipperAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "masterClipper", clipperButton);

    addAndMakeVisible (flangerOnButton);
    flangerOnButton.setButtonText ("FLANG"); flangerOnButton.setClickingTogglesState (true);
    flangerOnButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff3498db));
    flangerOnButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xffcccccc));
    flangerOnButton.setColour (juce::TextButton::textColourOffId, juce::Colour (0xff222222));
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

    // Etiquetas debajo de cada knob maestro
    g.setFont (juce::FontOptions (8.0f, juce::Font::bold)); g.setColour (juce::Colour (0xff444444));
    g.drawText ("HPF CUT", 435, 64, 40, 12, juce::Justification::centred);
    g.drawText ("HPF RES", 477, 64, 40, 12, juce::Justification::centred);
    g.drawText ("LPF CUT", 535, 64, 40, 12, juce::Justification::centred);
    g.drawText ("LPF RES", 577, 64, 40, 12, juce::Justification::centred);
    g.drawText ("BITS", 635, 64, 30, 12, juce::Justification::centred);
    g.drawText ("RATE", 669, 64, 30, 12, juce::Justification::centred);
    g.drawText ("FRATE", 735, 64, 35, 12, juce::Justification::centred);
    g.drawText ("FFB", 773, 64, 30, 12, juce::Justification::centred);
    g.drawText ("S-DEC", 835, 64, 35, 12, juce::Justification::centred);
    g.drawText ("S-TON", 873, 64, 35, 12, juce::Justification::centred);

    juce::String labels[10] = { "BD", "SD", "CH", "OH", "CP", "CB", "RS", "HT", "MT", "LT" };
    for (int i = 0; i < 10; ++i) {
        int x = 10 + i * 96;
        g.setColour (juce::Colour (0xffe0e0e0)); g.fillRect (x, 115, 92, 255);
        g.setColour (juce::Colour (0xffcccccc)); g.drawRect ((float)x, 115.0f, 92.0f, 255.0f, 1.0f);
        g.setColour (audioProcessor.flashCounters[i].load() > 0 ? juce::Colour (0xff3498db) : juce::Colour (0xffaaaaaa));
        g.fillEllipse ((float)(x + 41), 122.0f, 10.0f, 10.0f);
        g.setColour (juce::Colour (0xff222222)); g.setFont (juce::FontOptions (12.0f, juce::Font::bold)); g.drawText (labels[i], x, 134, 92, 16, juce::Justification::centred);
        g.drawText (labels[i], 2, 385 + (i * 24), 30, 20, juce::Justification::centredRight);
    }
    
    // Etiqueta FILL cambiada de color y pegada a la izquierda del carril (X=50)
    g.setColour (juce::Colour (0xff222222)); g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText ("FILL", 52, 385 + (10 * 24) + 5, 30, 20, juce::Justification::centredLeft);

    for (int i = 0; i < 10; ++i) for (int step = 0; step < 16; ++step) updateStepButtonVisuals (i, step);
    for (int step = 0; step < 16; ++step) updateFillButtonVisuals (step);

    g.setColour (juce::Colour (0xff1a1a1a)); g.fillRect (972, 385, 10, 265); g.fillRect (986, 385, 10, 265);
    float mL = juce::jlimit(0.0f, 1.0f, audioProcessor.outputLevelL.load()*6.0f);
    float mR = juce::jlimit(0.0f, 1.0f, audioProcessor.outputLevelR.load()*6.0f);
    juce::ColourGradient grad (juce::Colours::red, 0, 385, juce::Colours::green, 0, 650, false); grad.addColour (0.3, juce::Colours::yellow);
    g.setGradientFill (grad);
    if (mL > 0) g.fillRect (973, 650 - (int)(265.0f*mL), 8, (int)(265.0f*mL));
    if (mR > 0) g.fillRect (987, 650 - (int)(265.0f*mR), 8, (int)(265.0f*mR));

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

    // Módulos maestros reorganizados con más espacio para etiquetas
    masterHpfSlider.setBounds (sz(445, 30, 22, 28)); masterHpfResSlider.setBounds (sz(485, 30, 22, 28));
    masterLpfSlider.setBounds (sz(540, 30, 22, 28)); masterLpfResSlider.setBounds (sz(580, 30, 22, 28));
    pcmBitsSlider.setBounds (sz(635, 30, 20, 28)); pcmRateSlider.setBounds (sz(668, 30, 20, 28));
    flangerRateSlider.setBounds (sz(730, 30, 20, 28)); flangerFbSlider.setBounds (sz(768, 30, 20, 28));
    flangerOnButton.setBounds (sz(730, 62, 58, 20));
    springDecaySlider.setBounds (sz(830, 30, 22, 28)); springToneSlider.setBounds (sz(870, 30, 22, 28));

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
