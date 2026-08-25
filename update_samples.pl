#!/usr/bin/perl
use strict;
use warnings;

my $file = 'src/PluginProcessor.cpp';
open(my $in, '<', $file) or die $!;
my @lines = <$in>;
close($in);

for my $line (@lines) {
    $line =~ s/sampleFileRates\[i\] = 44100.0;//g;
    $line =~ s/const juce::ScopedLock lock \(sampleLock\);//g;
    $line =~ s/juce::ScopedTryLock tryLock \(sampleLock\);//g;
    $line =~ s/for \(int i=0; i<12; \+\+i\) \{ sampleBuffers\[i\]\.setSize\(0, 0\); sampleLengths\[i\] = 0; \}/for (int i=0; i<12; ++i) { juce::SpinLock::ScopedLockType sl(pointerLock); sampleBuffers[i] = nullptr; }/g;

    # In loadSampleForChannel:
    $line =~ s/sampleBuffers\[ch\] = tempBuffer;/auto newBuf = new SampleBuffer(std::move(tempBuffer), fileSr > 0.0 ? fileSr : 44100.0);\n            {\n                juce::SpinLock::ScopedLockType sl(pointerLock);\n                sampleBuffers[ch] = newBuf;\n            }/g;
    $line =~ s/sampleLengths\[ch\] = numSamps;//g;
    $line =~ s/sampleFileRates\[ch\] = fileSr > 0.0 \? fileSr : 44100.0;//g;
    
    # In triggerChannel
    $line =~ s/if \(ch >= 0 && ch < 12 && sampleLengths\[ch\] > 0\)/if (ch >= 0 && ch < 12 && sampleBuffers[ch] != nullptr)/g;

    # In processBlock, we will insert the localSamples fetch at the very beginning of processBlock:
    $line =~ s/midi\.clear\(\);/midi.clear();\n    SampleBuffer::Ptr localSamples[12];\n    {\n        juce::SpinLock::ScopedLockType sl(pointerLock);\n        for (int i=0; i<12; ++i) localSamples[i] = sampleBuffers[i];\n    }/g;

    # In processBlock loops:
    $line =~ s/sampleFileRates\[i\]/localSamples[i]->sampleRate/g;
    
    # In getSampleHermite
    $line =~ s/sampleBuffers\[chIdx\]\.getSample\(audioChan, idx0\)/localSamples[chIdx]->buffer.getSample(audioChan, idx0)/g;
    $line =~ s/sampleBuffers\[chIdx\]\.getSample\(audioChan, idx1\)/localSamples[chIdx]->buffer.getSample(audioChan, idx1)/g;
    $line =~ s/sampleBuffers\[chIdx\]\.getSample\(audioChan, idx2\)/localSamples[chIdx]->buffer.getSample(audioChan, idx2)/g;
    $line =~ s/sampleBuffers\[chIdx\]\.getSample\(audioChan, idx3\)/localSamples[chIdx]->buffer.getSample(audioChan, idx3)/g;

    # Length checks
    $line =~ s/sampleLengths\[ch\]/localSamples[ch]->numSamples/g;
    $line =~ s/sampleLengths\[i\]/localSamples[i]->numSamples/g;

    # Replace remaining sampleBuffers with localSamples
    $line =~ s/sampleBuffers\[i\]\.getNumChannels/localSamples[i]->numChannels/g;
    $line =~ s/sampleBuffers\[i\] != nullptr/localSamples[i] != nullptr/g;
    $line =~ s/sampleBuffers\[ch\] != nullptr/localSamples[ch] != nullptr/g;

    $line =~ s/if \(localSamples\[ch\]->numSamples > 0\)/if (localSamples[ch] != nullptr)/g;
}

open(my $out, '>', $file) or die $!;
print $out @lines;
close($out);
