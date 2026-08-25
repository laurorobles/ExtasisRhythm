import re

with open('src/ChannelStripComponent.cpp', 'r') as f:
    cpp = f.read()

# Replace setupKnob definition to accept juce::Colour
old_setup = r'auto setupKnob = \[this, knobLaf\]\(juce::Slider& sl, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& att, const juce::String& paramId\) \{.*?att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> \(audioProcessor\.apvts, paramId, sl\);\s*\};'
new_setup = '''auto setupKnob = [this, knobLaf](juce::Slider& sl, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& att, const juce::String& paramId, juce::Colour col) {
        addAndMakeVisible (sl);
        sl.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        sl.setColour(juce::Slider::rotarySliderFillColourId, col);
        if (knobLaf) sl.setLookAndFeel(knobLaf);
        att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, paramId, sl);
    };'''
cpp = re.sub(old_setup, new_setup, cpp, flags=re.DOTALL)

# Replace the setupKnob calls with colors
old_calls = r'setupKnob \(volSlider, volAtt, "gain" \+ chStr\);.*?setupKnob \(delaySendSlider, delaySendAtt, "delaySend" \+ chStr\);'
new_calls = '''setupKnob (volSlider, volAtt, "gain" + chStr, juce::Colour(0xff3498db));
    setupKnob (panSlider, panAtt, "pan" + chStr, juce::Colour(0xff3498db));
    setupKnob (pitchSlider, pitchAtt, "pitch" + chStr, juce::Colour(0xff3498db));
    setupKnob (toneSlider, toneAtt, "tone" + chStr, juce::Colour(0xff3498db));
    setupKnob (attackSlider, attAtt, "attack" + chStr, juce::Colour(0xff3498db));
    setupKnob (decaySlider, decAtt, "decay" + chStr, juce::Colour(0xff3498db));
    setupKnob (springSendSlider, springSendAtt, "springSend" + chStr, juce::Colour(0xff556b2f));
    setupKnob (delaySendSlider, delaySendAtt, "delaySend" + chStr, juce::Colour(0xff00d2ff));'''
cpp = re.sub(old_calls, new_calls, cpp, flags=re.DOTALL)

# Fix resized() coords
old_resized = r'void ChannelStripComponent::resized\(\)\s*\{.*?\}'
new_resized = '''void ChannelStripComponent::resized()
{
    ledButton->setBounds (5, 17, 82, 28);
    
    muteButton.setBounds (6, 48, 18, 16);
    soloButton.setBounds (27, 48, 18, 16);
    envChannelButton.setBounds (48, 48, 38, 16);
    
    sampleSourceSelector.setBounds (6, 67, 80, 17);
    sampleVariantSelector.setBounds (6, 86, 80, 17);
    
    int kSize = 38; // or whatever was the original kSize (likely 38 or 36)
    
    volSlider.setBounds (7, 103, kSize, kSize);
    panSlider.setBounds (51, 103, kSize, kSize);
    
    pitchSlider.setBounds (7, 144, kSize, kSize);
    toneSlider.setBounds (51, 144, kSize, kSize);
    
    attackSlider.setBounds (7, 185, kSize, kSize);
    decaySlider.setBounds (51, 185, kSize, kSize);
    
    springSendSlider.setBounds (7, 226, kSize, kSize);
    delaySendSlider.setBounds (51, 226, kSize, kSize);
}'''
cpp = re.sub(old_resized, new_resized, cpp, flags=re.DOTALL)

with open('src/ChannelStripComponent.cpp', 'w') as f:
    f.write(cpp)
