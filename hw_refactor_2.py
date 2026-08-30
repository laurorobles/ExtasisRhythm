import re

with open('src/PluginProcessor.cpp', 'r') as f:
    code = f.read()

# Replace loadSmartSampleForChannel entirely
smart_loader_new = '''void ExtasisRhythmProcessor::loadSmartSampleForChannel(int ch) {
    if (ch < 0 || ch >= 8) return;
    loadSampleFromCategory(ch);
}

void ExtasisRhythmProcessor::loadNextSample(int channel, int direction) {
    if (channel < 0 || channel >= 8) return;
    int cat = currentCategoryIndex[channel].load();
    if (categorizedFiles[cat].isEmpty()) return;
    
    int currentIdx = currentSampleIndex[channel].load();
    currentIdx += direction;
    if (currentIdx < 0) currentIdx = categorizedFiles[cat].size() - 1;
    if (currentIdx >= categorizedFiles[cat].size()) currentIdx = 0;
    
    currentSampleIndex[channel].store(currentIdx);
    loadSampleFromCategory(channel);
}

void ExtasisRhythmProcessor::changeCategory(int channel, int direction) {
    if (channel < 0 || channel >= 8) return;
    int cat = currentCategoryIndex[channel].load();
    cat += direction;
    if (cat < 0) cat = 7;
    if (cat > 7) cat = 0;
    
    currentCategoryIndex[channel].store(cat);
    currentSampleIndex[channel].store(0);
    loadSampleFromCategory(channel);
}

void ExtasisRhythmProcessor::loadSampleFromCategory(int channel) {
    int cat = currentCategoryIndex[channel].load();
    int idx = currentSampleIndex[channel].load();
    if (cat >= 0 && cat < 8 && !categorizedFiles[cat].isEmpty()) {
        idx = juce::jlimit(0, categorizedFiles[cat].size() - 1, idx);
        sampleBuffers[channel] = new SampleBuffer(categorizedFiles[cat][idx], formatManager);
    }
}
'''

code = re.sub(r'void ExtasisRhythmProcessor::loadSmartSampleForChannel.*?\}\n\n(?=void ExtasisRhythmProcessor::processBlock)', smart_loader_new + '\n\n', code, flags=re.DOTALL)

# Refactor scanSampleFolders
scan_folders_new = '''void ExtasisRhythmProcessor::scanSampleFolders() {
    for (int i = 0; i < 8; ++i) categorizedFiles[i].clear();
    
    juce::Array<juce::File> allWavs;
    for (auto& folderPath : drumFolders) {
        juce::File folder(folderPath);
        if (folder.isDirectory()) {
            allWavs.addArray(folder.findChildFiles(juce::File::findFiles, true, "*.wav;*.aif;*.aiff"));
        }
    }
    
    for (auto& file : allWavs) {
        juce::String nameLower = file.getFileNameWithoutExtension().toLowerCase();
        juce::StringArray fileTokens;
        fileTokens.addTokens(nameLower, "_-. 0123456789()[]", "");
        
        int bestCat = -1;
        int bestDist = 999;
        
        for (int c = 0; c < 8; ++c) {
            for (auto& token : fileTokens) {
                if (token.length() <= 1) continue;
                for (auto& dictWord : channelTags[c]) {
                    int dist = levenshteinDistance(token, dictWord);
                    if (dist < bestDist && dist <= 1) { // Very strict fuzzy or exact match
                        bestDist = dist;
                        bestCat = c;
                    }
                }
            }
        }
        
        // If not matched, guess by primitive heuristic or just dump in Perc2
        if (bestCat == -1) {
            if (nameLower.contains("kick") || nameLower.contains("bd")) bestCat = 0;
            else if (nameLower.contains("snare") || nameLower.contains("sd")) bestCat = 1;
            else if (nameLower.contains("hat") || nameLower.contains("hh")) bestCat = 3;
            else bestCat = 6;
        }
        
        categorizedFiles[bestCat].add(file);
    }
    
    // Auto-load defaults
    for (int i = 0; i < 8; ++i) {
        if (!categorizedFiles[i].isEmpty()) {
            currentCategoryIndex[i].store(i);
            currentSampleIndex[i].store(0);
            loadSampleFromCategory(i);
        }
    }
}
'''
code = re.sub(r'void ExtasisRhythmProcessor::scanSampleFolders.*?\}\n\n(?=void ExtasisRhythmProcessor::loadSmartSampleForChannel)', scan_folders_new + '\n\n', code, flags=re.DOTALL)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(code)

