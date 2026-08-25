import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# 1. Update paint() text
old_paint_branding = r'// --- BRANDING AREA ---.*?g\.drawText \("DSP & UI by @laurorobles", 152, 85, 200, 14, juce::Justification::left\);'

new_paint_branding = '''// --- BRANDING AREA (270px width) ---
    if (logoImage.isValid())
    {
        auto logoRect = juce::Rectangle<float> (100.0f, 15.0f, 70.0f, 70.0f);
        auto placement = juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize;

        g.setColour (juce::Colours::black.withAlpha (0.45f));
        g.drawImage (logoImage, logoRect.translated (2.0f, 2.0f), placement, true);
        g.setColour (juce::Colours::white);
        g.drawImage (logoImage, logoRect, placement, true);
        g.drawImage (logoImage, logoRect, placement, false);
    }

    // Title
    g.setFont (juce::FontOptions (26.0f, juce::Font::bold)); 
    g.setColour (juce::Colours::black.withAlpha(0.2f)); 
    g.drawText ("EXTASIS RHYTHM", 12, 92, 270, 30, juce::Justification::centred); 
    g.setColour (juce::Colour (0xff00d2ff)); 
    g.drawText ("EXTASIS RHYTHM", 10, 90, 270, 30, juce::Justification::centred);

    // Version
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold)); 
    g.setColour (juce::Colour (0xff666666)); 
    g.drawText ("v3.0", 10, 120, 270, 14, juce::Justification::centred);
    
    // Credits
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold)); 
    g.setColour (juce::Colour (0xff444444)); 
    g.drawText ("DSP & UI by @laurorobles", 10, 135, 270, 14, juce::Justification::centred);'''

cpp = re.sub(old_paint_branding, new_paint_branding, cpp, flags=re.DOTALL)

# 2. Update resized() for bandcampButton
old_resized = r'bandcampButton\.setBounds \(sz\(150, 110, 200, 24\)\);'
new_resized = 'bandcampButton.setBounds (sz(10, 153, 270, 18));'
cpp = re.sub(old_resized, new_resized, cpp)

# 3. Fix justification of bandcamp button to be centered
cpp = re.sub(r'bandcampButton\.setJustificationType \(juce::Justification::left\);', 'bandcampButton.setJustificationType (juce::Justification::centred);', cpp)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
