#!/usr/bin/perl
use strict;
use warnings;

my $file = 'src/PluginProcessor.cpp';
open(my $in, '<', $file) or die $!;
my @lines = <$in>;
close($in);

for my $line (@lines) {
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"gain"\s*\+\s*ch\s*\)/cachedParams.chanGain[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"pan"\s*\+\s*ch\s*\)/cachedParams.chanPan[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"pitch"\s*\+\s*ch\s*\)/cachedParams.chanPitch[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"springSend"\s*\+\s*ch\s*\)/cachedParams.chanSSend[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"delaySend"\s*\+\s*ch\s*\)/cachedParams.chanDSend[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"attack"\s*\+\s*ch\s*\)/cachedParams.chanAttack[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"decay"\s*\+\s*ch\s*\)/cachedParams.chanDecay[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"mute"\s*\+\s*ch\s*\)/cachedParams.chanMute[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"solo"\s*\+\s*ch\s*\)/cachedParams.chanSolo[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"envChan_"\s*\+\s*ch\s*\)/cachedParams.chanEnv[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"triplet"\s*\+\s*ch\s*\)/cachedParams.chanTriplet[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"fit"\s*\+\s*ch\s*\)/cachedParams.chanFit[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"tone"\s*\+\s*ch\s*\)/cachedParams.chanTone[i]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"sampleSource_"\s*\+\s*(juce::String\(i\)|ch)\s*\)/cachedParams.sampleSource[i]/g;
    
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"step_"\s*\+\s*(juce::String\(i\)|ch)\s*\+\s*"_"\s*\+\s*juce::String\(s\)\s*\)/cachedParams.stepParams[i][s]/g;
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"fill_step_"\s*\+\s*juce::String\(s\)\s*\)/cachedParams.fillStepParams[s]/g;
}

open(my $out, '>', $file) or die $!;
print $out @lines;
close($out);
