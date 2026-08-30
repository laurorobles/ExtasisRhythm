cmake_minimum_required(VERSION 3.22)
project(ExtasisRhythm VERSION 2.6.0)

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
    PRODUCT_NAME "ExtasisRhythm"
    ICON_BIG "assets/icon.png"
    ICON_SMALL "assets/icon.png"
    VST3_CATEGORIES "Instrument" "Drum" "Synth"
)

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
    JUCE_PLUGIN_MANAGES_MIDI_OUTPUT=1
)



# Enlazar la librería a tu VST3
target_link_libraries(ExtasisRhythm PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
    PUBLIC juce::juce_recommended_lto_flags
    PUBLIC juce::juce_recommended_warning_flags
)

target_link_libraries(ExtasisRhythm PRIVATE
    ExtasisRhythmData
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_opengl
    juce_recommended_config_flags
    
)
# Custom post-build step to guarantee installation and app icon
add_custom_command(TARGET ExtasisRhythm_Standalone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.icns"
        "$<TARGET_BUNDLE_DIR:ExtasisRhythm_Standalone>/Contents/Resources/Icon.icns"
    COMMENT "Copying icon.icns to Standalone app bundle"
)

# Custom post-build step to guarantee installation to user plug-in directory
if (APPLE)
    if (TARGET ExtasisRhythm_VST3)
        add_custom_command(TARGET ExtasisRhythm_VST3 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$ENV{HOME}/Library/Audio/Plug-Ins/VST3"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "$<TARGET_BUNDLE_DIR:ExtasisRhythm_VST3>"
                "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/ExtasisRhythm.vst3"
            COMMENT "Installing VST3 to ~/Library/Audio/Plug-Ins/VST3/"
        )
    endif()

    if (TARGET ExtasisRhythm_AU)
        add_custom_command(TARGET ExtasisRhythm_AU POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$ENV{HOME}/Library/Audio/Plug-Ins/Components"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "$<TARGET_BUNDLE_DIR:ExtasisRhythm_AU>"
                "$ENV{HOME}/Library/Audio/Plug-Ins/Components/ExtasisRhythm.component"
            COMMENT "Installing AU to ~/Library/Audio/Plug-Ins/Components/"
        )
    endif()
endif()
