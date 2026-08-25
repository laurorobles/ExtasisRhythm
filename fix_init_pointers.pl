#!/usr/bin/perl
use strict;
use warnings;

my $file = 'src/PluginProcessor.cpp';
open(my $in, '<', $file) or die $!;
my $content = do { local $/; <$in> };
close($in);

# We need to replace the entire ExtasisRhythmProcessor::initializeParameterPointers function.
# It starts at void ExtasisRhythmProcessor::initializeParameterPointers() and ends at the first const juce::String ExtasisRhythmProcessor::getName()

my $correct_func = <<'CODE';
void ExtasisRhythmProcessor::initializeParameterPointers()
{
    cachedParams.isPlaying = apvts.getRawParameterValue("isPlaying");
    cachedParams.bpm = apvts.getRawParameterValue("bpm");
    cachedParams.globalKitChoice = apvts.getRawParameterValue("globalKitChoice");

    cachedParams.masterHpf = apvts.getRawParameterValue("masterHpf");
    cachedParams.masterHpfRes = apvts.getRawParameterValue("masterHpfRes");
    cachedParams.masterLpf = apvts.getRawParameterValue("masterLpf");
    cachedParams.masterLpfRes = apvts.getRawParameterValue("masterLpfRes");
    cachedParams.pcmBits = apvts.getRawParameterValue("pcmBits");
    cachedParams.pcmRate = apvts.getRawParameterValue("pcmRate");
    cachedParams.masterAnalog = apvts.getRawParameterValue("masterAnalog");
    cachedParams.masterVinyl = apvts.getRawParameterValue("masterVinyl");
    cachedParams.pumpOn = apvts.getRawParameterValue("pumpOn");
    cachedParams.masterAnti = apvts.getRawParameterValue("masterAnti");
    cachedParams.masterLimiter = apvts.getRawParameterValue("masterLimiter");
    cachedParams.flangerOn = apvts.getRawParameterValue("flangerOn");
    cachedParams.flangerRate = apvts.getRawParameterValue("flangerRate");
    cachedParams.flangerFeedback = apvts.getRawParameterValue("flangerFeedback");
    cachedParams.chorusOn = apvts.getRawParameterValue("chorusOn");
    cachedParams.chorusRate = apvts.getRawParameterValue("chorusRate");
    cachedParams.chorusDepth = apvts.getRawParameterValue("chorusDepth");
    cachedParams.envFilterCut = apvts.getRawParameterValue("envFilterCut");
    cachedParams.envFilterRes = apvts.getRawParameterValue("envFilterRes");
    cachedParams.pumpThr = apvts.getRawParameterValue("pumpThr");
    cachedParams.pumpAmt = apvts.getRawParameterValue("pumpAmt");
    cachedParams.transientAttack = apvts.getRawParameterValue("transientAttack");
    cachedParams.transientSustain = apvts.getRawParameterValue("transientSustain");
    cachedParams.driveDist = apvts.getRawParameterValue("driveDist");
    cachedParams.driveFilter = apvts.getRawParameterValue("driveFilter");
    cachedParams.driveVol = apvts.getRawParameterValue("driveVol");
    cachedParams.springDecay = apvts.getRawParameterValue("springDecay");
    cachedParams.springTone = apvts.getRawParameterValue("springTone");
    cachedParams.delayTime = apvts.getRawParameterValue("delayTime");
    cachedParams.delaySync = apvts.getRawParameterValue("delaySync");
    cachedParams.delayFb = apvts.getRawParameterValue("delayFb");
    cachedParams.delayModRate = apvts.getRawParameterValue("delayModRate");
    cachedParams.delayModDepth = apvts.getRawParameterValue("delayModDepth");
    cachedParams.masterVolume = apvts.getRawParameterValue("masterVolume");
    cachedParams.masterClipper = apvts.getRawParameterValue("masterClipper");

    for (int i = 0; i < 12; ++i)
    {
        juce::String ch = juce::String(i);
        cachedParams.lengthParams[i] = apvts.getRawParameterValue ("length" + ch);
        cachedParams.chanGain[i] = apvts.getRawParameterValue ("gain" + ch);
        cachedParams.chanPan[i] = apvts.getRawParameterValue ("pan" + ch);
        cachedParams.chanPitch[i] = apvts.getRawParameterValue ("pitch" + ch);
        cachedParams.chanSSend[i] = apvts.getRawParameterValue ("springSend" + ch);
        cachedParams.chanDSend[i] = apvts.getRawParameterValue ("delaySend" + ch);
        cachedParams.chanAttack[i] = apvts.getRawParameterValue ("attack" + ch);
        cachedParams.chanDecay[i] = apvts.getRawParameterValue ("decay" + ch);
        cachedParams.chanMute[i] = apvts.getRawParameterValue ("mute" + ch);
        cachedParams.chanSolo[i] = apvts.getRawParameterValue ("solo" + ch);
        cachedParams.chanEnv[i] = apvts.getRawParameterValue ("envChan_" + ch);
        cachedParams.chanTriplet[i] = apvts.getRawParameterValue ("triplet" + ch);
        cachedParams.chanFit[i] = apvts.getRawParameterValue ("fit" + ch);
        cachedParams.chanTone[i] = apvts.getRawParameterValue ("tone" + ch);
        cachedParams.sampleSource[i] = apvts.getRawParameterValue ("sampleSource_" + ch);
        
        for (int s = 0; s < 32; ++s)
        {
            cachedParams.stepParams[i][s] = apvts.getRawParameterValue ("step_" + ch + "_" + juce::String(s));
        }
    }
    for (int s = 0; s < 16; ++s)
    {
        cachedParams.fillStepParams[s] = apvts.getRawParameterValue ("fill_step_" + juce::String (s));
    }
    cachedParams.fillLength = apvts.getRawParameterValue ("fillLength");
    cachedParams.tripletFill = apvts.getRawParameterValue ("tripletFill");
    cachedParams.fillFit = apvts.getRawParameterValue ("fillFit");
}
CODE

$content =~ s/void ExtasisRhythmProcessor::initializeParameterPointers\(\)\s*\{.*?(?=const juce::String ExtasisRhythmProcessor::getName\(\) const)/$correct_func\n/s;

open(my $out, '>', $file) or die $!;
print $out $content;
close($out);
