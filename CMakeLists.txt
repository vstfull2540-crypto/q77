cmake_minimum_required(VERSION 3.22)
project(NAMAudioProcessor VERSION 0.1.0)

# Fetches JUCE during CMake configure. Internet is required the first time.
include(FetchContent)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.10
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(NAMAudioProcessor
  COMPANY_NAME "NAMAudio"
  BUNDLE_ID "com.namaudio.processor"
  PLUGIN_MANUFACTURER_CODE Nmaa
  PLUGIN_CODE Namp
  FORMATS VST3 Standalone
  PRODUCT_NAME "NAM Audio Processor"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS TRUE
  COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(NAMAudioProcessor)
target_sources(NAMAudioProcessor PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginEditor.cpp)
target_compile_features(NAMAudioProcessor PRIVATE cxx_std_17)
target_compile_definitions(NAMAudioProcessor PUBLIC
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(NAMAudioProcessor PRIVATE
  juce::juce_audio_utils
  juce::juce_audio_processors
  juce::juce_gui_extra
  PUBLIC juce::juce_recommended_config_flags
         juce::juce_recommended_lto_flags
         juce::juce_recommended_warning_flags)
