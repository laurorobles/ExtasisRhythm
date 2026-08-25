import re

with open('src/PluginEditor.cpp', 'r') as f:
    cpp = f.read()

replacements = [
    (r'layoutModuleSmart \(fxStartX,         2, fxW, 86, \{ &masterHpfSlider, &masterHpfResSlider, &masterLpfSlider, &masterLpfResSlider \}, nullptr, false, 26, 7\);',
     r'layoutModuleSmart (fxStartX,         2, fxW, 86, { &masterHpfSlider, &masterHpfResSlider, &masterLpfSlider, &masterLpfResSlider }, nullptr, false, 29, 7);'),
    
    (r'layoutModuleSmart \(fxStartX \+ fxW\+gapFx,  2, fxW, 86, \{ &pcmBitsSlider, &pcmRateSlider \}, nullptr, false, 28, 12\);',
     r'layoutModuleSmart (fxStartX + fxW+gapFx,  2, fxW, 86, { &pcmBitsSlider, &pcmRateSlider }, nullptr, false, 31, 12);'),
     
    (r'layoutModuleSmart \(fxStartX \+ \(fxW\+gapFx\)\*2, 2, fxW, 86, \{ &driveDistSlider, &driveFilterSlider, &driveVolSlider \}, nullptr, false, 26, 7\);',
     r'layoutModuleSmart (fxStartX + (fxW+gapFx)*2, 2, fxW, 86, { &driveDistSlider, &driveFilterSlider, &driveVolSlider }, nullptr, false, 29, 7);'),
     
    (r'layoutModuleSmart \(fxStartX \+ \(fxW\+gapFx\)\*3, 2, fxW, 86, \{ &transAttackSlider, &transSustainSlider \}, nullptr, false, 28, 12\);',
     r'layoutModuleSmart (fxStartX + (fxW+gapFx)*3, 2, fxW, 86, { &transAttackSlider, &transSustainSlider }, nullptr, false, 31, 12);'),
     
    (r'layoutModuleSmart \(fxStartX \+ \(fxW\+gapFx\)\*4, 2, fxW, 86, \{ &envFilterCutSlider, &envFilterResSlider \}, nullptr, false, 28, 12\);',
     r'layoutModuleSmart (fxStartX + (fxW+gapFx)*4, 2, fxW, 86, { &envFilterCutSlider, &envFilterResSlider }, nullptr, false, 31, 12);'),
     
    (r'layoutModuleSmart \(fxStartX,         90, fxW, 88, \{ &pumpThrSlider, &pumpAmtSlider \}, nullptr, false, 28, 12\);',
     r'layoutModuleSmart (fxStartX,         90, fxW, 88, { &pumpThrSlider, &pumpAmtSlider }, nullptr, false, 31, 12);'),
     
    (r'layoutModuleSmart \(fxStartX \+ fxW\+gapFx,  90, fxW, 88, \{ &flangerRateSlider, &flangerFbSlider \}, &flangerOnButton, false, 28, 12\);',
     r'layoutModuleSmart (fxStartX + fxW+gapFx,  90, fxW, 88, { &flangerRateSlider, &flangerFbSlider }, &flangerOnButton, false, 31, 12);'),
     
    (r'layoutModuleSmart \(fxStartX \+ \(fxW\+gapFx\)\*2, 90, fxW, 88, \{ &chorusRateSlider, &chorusDepthSlider \}, &chorusOnButton, false, 28, 12\);',
     r'layoutModuleSmart (fxStartX + (fxW+gapFx)*2, 90, fxW, 88, { &chorusRateSlider, &chorusDepthSlider }, &chorusOnButton, false, 31, 12);'),
     
    (r'layoutModuleSmart \(fxStartX \+ \(fxW\+gapFx\)\*3, 90, fxW, 88, \{ &delayTimeSlider, &delayFbSlider, &delayModRateSlider \}, &delaySyncButton, true, 26, 7\);',
     r'layoutModuleSmart (fxStartX + (fxW+gapFx)*3, 90, fxW, 88, { &delayTimeSlider, &delayFbSlider, &delayModRateSlider }, &delaySyncButton, true, 29, 7);'),
     
    (r'layoutModuleSmart \(fxStartX \+ \(fxW\+gapFx\)\*4, 90, fxW, 88, \{ &springDecaySlider, &springToneSlider \}, nullptr, false, 28, 12\);',
     r'layoutModuleSmart (fxStartX + (fxW+gapFx)*4, 90, fxW, 88, { &springDecaySlider, &springToneSlider }, nullptr, false, 31, 12);')
]

for old, new in replacements:
    cpp = re.sub(old, new, cpp)

with open('src/PluginEditor.cpp', 'w') as f:
    f.write(cpp)
