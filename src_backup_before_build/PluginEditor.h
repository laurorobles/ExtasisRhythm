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
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override {
        auto bounds = juce::Rectangle<float> ((float)x, (float)y, (float)width, (float)height).reduced (3.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto center = bounds.getCentre();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        g.setColour (juce::Colour (0xffcccccc));
        g.fillEllipse (center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
        g.setColour (juce::Colour (0xff888888));
        g.drawEllipse (center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f, 1.5f);

        juce::Path p;
        float pointerLength = radius * 0.65f;
        float pointerThickness = 2.0f;
        p.addRectangle (-pointerThickness / 2.0f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (center.x, center.y));
        g.setColour (juce::Colour (0xff222222));
        g.fillPath (p);
    }
};

class ExtasisRhythmEditor : public juce::AudioProcessorEditor,
                            public juce::Timer
{
public:
    ExtasisRhythmEditor (ExtasisRhythmProcessor&);
    ~ExtasisRhythmEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void mouseDown (const juce::MouseEvent&) override;

private:
    ExtasisRhythmProcessor& audioProcessor;
    SimpleKnobLookAndFeel simpleKnobLAF;
    std::unique_ptr<juce::Drawable> extasisLogo;

    juce::TextButton playButton, stopButton;
    juce::TextButton flangerOnButton;
    juce::Slider bpmSlider, masterVolSlider, masterClipperSlider;
    juce::ComboBox globalKitSelector;
    juce::TextButton saveKitButton, loadKitButton, resetButton, seqResetButton;

    std::array<juce::TextButton, 8> patternButtons;
    int activePatternButton = 0;

    juce::Slider masterHpfSlider, masterHpfResSlider;
    juce::Slider masterLpfSlider, masterLpfResSlider;
    juce::Slider pcmBitsSlider, pcmRateSlider;
    juce::Slider flangerRateSlider, flangerFbSlider;
    juce::Slider transAttackSlider, transSustainSlider;
    juce::Slider springDecaySlider, springToneSlider;

    juce::ComboBox sampleSourceSelectors[12];
    juce::ComboBox sampleVariantSelectors[12];
    juce::TextButton muteButtons[12], soloButtons[12];
    juce::Slider volumeSliders[12], panSliders[12], pitchSliders[12], toneSliders[12];
    juce::Slider attackSliders[12], decaySliders[12], sendSliders[12];
    juce::Slider hiddenLengthSliders[12];
    juce::TextButton minusButtons[12], plusButtons[12];
    juce::Label lengthLabels[12];
    juce::TextButton stepButtons[12][16];
    juce::TextButton fillStepButtons[16];

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> flangerOnAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bpmAtt, masterVolAtt, masterClipAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> globalKitAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpfAtt, hpfResAtt, lpfAtt, lpfResAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pcmBitsAtt, pcmRateAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flRateAtt, flFbAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> transAttAtt, transSusAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> springDecAtt, springToneAtt;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sampleSourceAtts[12];
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAtts[12], soloAtts[12];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAtts[12], panAtts[12], pitchAtts[12], toneAtts[12];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attAtts[12], decAtts[12], sendAtts[12], stepLengthAtts[12];

    std::unique_ptr<juce::FileChooser> saveChooser, loadChooser;
    std::unique_ptr<juce::ResizableCornerComponent> cornerResizer;

    void updateStepButtonVisuals (int ch, int step);
    void updateFillButtonVisuals (int step);
    void updateLengthLabel (int i);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtasisRhythmEditor)
};
