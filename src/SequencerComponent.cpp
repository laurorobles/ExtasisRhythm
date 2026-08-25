#include "SequencerComponent.h"
#include "PluginEditor.h" // Might be needed for some types, or maybe just PluginProcessor

SequencerComponent::SequencerComponent(ExtasisRhythmProcessor& p) : audioProcessor(p)
{
    // Constructor logic to be moved here...
}

SequencerComponent::~SequencerComponent()
{
}

void SequencerComponent::paint(juce::Graphics& g)
{
}

void SequencerComponent::resized()
{
}

void SequencerComponent::refreshAllStepButtons() {}
void SequencerComponent::updateStepButtonVisuals(int ch, int step) {}
void SequencerComponent::updateFillButtonVisuals(int step) {}
void SequencerComponent::updateLengthLabel(int i) {}
void SequencerComponent::updateFillLengthLabel() {}
void SequencerComponent::updatePatternButtonStates() {}
