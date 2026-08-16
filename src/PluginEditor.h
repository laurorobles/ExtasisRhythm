#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class TR909LookAndFeel : public juce::LookAndFeel_V4 {
public:
    TR909LookAndFeel() {
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffff6600));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xffb0b0b0));
        setColour (juce::Slider::thumbColourId, juce::Colour (0xff333333));
        setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xffe6e6e6));
        setColour (juce::ComboBox::outlineColourId, juce::Colour (0xffaaaaaa));
        setColour (juce::ComboBox::textColourId, juce::Colour (0xff222222));
    }
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float startAngle, float endAngle, juce::Slider& slider) override {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 1.5f;
        auto cx = (float) x + (float) width * 0.5f; auto cy = (float) y + (float) height * 0.5f;
        g.setColour (juce::Colour (0xffcccccc)); g.fillEllipse (cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
        g.setColour (juce::Colour (0xff888888)); g.drawEllipse (cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.2f);
        juce::Path p; p.addRectangle (-1.0f, -radius + 2.0f, 2.0f, radius * 0.7f);
        p.applyTransform (juce::AffineTransform::rotation (startAngle + sliderPos * (endAngle - startAngle)).translated (cx, cy));
        g.setColour (juce::Colour (0xff333333)); g.fillPath (p);
    }
};

class ExtasisRhythmEditor : public juce::AudioProcessorEditor, public juce::Timer {
public:
    ExtasisRhythmEditor (ExtasisRhythmProcessor&);
    ~ExtasisRhythmEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    bool keyPressed (const juce::KeyPress& key) override {
        if (key.getKeyCode() == juce::KeyPress::spaceKey) {
            auto* p = audioProcessor.apvts.getParameter ("isPlaying");
            if (p) { p->beginChangeGesture(); p->setValueNotifyingHost (p->getValue() > 0.5f ? 0.0f : 1.0f); p->endChangeGesture(); }
            return true;
        }
        return false;
    }
    void mouseDown (const juce::MouseEvent&) override;

private:
    ExtasisRhythmProcessor& audioProcessor;
    TR909LookAndFeel tr909Feel;
    juce::TextButton playButton, stopButton, saveKitButton { "Save" }, loadKitButton { "Load" }, resetButton { "Reset" };
    juce::Slider bpmSlider, masterVolSlider;
    juce::TextButton clipperButton, flangerOnButton;
    std::unique_ptr<juce::FileChooser> saveChooser, loadChooser;
    juce::ComboBox globalKitSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> globalKitAtt;
    juce::Slider masterHpfSlider, masterHpfResSlider, masterLpfSlider, masterLpfResSlider;
    juce::Slider pcmBitsSlider, pcmRateSlider, flangerRateSlider, flangerFbSlider;
    juce::Slider springDecaySlider, springToneSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> playAtt, clipperAtt, flangerOnAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bpmAtt, masterVolAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpfAtt, hpfResAtt, lpfAtt, lpfResAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pcmBitsAtt, pcmRateAtt, flRateAtt, flFbAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> springDecAtt, springToneAtt;
    juce::ComboBox sampleSourceSelectors[10];
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sampleSourceAtts[10];
    juce::Slider volumeSliders[10], panSliders[10], pitchSliders[10], toneSliders[10], attackSliders[10], decaySliders[10], sendSliders[10], stepLengthSliders[10];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAtts[10], panAtts[10], pitchAtts[10], toneAtts[10], attAtts[10], decAtts[10], sendAtts[10], stepLengthAtts[10];
    juce::TextButton stepButtons[10][16], fillStepButtons[16];
    std::unique_ptr<juce::ResizableCornerComponent> myResizableCorner;
    void updateStepButtonVisuals (int, int);
    void updateFillButtonVisuals (int);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtasisRhythmEditor)
};
