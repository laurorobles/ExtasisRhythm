#!/usr/bin/perl
use strict;
use warnings;

my $file = 'src/PluginProcessor.cpp';
open(my $in, '<', $file) or die $!;
my @lines = <$in>;
close($in);

for my $line (@lines) {
    $line =~ s/cachedLengthParams/cachedParams.lengthParams/g;
    $line =~ s/cachedStepParams/cachedParams.stepParams/g;
    $line =~ s/cachedTripletFillParam/cachedParams.tripletFill/g;
    $line =~ s/cachedFillFitParam/cachedParams.fillFit/g;
    $line =~ s/cachedFillLengthParam/cachedParams.fillLength/g;
    $line =~ s/cachedFillStepParams/cachedParams.fillStepParams/g;
}

open(my $out, '>', $file) or die $!;
print $out @lines;
close($out);
