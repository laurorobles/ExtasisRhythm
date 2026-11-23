#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

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
        int flash = processor.flashCounters[channelIndex].load();
        bool isLit = flash > 0;
        float ledSize = 10.0f;
        float ledX = ((float)getWidth() - ledSize) * 0.5f;
        float ledY = 2.0f;
        float cx = ledX + ledSize * 0.5f;
        float cy = ledY + ledSize * 0.5f;

        if (isLit)
        {
            float intensity = juce::jlimit (0.35f, 1.0f, (float)flash / 15.0f);
            juce::ColourGradient glow (juce::Colour (0xff00d2ff).withAlpha (0.65f * intensity), cx, cy,
                                       juce::Colour (0xff00d2ff).withAlpha (0.0f), cx + ledSize * 0.9f, cy, true);
            g.setGradientFill (glow);
            g.fillEllipse (cx - ledSize * 0.9f, cy - ledSize * 0.9f, ledSize * 1.8f, ledSize * 1.8f);
        }

        g.setColour (isLit ? juce::Colour (0xff00d2ff) : juce::Colour (0xff3a3f46));
        g.fillEllipse (ledX, ledY, ledSize, ledSize);
        g.setColour (juce::Colours::black.withAlpha (0.65f));
        g.drawEllipse (ledX, ledY, ledSize, ledSize, 1.2f);
        if (isLit)
        {
            g.setColour (juce::Colours::white.withAlpha (0.95f));
            g.fillEllipse (ledX + 2.5f, ledY + 2.5f, ledSize - 5.0f, ledSize - 5.0f);
        }
    }
};


class ExportDragComponent : public juce::Component, public juce::SettableTooltipClient
{
public:
    ExportDragComponent(ExtasisRhythmProcessor& p) : processor(p) {
        setMouseCursor (juce::MouseCursor::DraggingHandCursor);
        setTooltip ("Drag to DAW or Desktop (WAV Loop)\nClick to Save As...");
    }

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        
        // Background: sleek hardware dark slate with responsive states
        juce::Colour bgCol = isDragging ? juce::Colour (0xff122332) 
                                       : (isHovered ? juce::Colour (0xff1c2630) : juce::Colour (0xff151b22));
        g.setColour (bgCol);
        g.fillRoundedRectangle (bounds, 4.0f);
        
        // Crisp glowing border
        juce::Colour borderCol = isDragging ? juce::Colour (0xffffcc00) 
                                           : (isHovered ? juce::Colour (0xff38bdf8) : juce::Colour (0xff00d2ff).withAlpha (0.75f));
        g.setColour (borderCol);
        g.drawRoundedRectangle (bounds.reduced (0.5f), 4.0f, 1.2f);
        
        // Draw crisp 5-bar vector audio waveform icon on the left (replaces broken emoji character)
        float waveStartX = bounds.getX() + 9.0f;
        float waveCenterY = bounds.getCentreY();
        float barHeights[5] = { 5.0f, 10.0f, 15.0f, 9.0f, 6.0f };
        
        juce::Colour waveCol = isDragging ? juce::Colour (0xffffcc00) 
                                          : (isHovered ? juce::Colour (0xff38bdf8) : juce::Colour (0xff00d2ff));
        g.setColour (waveCol);
        for (int i = 0; i < 5; ++i) {
            float bx = waveStartX + (float)i * 3.5f;
            float bh = barHeights[i];
            g.fillRoundedRectangle (bx, waveCenterY - bh * 0.5f, 2.0f, bh, 1.0f);
        }
        
        // Clean, bold typography
        g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        g.setColour (juce::Colours::white);
        float textX = waveStartX + 20.0f;
        float textW = bounds.getWidth() - (textX - bounds.getX()) - 4.0f;
        g.drawText ("DRAG WAV", (int)textX, (int)bounds.getY(), (int)textW, (int)bounds.getHeight(), juce::Justification::centred);
    }

    void mouseEnter(const juce::MouseEvent&) override { isHovered = true; repaint(); }
    void mouseExit(const juce::MouseEvent&) override { isHovered = false; repaint(); }
    
    void mouseDown(const juce::MouseEvent&) override {
        isDragging = false;
    }

    void mouseDrag(const juce::MouseEvent& e) override {
        if (!isDragging && e.getDistanceFromDragStart() > 3) {
            isDragging = true;
            repaint();
            
            juce::File tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
            juce::File tempWav = tempDir.getChildFile("Extasis_Loop.wav");
            
            if (processor.renderOfflineLoop(tempWav)) {
                juce::StringArray files;
                files.add(tempWav.getFullPathName());
                juce::DragAndDropContainer::performExternalDragDropOfFiles(files, false, this);
            }
            
            isDragging = false;
            repaint();
        }
    }
    
    void mouseUp(const juce::MouseEvent& e) override {
        if (!isDragging && e.getDistanceFromDragStart() <= 3) {
            chooser = std::make_unique<juce::FileChooser>("Save Loop as WAV...", 
                                                          juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Extasis_Loop.wav"), 
                                                          "*.wav");
            chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
                [this](const juce::FileChooser& fc) {
                    juce::File file = fc.getResult();
                    if (file != juce::File()) {
                        processor.renderOfflineLoop(file);
                    }
                });
        }
    }

private:
    ExtasisRhythmProcessor& processor;
    bool isDragging = false;
    bool isHovered = false;
    std::unique_ptr<juce::FileChooser> chooser;
};

class ExtasisRhythmEditor  : public juce::AudioProcessorEditor,
                             public juce::Timer,
                             public juce::FileDragAndDropTarget
{
public:
    ExtasisRhythmEditor (ExtasisRhythmProcessor&);
    ~ExtasisRhythmEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    bool keyPressed (const juce::KeyPress& key) override;
    void mouseDown (const juce::MouseEvent& e) override;

    // File Drag and Drop Target for loading samples onto channels
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void fileDragEnter (const juce::StringArray& files, int x, int y) override;
    void fileDragMove (const juce::StringArray& files, int x, int y) override;
    void fileDragExit (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;

    int dragHoveredChannel = -1;

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
    juce::TextButton saveKitButton, loadKitButton, randomKitButton, browseFolderButton, collectSaveButton;
    ExportDragComponent exportButton { audioProcessor };
    std::unique_ptr<juce::FileChooser> saveChooser;
    std::unique_ptr<juce::FileChooser> loadChooser;
    std::unique_ptr<juce::FileChooser> folderChooser;

    void refreshKitSelectors();

    juce::TextButton patternButtons[8];
    juce::TextButton patternPageButtons[4];
    juce::TextButton copyPatternButton;
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
    juce::TextButton loadCustomButtons[12];
    std::unique_ptr<juce::FileChooser> channelFileChoosers[12];
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

    void updateStepButtonVisuals (int ch, int step);
    void updateFillButtonVisuals (int step);
    void updateLengthLabel (int i);
    void updateFillLengthLabel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtasisRhythmEditor)
};