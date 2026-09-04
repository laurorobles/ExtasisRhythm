import re

with open('src/PluginProcessor.cpp', 'r') as f:
    content = f.read()

old_block = """    // Set current kit to the new folder
    for (int i = 0; i < drumFolders.size(); ++i) {
        if (drumFolders[i].getFileName() == kitName) {
            apvts.getRawParameterValue("current_kit")->store(i);
            break;
        }
    }
}"""

new_block = """    // Set current kit to the new folder
    for (int i = 0; i < drumFolders.size(); ++i) {
        if (drumFolders[i].getFileName() == kitName) {
            loadGlobalDrumKit(i);
            break;
        }
    }
}"""

content = content.replace(old_block, new_block)

# Also fix the apvts string from "sample_source" to "sampleSource_"
content = content.replace('apvts.getRawParameterValue("sample_source" + juce::String(i))', 'apvts.getRawParameterValue("sampleSource_" + juce::String(i))')

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(content)
