import re

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

# Find the start of loadSmartSampleForChannel
start_idx = cpp.find('void ExtasisRhythmProcessor::loadSmartSampleForChannel(int i, int kit) {')
# Find the start of the next function
end_idx = cpp.find('void ExtasisRhythmProcessor::loadGlobalDrumKit(int kit) {')

if start_idx != -1 and end_idx != -1:
    new_func = '''void ExtasisRhythmProcessor::loadSmartSampleForChannel(int i, int kit) {
    if (kit < 0 || kit >= drumFolders.size()) return;
    juce::File kitDir = drumFolders[kit];
    juce::Array<juce::File> allFiles;
    kitDir.findChildFiles(allFiles, juce::File::findFiles, false);
    juce::StringArray allVariants = getVariantsForChannel(kit, i);
    if (allVariants.isEmpty()) return;

    juce::StringArray matchedVars;
    juce::StringArray fuzzyMatchedVars;

    for (auto& var : allVariants) {
        juce::String nameLower = var.toLowerCase();
        juce::StringArray fileTokens; fileTokens.addTokens(nameLower, "_-. ", "");
        
        bool exactMatch = false;
        bool fuzzyMatch = false;

        for (auto& tok : channelTags[i]) {
            if (fileTokens.contains(tok)) {
                exactMatch = true;
                break;
            }
            // Nivel 2: Fuzzy Matching
            for (auto& fTok : fileTokens) {
                if (std::abs(fTok.length() - tok.length()) <= 2) {
                    int dist = levenshteinDistance(fTok, tok);
                    if (dist <= 1 || (tok.length() > 4 && dist <= 2)) {
                        fuzzyMatch = true;
                    }
                }
            }
        }
        
        if (exactMatch) {
            matchedVars.add(var);
        } else if (fuzzyMatch) {
            fuzzyMatchedVars.add(var);
        }
    }

    if (!matchedVars.isEmpty()) {
        // Tier 1: Exact Match
        int rIdx = juce::Random::getSystemRandom().nextInt(matchedVars.size());
        loadSampleForChannel(i, kit, matchedVars[rIdx]);
    } else if (!fuzzyMatchedVars.isEmpty()) {
        // Tier 2: Fuzzy Match
        int rIdx = juce::Random::getSystemRandom().nextInt(fuzzyMatchedVars.size());
        loadSampleForChannel(i, kit, fuzzyMatchedVars[rIdx]);
    } else {
        // Tier 3: DSP Auto-Tagging
        bool dspFound = false;
        int maxAttempts = juce::jmin(20, allVariants.size());
        juce::Array<int> checkedIndices;
        
        for (int attempts = 0; attempts < maxAttempts; ++attempts) {
            int rIdx = juce::Random::getSystemRandom().nextInt(allVariants.size());
            if (checkedIndices.contains(rIdx)) continue;
            checkedIndices.add(rIdx);
            
            juce::File f = kitDir.getChildFile(allVariants[rIdx]);
            int guessedChan = analyzeAudioFile(f);
            
            if (guessedChan == i) {
                loadSampleForChannel(i, kit, allVariants[rIdx]);
                dspFound = true;
                break;
            }
        }
        
        if (!dspFound) {
            // FALLBACK
            int rIdx = juce::Random::getSystemRandom().nextInt(allVariants.size());
            loadSampleForChannel(i, kit, allVariants[rIdx]);
        }
    }
}

'''
    cpp = cpp[:start_idx] + new_func + cpp[end_idx:]
    with open('src/PluginProcessor.cpp', 'w') as f:
        f.write(cpp)
