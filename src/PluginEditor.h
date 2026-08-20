#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

class SimpleKnobLookAndFeel : public juce::LookAndFeel_V4 {
public:
    SimpleKnobLookAndFeel() {
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff3498db));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xffb0b0b0));
        setColour (juce::Slider::thumbColourId, juce::Colour (0xff222222));
    }
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override {
        auto bounds = juce::Rectangle<float> ((float)x, (float)y, (float)width, (float)height).reduced (3.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto center = bounds.getCentre();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        g.setColour (juce::Colour (0xffcccccc));
        g.fillEllipse (center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
        g.setColour (juce::Colour (0xffb0b0b0));
        g.drawEllipse (center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f, 1.0f);

        juce::Path p;
        p.addRectangle (-1.5f, -radius + 3.0f, 3.0f, 8.0f);
        
        juce::Colour fillColour = slider.findColour(juce::Slider::rotarySliderFillColourId);
        g.setColour(fillColour);
        g.fillEllipse(center.x - (radius * 0.4f), center.y - (radius * 0.4f), radius * 0.8f, radius * 0.8f);

        g.setColour (juce::Colour (0xff222222));
        g.fillPath (p, juce::AffineTransform::rotation (angle).translated (center));
    }
};

static SimpleKnobLookAndFeel simpleKnobLAF;

class ExtasisRhythmEditor : public juce::AudioProcessorEditor,
                            private juce::Timer {
public:
    ExtasisRhythmEditor (ExtasisRhythmProcessor&);
    ~ExtasisRhythmEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent&) override;
    bool keyPressed (const juce::KeyPress& key) override;

private:
    void timerCallback() override;
    void updateStepButtonVisuals (int ch, int step);
    void updateFillButtonVisuals (int step);
    void updateLengthLabel (int i);

    ExtasisRhythmProcessor& audioProcessor;

    juce::TextButton playButton, stopButton, resetButton, seqResetButton, saveKitButton, loadKitButton, killAudioButton;
    juce::Slider bpmSlider;
    juce::ComboBox globalKitSelector;
    juce::TextButton patternButtons[8];
    int activePatternButton = 0;

    juce::Slider masterHpfSlider, masterHpfResSlider, masterLpfSlider, masterLpfResSlider;
    juce::Slider pcmBitsSlider, pcmRateSlider;
    juce::Slider flangerRateSlider, flangerFbSlider;
    juce::TextButton flangerOnButton;
    juce::Slider transAttackSlider, transSustainSlider;
    juce::Slider driveDistSlider, driveFilterSlider, driveVolSlider;
    juce::Slider springDecaySlider, springToneSlider;
    juce::Slider delayTimeSlider, delayFbSlider, delayModRateSlider, delayModDepthSlider;
    juce::Slider masterVolSlider, masterClipperSlider;

    juce::ComboBox sampleSourceSelectors[12];
    juce::ComboBox sampleVariantSelectors[12];
    juce::TextButton muteButtons[12];
    juce::TextButton soloButtons[12];
    juce::TextButton tripletButtons[12];
    juce::Slider volumeSliders[12], panSliders[12], pitchSliders[12], toneSliders[12];
    juce::Slider attackSliders[12], decaySliders[12], springSendSliders[12], delaySendSliders[12];

    juce::TextButton minusButtons[12], plusButtons[12];
    juce::Label lengthLabels[12];
    juce::Slider hiddenLengthSliders[12];

    juce::TextButton stepButtons[12][16];
    juce::TextButton fillStepButtons[16];
    juce::TextButton tripletFillButton;
    std::unique_ptr<juce::ResizableCornerComponent> cornerResizer;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bpmAtt, masterVolAtt, masterClipAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> globalKitAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpfAtt, hpfResAtt, lpfAtt, lpfResAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pcmBitsAtt, pcmRateAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flRateAtt, flFbAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> flangerOnAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> transAttAtt, transSusAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveDistAtt, driveFilterAtt, driveVolAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> springDecAtt, springToneAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delTimeAtt, delFbAtt, delModRateAtt, delModDepthAtt;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sampleSourceAtts[12];
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAtts[12], soloAtts[12], tripletAtts[12], tripletFillAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAtts[12], panAtts[12], pitchAtts[12], toneAtts[12];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attAtts[12], decAtts[12], springSendAtts[12], delaySendAtts[12], stepLengthAtts[12];

    std::unique_ptr<juce::FileChooser> saveChooser, loadChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtasisRhythmEditor)
};