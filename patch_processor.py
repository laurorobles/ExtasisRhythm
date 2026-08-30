import re

with open('src/PluginProcessor.h', 'r') as f:
    code = f.read()

# Add #include "HardwareManager.h"
code = code.replace('#include "SampleBuffer.h"', '#include "SampleBuffer.h"\n#include "HardwareManager.h"')

# Add HardwareManager and getParam definition
code = code.replace('juce::StringArray channelTags[8];',
'''    std::unique_ptr<HardwareManager> hardwareManager;
    juce::AudioParameterFloat* getParam(const juce::String& id) { return dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(id)); }
    juce::StringArray channelTags[8];''')

with open('src/PluginProcessor.h', 'w') as f:
    f.write(code)

with open('src/PluginProcessor.cpp', 'r') as f:
    code = f.read()

# Instantiate HardwareManager
code = code.replace('scanSampleFolders();', 'scanSampleFolders();\n    hardwareManager = std::make_unique<HardwareManager>(*this);')

# Update LEDs in processBlock (throttled)
code = code.replace('demoSamplesElapsed.store (currentElapsed + buffer.getNumSamples());\n        }', 
'''demoSamplesElapsed.store (currentElapsed + buffer.getNumSamples());
        }
    }
    
    // Throttled LED updates (e.g. every ~30ms to not choke the MIDI out)
    static int ledCounter = 0;
    if (++ledCounter > 15) {
        if (hardwareManager) hardwareManager->updateLEDs();
        ledCounter = 0;
    }''')

# Add missing CMake target sources
with open('CMakeLists.txt', 'r') as f:
    cmake = f.read()

cmake = cmake.replace('src/PluginProcessor.cpp', 'src/PluginProcessor.cpp\n    src/HardwareManager.cpp')

with open('CMakeLists.txt', 'w') as f:
    f.write(cmake)

