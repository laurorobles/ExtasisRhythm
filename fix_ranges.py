import re

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

# Replace masterHpf
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("masterHpf", 1\), "HPF", 20\.0f, 2000\.0f, 20\.0f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("masterHpf", 1), "HPF", juce::NormalisableRange<float>(20.0f, 10000.0f, 1.0f, 0.3f), 20.0f)', cpp)

# Replace masterLpf
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("masterLpf", 1\), "LPF", 500\.0f, 20000\.0f, 20000\.0f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("masterLpf", 1), "LPF", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20000.0f)', cpp)

# Replace flangerRate
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("flangerRate", 1\), "FRate", 0\.1f, 10\.0f, 0\.40f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("flangerRate", 1), "FRate", juce::NormalisableRange<float>(0.05f, 15.0f, 0.01f, 0.5f), 0.40f)', cpp)

# Replace flangerFeedback
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("flangerFeedback", 1\), "FFB", -0\.9f, 0\.9f, 0\.0f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("flangerFeedback", 1), "FFB", -0.95f, 0.95f, 0.0f)', cpp)

# Replace chorusRate
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("chorusRate", 1\), "CRate", 0\.1f, 10\.0f, 1\.5f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("chorusRate", 1), "CRate", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f), 1.5f)', cpp)

# Replace springTone
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("springTone", 1\), "STon", 500\.0f, 10000\.0f, 4000\.0f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("springTone", 1), "STon", juce::NormalisableRange<float>(200.0f, 12000.0f, 1.0f, 0.3f), 4000.0f)', cpp)

# Replace envFilterCut
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("envFilterCut", 1\), "EnvCut", 200\.0f, 15000\.0f, 3000\.0f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("envFilterCut", 1), "EnvCut", juce::NormalisableRange<float>(50.0f, 15000.0f, 1.0f, 0.3f), 3000.0f)', cpp)

# Replace delayTime
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("delayTime", 1\), "DTime", 10\.0f, 1125\.0f, 300\.0f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("delayTime", 1), "DTime", juce::NormalisableRange<float>(1.0f, 2000.0f, 1.0f, 0.4f), 300.0f)', cpp)

# Replace delayFb (allow self oscillation)
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("delayFb", 1\), "DFb", 0\.0f, 0\.95f, 0\.30f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("delayFb", 1), "DFb", 0.0f, 1.2f, 0.30f)', cpp)

# Replace delayModRate
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("delayModRate", 1\), "DModR", 0\.1f, 10\.0f, 1\.5f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("delayModRate", 1), "DModR", juce::NormalisableRange<float>(0.05f, 10.0f, 0.01f, 0.5f), 1.5f)', cpp)

# Note: chanTone in channel loop
cpp = re.sub(r'juce::AudioParameterFloat> \(juce::ParameterID \("chanTone_" \+ juce::String \(i\), 1\), "Tone", 100\.0f, 15000\.0f, 15000\.0f\)',
             r'juce::AudioParameterFloat> (juce::ParameterID ("chanTone_" + juce::String (i), 1), "Tone", juce::NormalisableRange<float>(100.0f, 15000.0f, 1.0f, 0.3f), 15000.0f)', cpp)


with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)
