with open('CMakeLists.txt', 'r') as f:
    code = f.read()

code = code.replace(
'''# Custom post-build step to guarantee installation and app icon
add_custom_command(TARGET ExtasisRhythm_Standalone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.icns"
        "$<TARGET_BUNDLE_DIR:ExtasisRhythm_Standalone>/Contents/Resources/Icon.icns"
    COMMENT "Copying icon.icns to Standalone app bundle"
)''',
'''# Custom post-build step to guarantee installation and app icon
if (APPLE)
    add_custom_command(TARGET ExtasisRhythm_Standalone POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.icns"
            "$<TARGET_BUNDLE_DIR:ExtasisRhythm_Standalone>/Contents/Resources/Icon.icns"
        COMMENT "Copying icon.icns to Standalone app bundle"
    )
endif()'''
)

code = code.replace('juce_recommended_config_flags\n', 'juce::juce_recommended_config_flags\n')

with open('CMakeLists.txt', 'w') as f:
    f.write(code)
