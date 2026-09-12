#include <iostream>
#include "src/PluginProcessor.h"

void test(int channel, int step, const char* name) {
    ExtasisRhythmProcessor proc;
    proc.prepareToPlay(44100.0, 512);
    proc.loadGlobalDrumKit(0);
    
    // Activar solo un canal
    if (auto* p = proc.apvts.getParameter("step_" + juce::String(channel) + "_" + juce::String(step))) 
        p->setValueNotifyingHost(1.0f);
    
    juce::File outWav("/tmp/extasis_test_" + juce::String(name) + ".wav");
    proc.renderOfflineLoop(outWav);
    std::cout << "Done " << name << std::endl;
}

int main() {
    juce::MessageManager::getInstance();
    test(3, 4, "open_hat");
    test(2, 8, "closed_hat");
    test(10, 10, "crash");
    return 0;
}
