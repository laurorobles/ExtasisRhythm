import os
import re

src_dir = "/Users/babyonk1/Desktop/ExtasisRecords/ExtasisRhythm/src"
fx_h_path = os.path.join(src_dir, "FXRackComponent.h")
fx_cpp_path = os.path.join(src_dir, "FXRackComponent.cpp")
editor_h = os.path.join(src_dir, "PluginEditor.h")
editor_cpp = os.path.join(src_dir, "PluginEditor.cpp")

# --- Generate FXRackComponent.h ---
fx_h_content = """#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UIComponents.h"

class FXRackComponent : public juce::Component {
public:
    FXRackComponent(ExtasisRhythmProcessor& p, juce::LookAndFeel_V4* knobLaf);
    ~FXRackComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    ExtasisRhythmProcessor& audioProcessor;
    juce::LookAndFeel_V4* knobLaf;

    juce::TextButton flangerOnButton, chorusOnButton, delaySyncButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> flangerOnAtt, chorusOnAtt, delaySyncAtt;

    juce::Slider flangerRateSlider, flangerFbSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flRateAtt, flFbAtt;

    juce::Slider chorusRateSlider, chorusDepthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusRateAtt, chorusDepthAtt;

    juce::Slider delayTimeSlider, delayFbSlider, delayModRateSlider, delayModDepthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delTimeAtt, delFbAtt, delModRateAtt, delModDepthAtt;

    juce::Slider springDecaySlider, springToneSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> springDecAtt, springToneAtt;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FXRackComponent)
};
"""
with open(fx_h_path, "w") as f: f.write(fx_h_content)

# --- Generate FXRackComponent.cpp ---
fx_cpp_content = """#include "FXRackComponent.h"

FXRackComponent::FXRackComponent(ExtasisRhythmProcessor& p, juce::LookAndFeel_V4* laf) 
    : audioProcessor(p), knobLaf(laf)
{
    auto setupFxBtn = [this](juce::TextButton& btn, const juce::String& text, juce::Colour onCol) {
        addAndMakeVisible (btn);
        btn.setButtonText (text);
        btn.setClickingTogglesState (true);
        btn.setColour (juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
        btn.setColour (juce::TextButton::buttonOnColourId, onCol);
        btn.setColour (juce::TextButton::textColourOffId, juce::Colours::grey);
        btn.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    };

    auto mkEffectKnob = [this] (juce::Slider& sl, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& at, const juce::String& id, juce::Colour col) {
        addAndMakeVisible(sl);
        sl.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag); 
        sl.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0); 
        if (knobLaf) sl.setLookAndFeel (knobLaf); 
        sl.setPopupDisplayEnabled (true, true, this); 
        sl.setNumDecimalPlacesToDisplay (2);
        sl.setColour(juce::Slider::rotarySliderFillColourId, col);
        at = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, id, sl);
    };

    setupFxBtn(flangerOnButton, "OFF", juce::Colour (0xff8e44ad)); 
    flangerOnButton.onClick = [this] { flangerOnButton.setButtonText (flangerOnButton.getToggleState() ? "ON" : "OFF"); }; 
    flangerOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "flangerOn", flangerOnButton);

    setupFxBtn(chorusOnButton, "OFF", juce::Colour (0xff2ecc71)); 
    chorusOnButton.onClick = [this] { chorusOnButton.setButtonText (chorusOnButton.getToggleState() ? "ON" : "OFF"); }; 
    chorusOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "chorusOn", chorusOnButton);

    setupFxBtn(delaySyncButton, "SYNC", juce::Colour (0xff00d2ff)); 
    delaySyncAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "delaySync", delaySyncButton);

    mkEffectKnob (flangerRateSlider, flRateAtt, "flangerRate", juce::Colour (0xff8e44ad)); 
    mkEffectKnob (flangerFbSlider, flFbAtt, "flangerFeedback", juce::Colour (0xff8e44ad)); 

    mkEffectKnob (springDecaySlider, springDecAtt, "springDecay", juce::Colour (0xff556b2f)); 
    mkEffectKnob (springToneSlider, springToneAtt, "springTone", juce::Colour (0xff556b2f)); 

    mkEffectKnob (delayTimeSlider, delTimeAtt, "delayTime", juce::Colour (0xff00d2ff)); 
    mkEffectKnob (delayFbSlider, delFbAtt, "delayFb", juce::Colour (0xff00d2ff)); 
    mkEffectKnob (delayModRateSlider, delModRateAtt, "delayModRate", juce::Colour (0xff00d2ff)); 
    mkEffectKnob (delayModDepthSlider, delModDepthAtt, "delayModDepth", juce::Colour (0xff00d2ff)); 

    mkEffectKnob (chorusRateSlider, chorusRateAtt, "chorusRate", juce::Colour (0xff2ecc71)); 
    mkEffectKnob (chorusDepthSlider, chorusDepthAtt, "chorusDepth", juce::Colour (0xff2ecc71));
}

FXRackComponent::~FXRackComponent() {
    flangerRateSlider.setLookAndFeel(nullptr);
    flangerFbSlider.setLookAndFeel(nullptr);
    chorusRateSlider.setLookAndFeel(nullptr);
    chorusDepthSlider.setLookAndFeel(nullptr);
    delayTimeSlider.setLookAndFeel(nullptr);
    delayFbSlider.setLookAndFeel(nullptr);
    delayModRateSlider.setLookAndFeel(nullptr);
    delayModDepthSlider.setLookAndFeel(nullptr);
    springDecaySlider.setLookAndFeel(nullptr);
    springToneSlider.setLookAndFeel(nullptr);
}

void FXRackComponent::paint(juce::Graphics& g) {
    float s = (float)getWidth() / 390.0f;
    g.saveState();
    g.addTransform (juce::AffineTransform::scale (s));

    auto drawModuleBox = [&g] (int x, int y, int w, int h, const juce::String& title, juce::Colour boxTint, bool showLed = false, bool ledState = false) {
        g.setColour (boxTint); 
        g.fillRoundedRectangle ((float)x, (float)y, (float)w, (float)h, 4.0f);
        g.setColour (juce::Colours::black.withAlpha(0.2f)); 
        g.drawRoundedRectangle ((float)x, (float)y, (float)w, (float)h, 4.0f, 1.0f);
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold)); 
        g.setColour (juce::Colour (0xff111111)); 
        g.drawText (title, x, y + 3, w, 14, juce::Justification::centred);
        if (showLed) { 
            g.setColour (ledState ? juce::Colour (0xffcc0000) : juce::Colour (0xff555555)); 
            g.fillEllipse ((float)(x + 7), (float)(y + 7), 5.5f, 5.5f); 
        }
    };
    auto drawModLabels = [&g] (int mx, int my, int mw, int mh, std::vector<juce::String> texts, int customKnobSize, bool hasTopButton = false, int rowGap = 12) {
        g.setFont (juce::FontOptions (8.5f, juce::Font::bold)); 
        g.setColour (juce::Colour (0xff222222));
        int numKnobs = (int)texts.size();
        int cols = juce::jmin(numKnobs, 2);
        int rows = (numKnobs + cols - 1) / cols;
        int titleOffset = hasTopButton ? 36 : 18;
        int totalContentHeight = rows * (customKnobSize + rowGap) - 2; 
        int availableHeight = mh - titleOffset;
        int topMargin = titleOffset + (availableHeight - totalContentHeight) / 2;

        for (int k = 0; k < numKnobs; ++k) {
            int r = k / cols; int c = k % cols;
            float relX = (cols == 1) ? 0.5f : (c == 0 ? 0.33f : 0.67f);
            if (numKnobs == 3 && k == 2) relX = 0.5f;
            int cx = mx + (int)(mw * relX);
            int cy = my + topMargin + (r * (customKnobSize + rowGap));
            int ly = cy + customKnobSize - 3;
            g.drawText(texts[k], cx - 20, ly, 40, 10, juce::Justification::centred);
        }
    };

    int fxW = 96; int gap = 2;
    drawModuleBox (0, 0, fxW, 88, "FLANGER", juce::Colour (0xffd8d8d8), true, flangerOnButton.getToggleState()); 
    drawModLabels (0, 0, fxW, 88, {"RATE", "FB"}, 28, true, 12);
    
    drawModuleBox (fxW+gap, 0, fxW, 88, "CE CHORUS", juce::Colour (0xffd8d8d8), true, chorusOnButton.getToggleState()); 
    drawModLabels (fxW+gap, 0, fxW, 88, {"RATE", "DEPTH"}, 28, true, 12);
    
    drawModuleBox ((fxW+gap)*2, 0, fxW, 88, "DELAY", juce::Colour (0xffd8d8d8), true, false); 
    drawModLabels ((fxW+gap)*2, 0, fxW, 88, {"TIME", "FB", "MOD"}, 26, false, 7); 
    
    drawModuleBox ((fxW+gap)*3, 0, fxW, 88, "SPRING", juce::Colour (0xffd8d8d8));      
    drawModLabels ((fxW+gap)*3, 0, fxW, 88, {"DEC", "TONE"}, 28, false, 12);

    g.restoreState();
}

void FXRackComponent::resized() {
    float s = (float)getWidth() / 390.0f;
    auto sz = [s](int x, int y, int w, int h) { 
        return juce::Rectangle<int> ((int)(x*s), (int)(y*s), (int)(w*s), (int)(h*s)); 
    };

    auto getModuleLayout = [](int mh, int numKnobs, int customKnobSize, bool hasTopButton = false, int rowGap = 12) {
        int cols = juce::jmin(numKnobs, 2);
        int rows = (numKnobs + cols - 1) / cols;
        int totalContentHeight = rows * (customKnobSize + rowGap) - 2; 
        int titleOffset = hasTopButton ? 36 : 18; 
        int availableHeight = mh - titleOffset;
        int topMargin = titleOffset + (availableHeight - totalContentHeight) / 2;
        return std::make_pair(customKnobSize, topMargin);
    };

    auto layoutModuleSmart = [&](int mx, int my, int mw, int mh, std::vector<juce::Slider*> knobs, juce::Button* btn = nullptr, bool useTopRightButton = false, int customKnobSize = 26, int rowGap = 12) {
        int numKnobs = (int)knobs.size();
        if (numKnobs == 0) return;
        bool hasTopButton = (btn != nullptr && !useTopRightButton);
        auto layout = getModuleLayout(mh, numKnobs, customKnobSize, hasTopButton, rowGap);
        int knobSize = layout.first;
        int topMargin = layout.second;
        int cols = juce::jmin(numKnobs, 2);

        for (int k = 0; k < numKnobs; ++k) {
            int r = k / cols; 
            int c = k % cols;
            float relX = (cols == 1) ? 0.5f : (c == 0 ? 0.33f : 0.67f);
            if (numKnobs == 3 && k == 2) relX = 0.5f; 
            int cx = mx + (int)(mw * relX);
            int cy = my + topMargin + (r * (knobSize + rowGap)); 
            knobs[k]->setBounds(sz(cx - (knobSize / 2), cy, knobSize, knobSize));
        }

        if (btn != nullptr) {
            int btnW = 34, btnH = 14;
            if (useTopRightButton) {
                btn->setBounds(sz(mx + mw - btnW - 6, my + 5, btnW, btnH)); 
            } else {
                btn->setBounds(sz(mx + (mw - btnW) / 2, my + 20, btnW, btnH)); 
            }
        }
    };

    int fxW = 96; int gap = 2;
    layoutModuleSmart (0, 0, fxW, 88, { &flangerRateSlider, &flangerFbSlider }, &flangerOnButton, false, 31, 12);
    layoutModuleSmart (fxW+gap, 0, fxW, 88, { &chorusRateSlider, &chorusDepthSlider }, &chorusOnButton, false, 31, 12);
    layoutModuleSmart ((fxW+gap)*2, 0, fxW, 88, { &delayTimeSlider, &delayFbSlider, &delayModRateSlider }, &delaySyncButton, true, 29, 7); 
    layoutModuleSmart ((fxW+gap)*3, 0, fxW, 88, { &springDecaySlider, &springToneSlider }, nullptr, false, 31, 12);
}
"""
with open(fx_cpp_path, "w") as f: f.write(fx_cpp_content)

# --- Update PluginEditor.h ---
with open(editor_h, "r") as f: h_code = f.read()
h_code = h_code.replace('#include "ChannelStripComponent.h"', '#include "ChannelStripComponent.h"\n#include "FXRackComponent.h"')

# Remove Flanger
h_code = re.sub(r'juce::TextButton flangerOnButton, chorusOnButton, delaySyncButton;\n', '', h_code)
h_code = re.sub(r'std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> flangerOnAtt, chorusOnAtt, delaySyncAtt;\n', '', h_code)

h_code = re.sub(r'juce::Slider flangerRateSlider, flangerFbSlider;\n', '', h_code)
h_code = re.sub(r'std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flRateAtt, flFbAtt;\n', '', h_code)

h_code = re.sub(r'juce::Slider springDecaySlider, springToneSlider;\n', '', h_code)
h_code = re.sub(r'std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> springDecAtt, springToneAtt;\n', '', h_code)

h_code = re.sub(r'juce::Slider delayTimeSlider, delayFbSlider, delayModRateSlider, delayModDepthSlider;\n', '', h_code)
h_code = re.sub(r'std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delTimeAtt, delFbAtt, delModRateAtt, delModDepthAtt;\n', '', h_code)

h_code = re.sub(r'juce::Slider chorusRateSlider, chorusDepthSlider;\n', '', h_code)
h_code = re.sub(r'std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusRateAtt, chorusDepthAtt;\n', '', h_code)

# Insert fxRack right after audioProcessor
h_code = h_code.replace('ExtasisRhythmProcessor& audioProcessor;', 'ExtasisRhythmProcessor& audioProcessor;\n    std::unique_ptr<FXRackComponent> fxRack;')
with open(editor_h, "w") as f: f.write(h_code)

# --- Update PluginEditor.cpp ---
with open(editor_cpp, "r") as f: c_code = f.read()

# In constructor: initialize fxRack and add it
init_fx = """    logoImage = juce::ImageFileFormat::loadFrom (BinaryData::logo_png, (size_t) BinaryData::logo_pngSize);

    fxRack = std::make_unique<FXRackComponent>(audioProcessor, &effectKnobLAF);
    addAndMakeVisible(fxRack.get());"""
c_code = c_code.replace('    logoImage = juce::ImageFileFormat::loadFrom (BinaryData::logo_png, (size_t) BinaryData::logo_pngSize);', init_fx)

# Remove old constructor lines
c_code = re.sub(r'\n\s*setupFxBtn\(flangerOnButton, "OFF", juce::Colour \(0xff8e44ad\)\);.*?\n.*?flangerOnAtt = .*?\n', '', c_code, flags=re.DOTALL)
c_code = re.sub(r'\n\s*setupFxBtn\(chorusOnButton, "OFF", juce::Colour \(0xff2ecc71\)\);.*?\n.*?chorusOnAtt = .*?\n', '', c_code, flags=re.DOTALL)
c_code = re.sub(r'\n\s*setupFxBtn\(delaySyncButton, "SYNC", juce::Colour \(0xff00d2ff\)\);.*?\n.*?delaySyncAtt = .*?\n', '', c_code, flags=re.DOTALL)

# Remove the mkEffectKnob calls for these
c_code = re.sub(r'\n\s*mkEffectKnob \(flangerRateSlider.*?\n.*?flangerFbSlider.*?\n', '\n', c_code)
c_code = re.sub(r'\n\s*mkEffectKnob \(springDecaySlider.*?\n.*?springToneSlider.*?\n', '\n', c_code)
c_code = re.sub(r'\n\s*mkEffectKnob \(delayTimeSlider.*?\n.*?delayFbSlider.*?\n.*?delayModRateSlider.*?\n.*?delayModDepthSlider.*?\n', '\n', c_code)
c_code = re.sub(r'\n\s*mkEffectKnob \(chorusRateSlider.*?\n.*?chorusDepthSlider.*?\n', '\n', c_code)

# Remove drawing in paint
c_code = re.sub(r'\n\s*drawModuleBox \(fxStartX \+ fxW\+gap,\s*90,\s*fxW,\s*88,\s*"FLANGER".*?RATE", "FB".*?12\);\n', '', c_code, flags=re.DOTALL)
c_code = re.sub(r'\n\s*drawModuleBox \(fxStartX \+ \(fxW\+gap\)\*2,\s*90,\s*fxW,\s*88,\s*"CE CHORUS".*?RATE", "DEPTH".*?12\);\n', '', c_code, flags=re.DOTALL)
c_code = re.sub(r'\n\s*drawModuleBox \(fxStartX \+ \(fxW\+gap\)\*3,\s*90,\s*fxW,\s*88,\s*"DELAY".*?TIME", "FB", "MOD".*?7\);\n', '', c_code, flags=re.DOTALL)
c_code = re.sub(r'\n\s*drawModuleBox \(fxStartX \+ \(fxW\+gap\)\*4,\s*90,\s*fxW,\s*88,\s*"SPRING".*?DEC", "TONE".*?12\);\n', '', c_code, flags=re.DOTALL)

# Remove layout in resized and add fxRack.setBounds
resized_replace = """    layoutModuleSmart (fxStartX,         90, fxW, 88, { &pumpThrSlider, &pumpAmtSlider }, nullptr, false, 31, 12);
    fxRack->setBounds(sz(fxStartX + fxW + gapFx, 90, 4 * fxW + 3 * gapFx, 88));"""
c_code = re.sub(r'\n\s*layoutModuleSmart \(fxStartX,.*?90,\s*fxW,\s*88,\s*\{\s*&pumpThrSlider.*?12\);\n\s*layoutModuleSmart \(fxStartX \+ fxW\+gapFx,.*?12\);\n\s*layoutModuleSmart \(fxStartX \+ \(fxW\+gapFx\)\*2,.*?12\);\n\s*layoutModuleSmart \(fxStartX \+ \(fxW\+gapFx\)\*3,.*?7\);\n\s*layoutModuleSmart \(fxStartX \+ \(fxW\+gapFx\)\*4,.*?12\);', '\n' + resized_replace, c_code, flags=re.DOTALL)

with open(editor_cpp, "w") as f: f.write(c_code)

