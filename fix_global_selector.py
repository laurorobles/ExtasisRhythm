import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

old_global_onchange = r'globalKitSelector\.onChange = \[this\] \{.*?channelStrips\[i\]->sampleVariantSelector\.setSelectedId\(matchIdx >= 0 \? matchIdx \+ 1 : 1, juce::dontSendNotification\);\s*\}\s*\};'

new_global_onchange = '''globalKitSelector.onChange = [this] { 
        int kitIdx = globalKitSelector.getSelectedId() - 1; 
        if (kitIdx < 0) return;
        
        for (int i = 0; i < 12; ++i) { juce::String chStr = juce::String(i);
            auto variants = audioProcessor.getVariantsForChannel(kitIdx, i);
            bool isValid = variants.contains(audioProcessor.currentSampleName[i]);
            
            // Si el kit global cambia, idealmente queremos que CADA canal agarre el sample ideal de ese nuevo kit
            audioProcessor.loadSmartSampleForChannel(i, kitIdx);
            variants = audioProcessor.getVariantsForChannel (kitIdx, i);

            if (auto* sourceParam = audioProcessor.apvts.getParameter("sampleSource_" + juce::String(i))) {
                float normVal = sourceParam->convertTo0to1((float)kitIdx);
                if (std::abs(sourceParam->getValue() - normVal) > 0.001f) {
                    sourceParam->beginChangeGesture();
                    sourceParam->setValueNotifyingHost(normVal);
                    sourceParam->endChangeGesture();
                }
            }

            channelStrips[i]->sampleSourceSelector.setSelectedId (kitIdx + 1, juce::dontSendNotification); 
            channelStrips[i]->sampleVariantSelector.clear (juce::dontSendNotification);
            
            for (int j = 0; j < variants.size(); ++j) {
                channelStrips[i]->sampleVariantSelector.addItem(variants[j].upToLastOccurrenceOf(".wav", false, true).upToLastOccurrenceOf(".WAV", false, true), j + 1);
            }
            
            int matchIdx = variants.indexOf(audioProcessor.currentSampleName[i]); 
            channelStrips[i]->sampleVariantSelector.setSelectedId(matchIdx >= 0 ? matchIdx + 1 : 1, juce::dontSendNotification);
        }
    };'''

cpp = re.sub(old_global_onchange, new_global_onchange, cpp, flags=re.DOTALL)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
