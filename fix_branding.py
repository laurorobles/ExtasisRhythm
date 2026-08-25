import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

# 1. Update paint() text
old_paint_branding = r'g\.setFont \(juce::FontOptions \(25\.0f, juce::Font::bold\)\).*?g\.drawImage \(logoImage, logoRect, placement, false\);\n    \}'

new_paint_branding = '''
    // --- BRANDING AREA ---
    if (logoImage.isValid())
    {
        auto logoRect = juce::Rectangle<float> (15.0f, 30.0f, 120.0f, 120.0f);
        auto placement = juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize;

        g.setColour (juce::Colours::black.withAlpha (0.45f));
        g.drawImage (logoImage, logoRect.translated (2.0f, 2.0f), placement, true);
        g.setColour (juce::Colours::white);
        g.drawImage (logoImage, logoRect, placement, true);
        g.drawImage (logoImage, logoRect, placement, false);
    }

    // Title
    g.setFont (juce::FontOptions (32.0f, juce::Font::bold)); 
    g.setColour (juce::Colours::black.withAlpha(0.2f)); 
    g.drawText ("ExtasisRhythm", 152, 47, 250, 40, juce::Justification::left); 
    g.setColour (juce::Colour (0xff00d2ff)); 
    g.drawText ("ExtasisRhythm", 150, 45, 250, 40, juce::Justification::left);

    // Version
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold)); 
    g.setColour (juce::Colour (0xff666666)); 
    g.drawText ("v3.0", 390, 50, 40, 14, juce::Justification::left);
    
    // Credits
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold)); 
    g.setColour (juce::Colour (0xff444444)); 
    g.drawText ("DSP & UI by @laurorobles", 152, 85, 200, 14, juce::Justification::left);
    '''

cpp = re.sub(old_paint_branding, new_paint_branding, cpp, flags=re.DOTALL)

# 2. Update resized() for bandcampButton
# I'll just append it to the top of resized()
old_resized_start = r'void ExtasisRhythmEditor::resized\(\)\n\{'
new_resized_start = '''void ExtasisRhythmEditor::resized()
{
    float s = (float) getWidth() / 1192.0f;
    auto sz = [s](int x, int y, int w, int h) {
        return juce::Rectangle<int>( (int)(x * s), (int)(y * s), (int)(w * s), (int)(h * s) );
    };
    
    bandcampButton.setBounds (sz(150, 110, 200, 24));'''

cpp = re.sub(old_resized_start, new_resized_start, cpp)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
