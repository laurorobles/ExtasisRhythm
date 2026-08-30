#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "LicenseManager.h"

class StepButton : public juce::TextButton
{
public:
    bool hasGlide = false;
    int semitoneOffset = 0; 
    juce::String customNoteText = ""; 
    std::function<void()> onRightClick;
    std::function<void(int)> onNoteChanged;

    int initialOffset = 0;
    bool isDragging = false;

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.mods.isPopupMenu())
        {
            if (onRightClick) onRightClick();
        }
        else
        {
            initialOffset = semitoneOffset;
            isDragging = false;
            juce::TextButton::mouseDown (e);
        }
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (!e.mods.isPopupMenu())
        {
            if (e.getDistanceFromDragStart() > 4)
            {
                isDragging = true;
                int deltaY = e.getMouseDownPosition().y - e.getPosition().y; 
                int semitones = deltaY / 6; 
                int newOffset = juce::jlimit (-24, 24, initialOffset + semitones);
                
                if (newOffset != semitoneOffset)
                {
                    semitoneOffset = newOffset;
                    if (onNoteChanged) onNoteChanged (semitoneOffset);
                    repaint();
                }
            }
        }
    }

    void mouseUp (const juce::MouseEvent& e) override
    {
        juce::TextButton::mouseUp (e);
        isDragging = false;
    }

    void paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::TextButton::paintButton (g, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        if (hasGlide)
        {
            g.setColour (juce::Colour (0xff00d2ff));
            g.drawLine (4.0f, (float)getHeight() - 4.0f, (float)getWidth() - 4.0f, 4.0f, 2.0f);
        }

        if (semitoneOffset != 0 && customNoteText.isNotEmpty())
        {
            g.setFont (juce::FontOptions (8.0f, juce::Font::bold));
            g.setColour (juce::Colours::black);
            g.drawText (customNoteText, 0, 0, getWidth(), getHeight(), juce::Justification::centred, false);
        }
    }
};

class ChannelLedButton : public juce::Component
{
public:
    ExtasisRhythmProcessor& processor;
    int channelIndex = 0;

    ChannelLedButton (ExtasisRhythmProcessor& p, int ch) : processor (p), channelIndex (ch) 
    {
        setMouseCursor (juce::MouseCursor::PointingHandCursor);
    }

    void mouseDown (const juce::MouseEvent&) override
    {
        processor.triggerChannel (channelIndex, 1.0f);
        processor.flashCounters[channelIndex].store (15);
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        bool isLit = processor.flashCounters[channelIndex].load() > 0;
        float ledSize = 10.0f;
        float ledX = ((float)getWidth() - ledSize) * 0.5f;
        float ledY = 2.0f;

        g.setColour (isLit ? juce::Colour (0xff00d2ff) : juce::Colour (0xff444444));
        g.fillEllipse (ledX, ledY, ledSize, ledSize);
        g.setColour (juce::Colours::black.withAlpha (0.6f));
        g.drawEllipse (ledX, ledY, ledSize, ledSize, 1.5f);
        if (isLit)
        {
            g.setColour (juce::Colours::white.withAlpha (0.95f));
            g.fillEllipse (ledX + 2.0f, ledY + 2.0f, ledSize - 4.0f, ledSize - 4.0f);
        }
    }
};

class ActivationOverlayComponent : public juce::Component
{
public:
    std::function<void(const juce::String&)> onActivate;
    std::function<void()> onContinueDemo;

    bool isExpired = false;

    juce::TextEditor licenseInput;
    juce::TextButton activateButton;
    juce::TextButton demoButton;
    juce::Label statusLabel;
    juce::HyperlinkButton gumroadLinkBtn { "BUY LICENSE", juce::URL ("http://laurorobles.gumroad.com") };

    ActivationOverlayComponent()
    {
        addAndMakeVisible (licenseInput);
        licenseInput.setMultiLine (false);
        licenseInput.setFont (juce::FontOptions (13.0f, juce::Font::bold));
        licenseInput.setJustification (juce::Justification::centred);
        licenseInput.setTextToShowWhenEmpty ("EXTR-XXXX-XXXX-XXXX-XXXX", juce::Colour(0xff718093));
        licenseInput.setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff14171a));
        licenseInput.setColour (juce::TextEditor::textColourId, juce::Colours::white);
        licenseInput.setColour (juce::TextEditor::outlineColourId, juce::Colour (0xff00d2ff));
        licenseInput.setColour (juce::TextEditor::focusedOutlineColourId, juce::Colour (0xff3498db));

        addAndMakeVisible (activateButton);
        activateButton.setButtonText ("ACTIVATE LICENSE");
        activateButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff27ae60));
        activateButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
        activateButton.onClick = [this]() {
            if (onActivate) onActivate (licenseInput.getText().trim());
        };

        addAndMakeVisible (demoButton);
        demoButton.setButtonText ("CONTINUE IN DEMO MODE");
        demoButton.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff3d3d3d));
        demoButton.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
        demoButton.onClick = [this]() {
            if (onContinueDemo) onContinueDemo();
        };

        addAndMakeVisible (statusLabel);
        statusLabel.setFont (juce::FontOptions (11.5f, juce::Font::bold));
        statusLabel.setJustificationType (juce::Justification::centred);

        addAndMakeVisible (gumroadLinkBtn);
        gumroadLinkBtn.setColour (juce::HyperlinkButton::textColourId, juce::Colour (0xff00d2ff));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xee0f141a));

        int modalW = 500;
        int modalH = 260;
        int modalX = (getWidth() - modalW) / 2;
        int modalY = (getHeight() - modalH) / 2;

        g.setColour (juce::Colours::black.withAlpha (0.7f));
        g.fillRoundedRectangle ((float)(modalX + 6), (float)(modalY + 6), (float)modalW, (float)modalH, 12.0f);

        juce::ColourGradient cardGrad (juce::Colour (0xff282c34), (float)modalX, (float)modalY,
                                       juce::Colour (0xff1c2025), (float)modalX, (float)(modalY + modalH), false);
        g.setGradientFill (cardGrad);
        g.fillRoundedRectangle ((float)modalX, (float)modalY, (float)modalW, (float)modalH, 12.0f);

        g.setColour (isExpired ? juce::Colour (0xffff5252).withAlpha (0.9f) : juce::Colour (0xff00d2ff).withAlpha (0.9f));
        g.drawRoundedRectangle ((float)modalX, (float)modalY, (float)modalW, (float)modalH, 12.0f, 1.5f);

        g.setColour (juce::Colour (0xff21252b));
        g.fillRoundedRectangle ((float)modalX + 1.0f, (float)modalY + 1.0f, (float)modalW - 2.0f, 44.0f, 12.0f);
        g.fillRect ((float)modalX + 1.0f, (float)modalY + 24.0f, (float)modalW - 2.0f, 21.0f);
        g.setColour (juce::Colour (0xff3a3f4b));
        g.drawHorizontalLine (modalY + 45, (float)modalX, (float)(modalX + modalW));

        g.setFont (juce::FontOptions (15.0f, juce::Font::bold));
        g.setColour (isExpired ? juce::Colour (0xffff5252) : juce::Colour (0xff00d2ff));
        g.drawText ("EXTASIS RHYTHM", modalX + 20, modalY + 12, 160, 22, juce::Justification::left);
        
        g.setFont (juce::FontOptions (12.0f, juce::Font::plain));
        g.setColour (juce::Colour (0xffdcdde1));
        g.drawText (isExpired ? "— Demo Expired" : "— Product Activation", modalX + 165, modalY + 13, 200, 22, juce::Justification::left);

        g.setFont (juce::FontOptions (11.5f, isExpired ? juce::Font::bold : juce::Font::plain));
        g.setColour (isExpired ? juce::Colour (0xffff6b6b) : juce::Colour (0xffc8d6e5));
        g.drawText (isExpired ? "Demo evaluation period has expired (10 minutes).\nEnter your license key to unlock and continue making music:"
                              : "Please enter your 16-character license key to unlock the full version:",
                    modalX + 20, modalY + 54, modalW - 40, 26, juce::Justification::centred);
    }

    void resized() override
    {
        int modalW = 500;
        int modalH = 260;
        int modalX = (getWidth() - modalW) / 2;
        int modalY = (getHeight() - modalH) / 2;

        licenseInput.setBounds (modalX + 45, modalY + 86, modalW - 90, 32);
        if (isExpired)
        {
            activateButton.setBounds (modalX + 120, modalY + 130, modalW - 240, 32);
            demoButton.setVisible (false);
        }
        else
        {
            activateButton.setBounds (modalX + 45, modalY + 130, 195, 32);
            demoButton.setBounds (modalX + 260, modalY + 130, 195, 32);
            demoButton.setVisible (true);
        }
        statusLabel.setBounds (modalX + 30, modalY + 172, modalW - 60, 24);
        gumroadLinkBtn.setBounds (modalX + (modalW - 140) / 2, modalY + modalH - 26, 140, 18);
    }
};

class ExtasisRhythmEditor  : public juce::AudioProcessorEditor,
                             public juce::Timer
{
public:
    ExtasisRhythmEditor (ExtasisRhythmProcessor&);
    ~ExtasisRhythmEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    bool keyPressed (const juce::KeyPress& key) override;
    void mouseDown (const juce::MouseEvent& e) override;

private:

    ExtasisRhythmProcessor& audioProcessor;

    juce::TooltipWindow tooltipWindow { this, 400 }; 

    juce::TextButton playButton, stopButton, resetButton, seqResetButton, seqToggleViewButton;
    juce::Slider bpmSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bpmAtt;

    juce::TextButton bit16Button, bit12Button, bit8Button;
    juce::TextButton analogButton, vinylMasterButton, pumpButton, antiAliasButton, limiterButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> analogAtt, vinylAtt, pumpMasterAtt, antiAtt, limiterAtt;

    juce::TextButton flangerOnButton, chorusOnButton, delaySyncButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> flangerOnAtt, chorusOnAtt, delaySyncAtt;

    juce::ComboBox globalKitSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> globalKitAtt;
    juce::TextButton saveKitButton, loadKitButton, randomKitButton, browseFolderButton;
    std::unique_ptr<juce::FileChooser> saveChooser;
    std::unique_ptr<juce::FileChooser> loadChooser;
    std::unique_ptr<juce::FileChooser> folderChooser;

    void refreshKitSelectors();

    juce::TextButton patternButtons[8];
    juce::TextButton patternPageButtons[4];
    juce::TextButton copyPatternButton;
    int currentPatternPage = 0;
    int activePatternButton = 0;

    void updatePatternButtonStates();
    void refreshAllStepButtons();

    juce::Slider masterVolSlider, masterClipperSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolAtt, masterClipAtt;

    juce::Slider masterHpfSlider, masterHpfResSlider, masterLpfSlider, masterLpfResSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpfAtt, hpfResAtt, lpfAtt, lpfResAtt;

    juce::Slider pcmBitsSlider, pcmRateSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pcmBitsAtt, pcmRateAtt;

    juce::Slider driveDistSlider, driveFilterSlider, driveVolSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveDistAtt, driveFilterAtt, driveVolAtt;

    juce::Slider flangerRateSlider, flangerFbSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flRateAtt, flFbAtt;

    juce::Slider springDecaySlider, springToneSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> springDecAtt, springToneAtt;

    juce::Slider pumpThrSlider, pumpAmtSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pumpThrAtt, pumpAmtAtt;

    juce::Slider envFilterCutSlider, envFilterResSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envCutAtt, envResAtt;

    juce::Slider transAttackSlider, transSustainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> transAttAtt, transSusAtt;

    juce::Slider delayTimeSlider, delayFbSlider, delayModRateSlider, delayModDepthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delTimeAtt, delFbAtt, delModRateAtt, delModDepthAtt;

    juce::Slider chorusRateSlider, chorusDepthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusRateAtt, chorusDepthAtt;

    juce::TextButton fillFitButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fillFitAtt;
    
    juce::TextButton fillStepButtons[16];
    juce::TextButton fillSeqModeButton, fillMinusButton, fillPlusButton, fillShiftLeftButton, fillShiftRightButton;
    juce::Label fillLengthLabel;
    juce::Slider hiddenFillLengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fillLengthAtt;
    int fillSeqModeState = 0;

    juce::ComboBox sampleSourceSelectors[12];
    juce::ComboBox sampleVariantSelectors[12];
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sampleSourceAtts[12];

    juce::TextButton muteButtons[12], soloButtons[12], envChannelButtons[12], fitButtons[12];
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAtts[12], soloAtts[12], envChanAtts[12], fitAtts[12];

    juce::Slider volumeSliders[12], panSliders[12], pitchSliders[12], toneSliders[12];
    juce::Slider attackSliders[12], decaySliders[12], springSendSliders[12], delaySendSliders[12];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volAtts[12], panAtts[12], pitchAtts[12], toneAtts[12];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attAtts[12], decAtts[12], springSendAtts[12], delaySendAtts[12];

    std::vector<std::unique_ptr<ChannelLedButton>> channelLedButtons;

    juce::Slider hiddenLengthSliders[12];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stepLengthAtts[12];

    juce::TextButton minusButtons[12], plusButtons[12], seqModeButtons[12];
    juce::TextButton shiftLeftButtons[12], shiftRightButtons[12];
    juce::Label lengthLabels[12];
    int seqModes[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

    StepButton stepButtons[12][32];

    std::unique_ptr<juce::ResizableCornerComponent> cornerResizer;
    juce::Image logoImage;
    bool isSequencerVisible = true;

    // License & Activation
    bool isActivated = false;
    bool showActivationModal = false;
    juce::TextButton licenseBadgeButton;
    ActivationOverlayComponent activationOverlay;

    void updateLicenseState();
    void updateStepButtonVisuals (int ch, int step);
    void updateFillButtonVisuals (int step);
    void updateLengthLabel (int i);
    void updateFillLengthLabel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtasisRhythmEditor)
};