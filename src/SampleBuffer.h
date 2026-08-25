#pragma once
#include <JuceHeader.h>

class SampleBuffer : public juce::ReferenceCountedObject
{
public:
    using Ptr = juce::ReferenceCountedObjectPtr<SampleBuffer>;

    SampleBuffer(juce::AudioBuffer<float>&& newBuffer, double newSampleRate) 
        : buffer(std::move(newBuffer)), sampleRate(newSampleRate) 
    {
        numSamples = buffer.getNumSamples();
        numChannels = buffer.getNumChannels();
    }

    juce::AudioBuffer<float> buffer;
    double sampleRate;
    int numSamples;
    int numChannels;
};
