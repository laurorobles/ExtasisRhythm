#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UIComponents.h"

class ChannelStripComponent : public juce::Component
{
public:
    ChannelStripComponent (ExtasisRhythmProcessor& p, int chIndex, juce::LookAndFeel* comboLaf, juce::LookAndFeel* knobLaf);
    ~ChannelStripComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    ExtasisRhythmProcessor& audioProcessor;
    int channelIndex;

    juce::ComboBox sampleSourceSelector;
    juce::ComboBox sampleVariantSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sampleSourceAtt;
    
    std::unique_ptr<ChannelLedButton> ledButton;

    juce::TextButton muteButton, soloButton, envChannelButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAtt, soloAtt, envChanAtt;

    juce::Slider volSlider, panSlider, pitchSlider, toneSlider;
    juce::Slider attackSlider, decaySlider, springSendSlider, delaySendSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAtt, panAtt, pitchAtt, toneAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attAtt, decAtt, springSendAtt, delaySendAtt;

    juce::String channelLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelStripComponent)
};
