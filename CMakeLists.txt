cmake_minimum_required(VERSION 3.15)
project(ExtasisRhythm VERSION 0.0.1)

add_subdirectory(JUCE)

juce_add_plugin(ExtasisRhythm
    COMPANY_NAME "ExtasisRecords"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Exta
    PLUGIN_CODE ExRh
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Extasis Rhythm"
)

juce_generate_juce_header(ExtasisRhythm)

target_sources(ExtasisRhythm
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
)

target_compile_definitions(ExtasisRhythm
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(ExtasisRhythm
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_formats
        juce::juce_gui_basics
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
