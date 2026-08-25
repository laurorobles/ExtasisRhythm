import re

with open('src/ChannelStripComponent.cpp', 'r') as f:
    cpp = f.read()

# Fix paint()
old_paint = r'void ChannelStripComponent::paint \(juce::Graphics& g\)\s*\{.*?g\.drawText \("DELAY", 48, 262, 40, 9, juce::Justification::centred\);\s*\}'
new_paint = '''void ChannelStripComponent::paint (juce::Graphics& g)
{
    float s = (float)getWidth() / 92.0f;
    g.saveState();
    g.addTransform (juce::AffineTransform::scale (s));

    g.setColour (juce::Colours::black.withAlpha (0.15f)); 
    g.fillRoundedRectangle (3.0f, 19.0f, 92.0f, 258.0f, 4.0f);
    juce::ColourGradient cg (juce::Colour (0xfffcfcfc), 0.0f, 16.0f, juce::Colour (0xffe0e0e0), 0.0f, 272.0f, false);
    g.setGradientFill (cg); 
    g.fillRoundedRectangle (0.0f, 16.0f, 92.0f, 258.0f, 4.0f);
    
    g.setColour (juce::Colour (0xffb8b8b8)); 
    g.drawRoundedRectangle (0.0f, 16.0f, 92.0f, 258.0f, 4.0f, 1.0f);
    
    g.setColour (juce::Colours::white.withAlpha(0.9f)); 
    g.drawHorizontalLine (17, 2.0f, 90.0f);

    g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); 
    g.setColour (juce::Colours::black.withAlpha(0.4f)); 
    g.drawText (channelLabel, 5, 34, 82, 11, juce::Justification::centred);
    g.setColour (juce::Colour (0xff222222)); 
    g.drawText (channelLabel, 4, 33, 82, 11, juce::Justification::centred);

    g.setFont (juce::FontOptions (7.5f, juce::Font::bold)); 
    g.setColour (juce::Colour (0xff444444));
    g.drawText ("VOL",   4,  139, 40, 9, juce::Justification::centred);
    g.drawText ("PAN",   48, 139, 40, 9, juce::Justification::centred);
    g.drawText ("PITCH", 4,  180, 40, 9, juce::Justification::centred);
    g.drawText ("TONE",  48, 180, 40, 9, juce::Justification::centred);
    g.drawText ("ATT",   4,  221, 40, 9, juce::Justification::centred);
    g.drawText ("DEC",   48, 221, 40, 9, juce::Justification::centred);
    g.drawText ("SPRING",4,  262, 40, 9, juce::Justification::centred);
    g.drawText ("DELAY", 48, 262, 40, 9, juce::Justification::centred);
    
    g.restoreState();
}'''
cpp = re.sub(old_paint, new_paint, cpp, flags=re.DOTALL)


# Fix resized()
old_resized = r'void ChannelStripComponent::resized\(\)\s*\{.*?\}'
new_resized = '''void ChannelStripComponent::resized()
{
    float s = (float)getWidth() / 92.0f;
    auto sz = [s](int x, int y, int w, int h) { 
        return juce::Rectangle<int> ((int)(x*s), (int)(y*s), (int)(w*s), (int)(h*s)); 
    };

    ledButton->setBounds (sz(5, 17, 82, 28));
    
    muteButton.setBounds (sz(6, 48, 18, 16));
    soloButton.setBounds (sz(27, 48, 18, 16));
    envChannelButton.setBounds (sz(48, 48, 38, 16));
    
    sampleSourceSelector.setBounds (sz(6, 67, 80, 17));
    sampleVariantSelector.setBounds (sz(6, 86, 80, 17));
    
    int kSize = 34; // Set to original 34 to match text spacing
    
    volSlider.setBounds (sz(7, 103, kSize, kSize));
    panSlider.setBounds (sz(51, 103, kSize, kSize));
    
    pitchSlider.setBounds (sz(7, 144, kSize, kSize));
    toneSlider.setBounds (sz(51, 144, kSize, kSize));
    
    attackSlider.setBounds (sz(7, 185, kSize, kSize));
    decaySlider.setBounds (sz(51, 185, kSize, kSize));
    
    springSendSlider.setBounds (sz(7, 226, kSize, kSize));
    delaySendSlider.setBounds (sz(51, 226, kSize, kSize));
}'''
cpp = re.sub(old_resized, new_resized, cpp, flags=re.DOTALL)

with open('src/ChannelStripComponent.cpp', 'w') as f:
    f.write(cpp)

