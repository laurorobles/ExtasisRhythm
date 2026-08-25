import re

with open('src/PluginProcessor.cpp', 'r') as f:
    cpp = f.read()

old_sync = r'if \(pos->getPpqPosition\(\)\.hasValue\(\)\) \{ ppqPosition = \*pos->getPpqPosition\(\); hasHostTime = true; \}'
new_sync = '''if (pos->getPpqPosition().hasValue() && currentHostPlaying) { 
                ppqPosition = *pos->getPpqPosition(); 
                hasHostTime = true; 
            }'''

cpp = re.sub(old_sync, new_sync, cpp)

with open('src/PluginProcessor.cpp', 'w') as f:
    f.write(cpp)
