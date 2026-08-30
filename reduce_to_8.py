import re

def process_file(filename):
    with open(filename, 'r') as f:
        content = f.read()
    
    # Simple arrays: [12] -> [8]
    content = re.sub(r'\[12\]', '[8]', content)
    content = content.replace('{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }', '{ -1, -1, -1, -1, -1, -1, -1, -1 }')
    
    # Processor loop limits: < 12 -> < 8
    content = re.sub(r'<\s*12\b', '< 8', content)
    
    # Tagging engine mapping in cpp
    tagging_engine_old = '''    } else {
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
    }'''

    tagging_engine_new = '''    } else {
        channelTags[0].addTokens("bd kik kick kck bombo sub bassdrum bassd kickdrum drum_kik", " ", ""); // Kick
        channelTags[1].addTokens("sd sn snare tarola caja rim snar s snaredrum drum_snr rs rimshot side", " ", ""); // Snare/Rim
        channelTags[2].addTokens("cp clap clp palmas handclap groupclap cla", " ", ""); // Clap
        channelTags[3].addTokens("ch hh closed hat hihat clh hat_c hh_c cht closedhat closed_hh", " ", ""); // Closed Hat
        channelTags[4].addTokens("oh open oph ohat hat_o hh_o oht openhat open_hh hho", " ", ""); // Open Hat
        channelTags[5].addTokens("ht hightom tomhi tom1 h-tom tom_hi conga_hi conga_high bongo_hi bnh cb cowbell bell cwb", " ", ""); // Perc1
        channelTags[6].addTokens("mt midtom tommid tom2 m-tom tom_mid conga_mid conga bongo_lo bol lt lowtom tomlow tom3 floor ftom l-tom", " ", ""); // Perc2
        channelTags[7].addTokens("cr crash cym cymbal crs china splash rd ride rid ride_bell", " ", ""); // Crash/Ride
    }'''
    content = content.replace(tagging_engine_old, tagging_engine_new)
    
    with open(filename, 'w') as f:
        f.write(content)

process_file('src/PluginProcessor.h')
process_file('src/PluginProcessor.cpp')
process_file('src/PluginEditor.h')
process_file('src/PluginEditor.cpp')
