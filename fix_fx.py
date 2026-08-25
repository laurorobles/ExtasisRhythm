import os

src_dir = "/Users/babyonk1/Desktop/ExtasisRecords/ExtasisRhythm/src"
editor_cpp = os.path.join(src_dir, "PluginEditor.cpp")

with open(editor_cpp, "r") as f: c_code = f.read()

# Replace constructor lines
c_code = c_code.replace('    setupFxBtn(flangerOnButton, "OFF", juce::Colour (0xff8e44ad));', '// flanger removed')
c_code = c_code.replace('    flangerOnButton.onClick = [this] { flangerOnButton.setButtonText (flangerOnButton.getToggleState() ? "ON" : "OFF"); };', '')
c_code = c_code.replace('    flangerOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "flangerOn", flangerOnButton);', '')

c_code = c_code.replace('    setupFxBtn(chorusOnButton, "OFF", juce::Colour (0xff2ecc71));', '// chorus removed')
c_code = c_code.replace('    chorusOnButton.onClick = [this] { chorusOnButton.setButtonText (chorusOnButton.getToggleState() ? "ON" : "OFF"); };', '')
c_code = c_code.replace('    chorusOnAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "chorusOn", chorusOnButton);', '')

c_code = c_code.replace('    setupFxBtn(delaySyncButton, "SYNC", juce::Colour (0xff00d2ff));', '// delay removed')
c_code = c_code.replace('    delaySyncAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.apvts, "delaySync", delaySyncButton);', '')

c_code = c_code.replace('    mkEffectKnob (flangerRateSlider, flRateAtt, "flangerRate", juce::Colour (0xff8e44ad));', '')
c_code = c_code.replace('    mkEffectKnob (flangerFbSlider, flFbAtt, "flangerFeedback", juce::Colour (0xff8e44ad));', '')

c_code = c_code.replace('    mkEffectKnob (springDecaySlider, springDecAtt, "springDecay", juce::Colour (0xff556b2f));', '')
c_code = c_code.replace('    mkEffectKnob (springToneSlider, springToneAtt, "springTone", juce::Colour (0xff556b2f));', '')

c_code = c_code.replace('    mkEffectKnob (delayTimeSlider, delTimeAtt, "delayTime", juce::Colour (0xff00d2ff));', '')
c_code = c_code.replace('    mkEffectKnob (delayFbSlider, delFbAtt, "delayFb", juce::Colour (0xff00d2ff));', '')
c_code = c_code.replace('    mkEffectKnob (delayModRateSlider, delModRateAtt, "delayModRate", juce::Colour (0xff00d2ff));', '')
c_code = c_code.replace('    mkEffectKnob (delayModDepthSlider, delModDepthAtt, "delayModDepth", juce::Colour (0xff00d2ff));', '')

c_code = c_code.replace('    mkEffectKnob (chorusRateSlider, chorusRateAtt, "chorusRate", juce::Colour (0xff2ecc71));', '')
c_code = c_code.replace('    mkEffectKnob (chorusDepthSlider, chorusDepthAtt, "chorusDepth", juce::Colour (0xff2ecc71));', '')

# Replace add fxRack
init_fx = """    logoImage = juce::ImageFileFormat::loadFrom (BinaryData::logo_png, (size_t) BinaryData::logo_pngSize);

    fxRack = std::make_unique<FXRackComponent>(audioProcessor, &effectKnobLAF);
    addAndMakeVisible(fxRack.get());"""
c_code = c_code.replace('    logoImage = juce::ImageFileFormat::loadFrom (BinaryData::logo_png, (size_t) BinaryData::logo_pngSize);', init_fx)

# Replace paint lines
paint_rem_1 = '    drawModuleBox (fxStartX + fxW+gap,  90, fxW, 88, "FLANGER", juce::Colour (0xffd8d8d8), true, flangerOnButton.getToggleState()); \n    drawModLabels (fxStartX + fxW+gap,  90, fxW, 88, {"RATE", "FB"}, 28, true, 12);'
paint_rem_2 = '    drawModuleBox (fxStartX + (fxW+gap)*2, 90, fxW, 88, "CE CHORUS", juce::Colour (0xffd8d8d8), true, chorusOnButton.getToggleState()); \n    drawModLabels (fxStartX + (fxW+gap)*2, 90, fxW, 88, {"RATE", "DEPTH"}, 28, true, 12);'
paint_rem_3 = '    drawModuleBox (fxStartX + (fxW+gap)*3, 90, fxW, 88, "DELAY", juce::Colour (0xffd8d8d8), true, false); \n    drawModLabels (fxStartX + (fxW+gap)*3, 90, fxW, 88, {"TIME", "FB", "MOD"}, 26, false, 7); '
paint_rem_4 = '    drawModuleBox (fxStartX + (fxW+gap)*4, 90, fxW, 88, "SPRING", juce::Colour (0xffd8d8d8));      \n    drawModLabels (fxStartX + (fxW+gap)*4, 90, fxW, 88, {"DEC", "TONE"}, 28, false, 12);'
c_code = c_code.replace(paint_rem_1, '')
c_code = c_code.replace(paint_rem_2, '')
c_code = c_code.replace(paint_rem_3, '')
c_code = c_code.replace(paint_rem_4, '')

# Replace resized lines
resized_rem_1 = '    layoutModuleSmart (fxStartX + fxW+gapFx,  90, fxW, 88, { &flangerRateSlider, &flangerFbSlider }, &flangerOnButton, false, 31, 12);'
resized_rem_2 = '    layoutModuleSmart (fxStartX + (fxW+gapFx)*2, 90, fxW, 88, { &chorusRateSlider, &chorusDepthSlider }, &chorusOnButton, false, 31, 12);'
resized_rem_3 = '    layoutModuleSmart (fxStartX + (fxW+gapFx)*3, 90, fxW, 88, { &delayTimeSlider, &delayFbSlider, &delayModRateSlider }, &delaySyncButton, true, 29, 7); '
resized_rem_4 = '    layoutModuleSmart (fxStartX + (fxW+gapFx)*4, 90, fxW, 88, { &springDecaySlider, &springToneSlider }, nullptr, false, 31, 12);'
c_code = c_code.replace(resized_rem_1, '    fxRack->setBounds(sz(fxStartX + fxW + gapFx, 90, 4 * fxW + 3 * gapFx, 88));')
c_code = c_code.replace(resized_rem_2, '')
c_code = c_code.replace(resized_rem_3, '')
c_code = c_code.replace(resized_rem_4, '')

with open(editor_cpp, "w") as f: f.write(c_code)

