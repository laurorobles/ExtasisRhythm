cmake_minimum_required(VERSION 3.22)
project(ExtasisRhythm VERSION 0.1.1)

if(NOT TARGET juce::juce_audio_utils)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
        add_subdirectory(JUCE)
    else()
        include(FetchContent)
        FetchContent_Declare(
            JUCE
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG 7.0.12
        )
        FetchContent_MakeAvailable(JUCE)
    endif()
endif()

juce_add_binary_data(ExtasisRhythmData
    SOURCES
        assets/logo.png
)

juce_add_plugin(ExtasisRhythm
    COMPANY_NAME "Extasis Records"
    BUNDLE_ID "com.laurorobles.ExtasisRhythm"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    PLUGIN_MANUFACTURER_CODE "Lau1"
    PLUGIN_CODE "ExtR"
    FORMATS VST3 Standalone
    COPY_PLUGIN_AFTER_BUILD TRUE
    PRODUCT_NAME "ExtasisRhythm")

# Genera automáticamente el JuceHeader.h requerido por JUCE con CMake
juce_generate_juce_header(ExtasisRhythm)

target_sources(ExtasisRhythm PRIVATE
    src/PluginProcessor.cpp
    src/PluginProcessor.h
    src/PluginEditor.cpp
    src/PluginEditor.h
)

target_compile_definitions(ExtasisRhythm PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(ExtasisRhythm PRIVATE
    ExtasisRhythmData
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_opengl
    juce_recommended_config_flags
)