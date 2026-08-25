#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UIComponents.h"

class MasterSectionComponent : public juce::Component, public juce::Timer {
public:
    MasterSectionComponent(ExtasisRhythmProcessor& p, juce::LookAndFeel_V4* knobLaf, juce::LookAndFeel_V4* btnLaf);
    ~MasterSectionComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    ExtasisRhythmProcessor& audioProcessor;
    juce::LookAndFeel_V4* knobLaf;
    juce::LookAndFeel_V4* btnLaf;

    juce::Slider masterVolSlider, masterClipperSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolAtt, masterClipAtt;

    juce::TextButton bit16Button, bit12Button, bit8Button;
    juce::TextButton analogButton, vinylMasterButton, pumpButton, antiAliasButton, limiterButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> analogAtt, vinylAtt, pumpMasterAtt, antiAtt, limiterAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterSectionComponent)
};
