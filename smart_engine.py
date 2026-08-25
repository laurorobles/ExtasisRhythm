import re

with open('src/PluginProcessor.h', 'r') as f:
    h = f.read()

# Add member variables and methods
if 'juce::StringArray channelTags[12];' not in h:
    h = re.sub(
        r'juce::StringArray currentSampleName;',
        r'juce::StringArray currentSampleName;\n    juce::StringArray channelTags[12];\n    void loadTagsFromJson();\n    int levenshteinDistance(const juce::String& s1, const juce::String& s2) const;\n    int analyzeAudioFile(const juce::File& file);',
        h
    )
    with open('src/PluginProcessor.h', 'w') as f:
        f.write(h)

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

# 1. Add constructor call to loadTagsFromJson()
if 'loadTagsFromJson();' not in cpp:
    cpp = re.sub(
        r'if \(\!drumFolders\.isEmpty\(\)\) loadGlobalDrumKit\(defaultKitIdx\);',
        r'loadTagsFromJson();\n    if (!drumFolders.isEmpty()) loadGlobalDrumKit(defaultKitIdx);',
        cpp
    )

# 2. Add the implementation of the new methods
new_methods = '''
void ExtasisRhythmProcessor::loadTagsFromJson() {
    juce::File docsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    juce::File sampleDir = docsDir.getChildFile("ExtasisRhythm_Samples");
    if (!sampleDir.exists()) sampleDir.createDirectory();

    juce::File tagsFile = sampleDir.getChildFile("tags.json");
    if (!tagsFile.existsAsFile()) {
        // Create default JSON
        juce::DynamicObject::Ptr root = new juce::DynamicObject();
        root->setProperty("0_Kick", "bd kik kick kck bombo sub bassdrum bassd kickdrum drum_kik");
        root->setProperty("1_Snare", "sd sn snare tarola caja rim snar s snaredrum drum_snr");
        root->setProperty("2_ClosedHat", "ch hh closed hat hihat clh hat_c hh_c cht closedhat closed_hh");
        root->setProperty("3_OpenHat", "oh open oph ohat hat_o hh_o oht openhat open_hh splash hho");
        root->setProperty("4_Clap", "cp clap clp palmas handclap groupclap cla");
        root->setProperty("5_Rimshot", "rs rim rimshot side sidestick woodblock wblk wood clave claves");
        root->setProperty("6_HiPerc", "ht hightom tomhi tom1 h-tom tom_hi conga_hi conga_high bongo_hi bnh");
        root->setProperty("7_MidPerc", "mt midtom tommid tom2 m-tom tom_mid conga_mid conga bongo_lo bol");
        root->setProperty("8_LowPerc", "lt lowtom tomlow tom3 floor ftom l-tom tom_lo timbale tmb conga_lo conga_low");
        root->setProperty("9_Cowbell", "cb cowbell bell cwb cow quijada vibraslap agogo ago shaker shk tambourine tamb maraca mrc");
        root->setProperty("10_Crash", "cr crash cym cymbal crs china");
        root->setProperty("11_Ride", "rd ride rid ride_bell ride_bow cym_ride");
        
        juce::var jsonVar(root.get());
        juce::String jsonString = juce::JSON::toString(jsonVar);
        tagsFile.replaceWithText(jsonString);
    }

    juce::var parsed = juce::JSON::parse(tagsFile);
    if (auto* obj = parsed.getDynamicObject()) {
        juce::StringArray keys = {"0_Kick", "1_Snare", "2_ClosedHat", "3_OpenHat", "4_Clap", "5_Rimshot", "6_HiPerc", "7_MidPerc", "8_LowPerc", "9_Cowbell", "10_Crash", "11_Ride"};
        for (int i = 0; i < 12; ++i) {
            juce::String tokens = obj->getProperty(keys[i]).toString();
            channelTags[i].addTokens(tokens, " ,", "");
        }
    } else {
        // Fallback if parsing fails
        channelTags[0].addTokens("bd kik kick kck bombo sub bassdrum bassd kickdrum drum_kik", " ", "");
        channelTags[1].addTokens("sd sn snare tarola caja rim snar s snaredrum drum_snr", " ", "");
        channelTags[2].addTokens("ch hh closed hat hihat clh hat_c hh_c cht closedhat closed_hh", " ", "");
        channelTags[3].addTokens("oh open oph ohat hat_o hh_o oht openhat open_hh splash hho", " ", "");
        channelTags[4].addTokens("cp clap clp palmas handclap groupclap cla", " ", "");
        channelTags[5].addTokens("rs rim rimshot side sidestick woodblock wblk wood clave claves", " ", "");
        channelTags[6].addTokens("ht hightom tomhi tom1 h-tom tom_hi conga_hi conga_high bongo_hi bnh", " ", "");
        channelTags[7].addTokens("mt midtom tommid tom2 m-tom tom_mid conga_mid conga bongo_lo bol", " ", "");
        channelTags[8].addTokens("lt lowtom tomlow tom3 floor ftom l-tom tom_lo timbale tmb conga_lo conga_low", " ", "");
        channelTags[9].addTokens("cb cowbell bell cwb cow quijada vibraslap agogo ago shaker shk tambourine tamb maraca mrc", " ", "");
        channelTags[10].addTokens("cr crash cym cymbal crs china", " ", "");
        channelTags[11].addTokens("rd ride rid ride_bell ride_bow cym_ride", " ", "");
    }
}

int ExtasisRhythmProcessor::levenshteinDistance(const juce::String& s1, const juce::String& s2) const {
    int m = s1.length(); int n = s2.length();
    if (m == 0) return n; if (n == 0) return m;
    std::vector<int> costs(n + 1);
    for (int k = 0; k <= n; k++) costs[k] = k;
    for (int i = 0; i < m; i++) {
        int cost = i; int pC = i;
        for (int j = 0; j < n; j++) {
            int val = costs[j];
            costs[j] = pC;
            pC = val;
            if (s1[i] != s2[j]) {
                cost = std::min(std::min(costs[j], costs[j + 1]), cost) + 1;
            } else {
                cost = costs[j];
            }
            costs[j + 1] = cost;
        }
    }
    return costs[n];
}

int ExtasisRhythmProcessor::analyzeAudioFile(const juce::File& file) {
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (!reader) return -1;
    
    int numSamples = juce::jmin((int)reader->lengthInSamples, 8192); // Leer primeros ~185ms
    if (numSamples < 100) return -1;

    juce::AudioBuffer<float> buffer(1, numSamples);
    reader->read(&buffer, 0, numSamples, 0, true, false);
    
    const float* data = buffer.getReadPointer(0);
    int zeroCrossings = 0;
    float energy = 0.0f;
    float peak = 0.0f;
    
    for (int i = 1; i < numSamples; ++i) {
        energy += data[i] * data[i];
        if (std::abs(data[i]) > peak) peak = std::abs(data[i]);
        if ((data[i] >= 0.0f && data[i-1] < 0.0f) || (data[i] < 0.0f && data[i-1] >= 0.0f)) {
            zeroCrossings++;
        }
    }
    
    float zcrRate = (float)zeroCrossings / (float)numSamples; // 0.0 a 1.0
    float rms = std::sqrt(energy / numSamples);
    
    // Heurísticas súper simples:
    // ZCR bajo (< 0.05) y mucha energía -> Bombo (0)
    // ZCR muy alto (> 0.20) -> Hihats/Cymbals (2, 3, 10, 11)
    // ZCR medio (0.05 - 0.20) -> Snares/Claps/Percs
    if (zcrRate < 0.04f && peak > 0.4f) return 0; // Kick
    if (zcrRate > 0.25f) return 2; // Hat
    if (zcrRate > 0.10f && zcrRate <= 0.25f && peak > 0.5f) return 1; // Snare
    if (zcrRate >= 0.04f && zcrRate <= 0.10f) return 7; // Mid Perc / Tom
    
    return -1; // Desconocido
}

'''
if 'void ExtasisRhythmProcessor::loadTagsFromJson()' not in cpp:
    cpp = cpp + '\n' + new_methods

# 3. Modify loadSmartSampleForChannel
old_smart_start = r'void ExtasisRhythmProcessor::loadSmartSampleForChannel\(int i, int kit\) \{.*?juce::StringArray matchedVars;'
new_smart = '''void ExtasisRhythmProcessor::loadSmartSampleForChannel(int i, int kit) {
    if (kit < 0 || kit >= drumFolders.size()) return;
    juce::File kitDir = drumFolders[kit];
    juce::Array<juce::File> allFiles;
    kitDir.findChildFiles(allFiles, juce::File::findFiles, false);
    juce::StringArray allVariants = getVariantsForChannel(kit, i);
    if (allVariants.isEmpty()) return;

    juce::StringArray matchedVars;
    juce::StringArray fuzzyMatchedVars;
    juce::StringArray dspMatchedVars;

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
        // Tier 3: DSP Auto-Tagging (Limited to max 20 random samples to avoid freezing)
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
    }'''

cpp = re.sub(r'void ExtasisRhythmProcessor::loadSmartSampleForChannel\(int i, int kit\) \{.*?(?= \}) \}', new_smart + '\n}', cpp, flags=re.DOTALL)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)
