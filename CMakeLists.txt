cmake_minimum_required(VERSION 3.22)
project(Series500ModularRack VERSION 0.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "")

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.15
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_binary_data(PanelArt SOURCES Assets/Panel.png)
if(APPLE)
    set(RACK_FORMATS VST3 AU Standalone)
else()
    # Audio Unit is an Apple-only format. Windows builds produce VST3 and the
    # standalone application from this same CMake project.
    set(RACK_FORMATS VST3 Standalone)
endif()

option(RACK_COPY_PLUGIN_AFTER_BUILD "Copy built plug-ins to system plug-in folders" OFF)

juce_add_plugin(RackStrip
    COMPANY_NAME "Series 500 Modular Rack"
    BUNDLE_ID com.series500modularrack.audiofx
    PLUGIN_MANUFACTURER_CODE Mstu
    PLUGIN_CODE Rkst
    FORMATS ${RACK_FORMATS}
    PRODUCT_NAME "Series 500 Modular Rack"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD ${RACK_COPY_PLUGIN_AFTER_BUILD}
    AU_MAIN_TYPE kAudioUnitType_Effect
    VST3_CATEGORIES Fx)

target_sources(RackStrip PRIVATE
    Source/Modules.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_definitions(RackStrip PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(RackStrip
    PRIVATE
        PanelArt juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Outil de développement : captures d'écran de l'interface (désactivé par défaut)
option(RACK_TOOLS "Build snapshot tool" OFF)
if(RACK_TOOLS)
    juce_add_console_app(Snapshot PRODUCT_NAME "Snapshot")
    target_sources(Snapshot PRIVATE tools/Snapshot.cpp Source/Modules.cpp Source/PluginProcessor.cpp Source/PluginEditor.cpp)
    target_compile_definitions(Snapshot PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_MODAL_LOOPS_PERMITTED=1)
    target_link_libraries(Snapshot PRIVATE PanelArt juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)
endif()

option(RACK_TESTS "Build and register DSP/state smoke tests" OFF)
if(RACK_TESTS)
    enable_testing()
    juce_add_console_app(RackSmokeTests PRODUCT_NAME "Rack Smoke Tests")
    target_sources(RackSmokeTests PRIVATE
        tests/RackSmokeTests.cpp
        Source/Modules.cpp
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)
    target_compile_definitions(RackSmokeTests PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_MODAL_LOOPS_PERMITTED=1)
    target_link_libraries(RackSmokeTests PRIVATE
        PanelArt juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags)
    add_test(NAME rack_smoke COMMAND RackSmokeTests)
endif()
