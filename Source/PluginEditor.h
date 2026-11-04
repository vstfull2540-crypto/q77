#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class LevelMeter final : public juce::Component
{
public:
    LevelMeter (juce::String labelText, std::atomic<float>& peakRef, std::atomic<float>& rmsRef);
    void paint (juce::Graphics& g) override;
private:
    juce::String label;
    std::atomic<float>& peak;
    std::atomic<float>& rms;
};

class NAMAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                      private juce::Timer
{
public:
    explicit NAMAudioProcessorEditor (NAMAudioProcessor&);
    ~NAMAudioProcessorEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    NAMAudioProcessor& processor;
    juce::Slider inputGain, outputGain;
    juce::Label inputLabel, outputLabel, modelLabel, titleLabel, statusLabel;
    juce::TextButton loadModelButton { "LOAD .NAM MODEL" }, bypassButton { "BYPASS: OFF" };
    LevelMeter inputMeter, outputMeter;
    std::unique_ptr<juce::FileChooser> chooser;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NAMAudioProcessorEditor)
};
