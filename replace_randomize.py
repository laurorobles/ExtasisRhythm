import sys

with open('src/PluginProcessor.cpp', 'r') as f:
    lines = f.readlines()

out = []
in_func = False
braces = 0

for line in lines:
    if "void ExtasisRhythmProcessor::randomizeKit()" in line:
        in_func = True
        braces = 0
        
        # Inject new logic:
        out.append("void ExtasisRhythmProcessor::randomizeKit() {\n")
        out.append('''    if (drumFolders.isEmpty()) return;

    juce::StringArray kickTokens, snareTokens;
    kickTokens.addTokens("bd kik kick kck bombo sub bassdrum bassd kickdrum drum_kik", " ", "");
    snareTokens.addTokens("sd sn snare tarola caja rim snar s snaredrum drum_snr", " ", "");

    for (int i = 0; i < 12; ++i) {
        // Try up to 10 random folders to find a matching sample
        for (int attempts = 0; attempts < 10; ++attempts) {
            int randomFolderIdx = juce::Random::getSystemRandom().nextInt(drumFolders.size());
            juce::StringArray variants = getVariantsForChannel(randomFolderIdx, i);
            if (variants.isEmpty()) continue;

            juce::String chosenSample;

            if (i == 0) {
                juce::StringArray matchKicks;
                for (auto& var : variants) {
                    juce::String nameLower = var.toLowerCase();
                    juce::StringArray fileTokens; fileTokens.addTokens(nameLower, "_-. ", "");
                    bool matched = false;
                    for (auto& tok : kickTokens) { if (fileTokens.contains(tok)) { matched = true; break; } }
                    if (matched) matchKicks.add(var);
                }
                if (!matchKicks.isEmpty()) {
                    int rIdx = juce::Random::getSystemRandom().nextInt(matchKicks.size());
                    chosenSample = matchKicks[rIdx];
                }
            } else if (i == 1) {
                juce::StringArray matchSnares;
                for (auto& var : variants) {
                    juce::String nameLower = var.toLowerCase();
                    juce::StringArray fileTokens; fileTokens.addTokens(nameLower, "_-. ", "");
                    bool matched = false;
                    for (auto& tok : snareTokens) { if (fileTokens.contains(tok)) { matched = true; break; } }
                    if (matched) matchSnares.add(var);
                }
                if (!matchSnares.isEmpty()) {
                    int rIdx = juce::Random::getSystemRandom().nextInt(matchSnares.size());
                    chosenSample = matchSnares[rIdx];
                }
            } else {
                int rIdx = juce::Random::getSystemRandom().nextInt(variants.size());
                chosenSample = variants[rIdx];
            }

            if (chosenSample.isNotEmpty()) {
                loadSampleForChannel(i, randomFolderIdx, chosenSample);
                
                // Update APVTS so GUI dropdown 1 updates automatically!
                if (auto* param = apvts.getParameter("sampleSource_" + juce::String(i))) {
                    param->beginChangeGesture();
                    param->setValueNotifyingHost(param->convertTo0to1((float)randomFolderIdx));
                    param->endChangeGesture();
                }
                break; // Move to next channel
            }
        }
    }
}
''')
    
    if in_func:
        braces += line.count('{')
        braces -= line.count('}')
        if braces == 0 and '{' in line:
            pass # just counting
        if braces <= 0 and "}" in line:
            in_func = False
    else:
        out.append(line)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.writelines(out)
