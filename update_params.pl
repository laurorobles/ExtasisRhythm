#!/usr/bin/perl
use strict;
use warnings;

my $file = 'src/PluginProcessor.cpp';
open(my $in, '<', $file) or die $!;
my @lines = <$in>;
close($in);

for my $line (@lines) {
    $line =~ s/apvts\.getRawParameterValue\s*\(\s*"([^"]+)"\s*\)/cachedParams.$1/g;
}

open(my $out, '>', $file) or die $!;
print $out @lines;
close($out);
