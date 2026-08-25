import re

with open('src/PluginEditor.h', 'r') as f:
    h = f.read()

deletions = [
    r'juce::ComboBox sampleSourceSelectors\[12\];\s*',
    r'juce::ComboBox sampleVariantSelectors\[12\];\s*',
    r'std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sampleSourceAtts\[12\];\s*',
    r'juce::TextButton muteButtons\[12\], soloButtons\[12\], envChannelButtons\[12\], fitButtons\[12\];\s*',
    r'std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAtts\[12\], soloAtts\[12\], envChanAtts\[12\], fitAtts\[12\];\s*',
    r'juce::Slider volumeSliders\[12\], panSliders\[12\], pitchSliders\[12\], toneSliders\[12\];\s*',
    r'juce::Slider attackSliders\[12\], decaySliders\[12\], springSendSliders\[12\], delaySendSliders\[12\];\s*',
    r'std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAtts\[12\], panAtts\[12\], pitchAtts\[12\], toneAtts\[12\];\s*',
    r'std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attAtts\[12\], decAtts\[12\], springSendAtts\[12\], delaySendAtts\[12\];\s*',
    r'std::vector<std::unique_ptr<ChannelLedButton>> channelLedButtons;\s*'
]
for d in deletions:
    h = re.sub(d, '', h)

h = re.sub(r'(std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stepLengthAtts\[12\];)', 
           r'std::unique_ptr<ChannelStripComponent> channelStrips[12];\n    \1', h)

with open('src/PluginEditor.h', 'w') as f:
    f.write(h)
