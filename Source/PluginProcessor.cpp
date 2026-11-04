#include "PluginProcessor.h"
#include "PluginEditor.h"

NAMAudioProcessor::NAMAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

void NAMAudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate;
}

bool NAMAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();
    return (in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo())
        && in == out;
}

void NAMAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = buffer.getNumChannels();
    const int samples = buffer.getNumSamples();
    float inPeak = 0.0f, outPeak = 0.0f;
    double inSquares = 0.0, outSquares = 0.0;
    const float inGain = juce::Decibels::decibelsToGain (inputGainDb.load());
    const float outGain = juce::Decibels::decibelsToGain (outputGainDb.load());
    const bool isBypassed = bypass.load();

    for (int ch = 0; ch < channels; ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        for (int i = 0; i < samples; ++i)
        {
            const float original = x[i];
            inPeak = juce::jmax (inPeak, std::abs (original));
            inSquares += double (original) * double (original);
            if (! isBypassed)
                x[i] = original * inGain * outGain;
            const float y = x[i];
            outPeak = juce::jmax (outPeak, std::abs (y));
            outSquares += double (y) * double (y);
        }
    }

    const double count = juce::jmax (1, channels * samples);
    // Fast peak display; smoothed RMS display to reduce visual flicker.
    inputPeak.store (juce::jmax (inPeak, inputPeak.load() * 0.82f));
    outputPeak.store (juce::jmax (outPeak, outputPeak.load() * 0.82f));
    const float inR = static_cast<float> (std::sqrt (inSquares / count));
    const float outR = static_cast<float> (std::sqrt (outSquares / count));
    inputRms.store (0.85f * inputRms.load() + 0.15f * inR);
    outputRms.store (0.85f * outputRms.load() + 0.15f * outR);
}

void NAMAudioProcessor::setModelPath (const juce::File& file)
{
    const juce::ScopedLock lock (modelLock);
    modelPath = file.getFullPathName();
}

juce::String NAMAudioProcessor::getModelPath() const
{
    const juce::ScopedLock lock (modelLock);
    return modelPath;
}

void NAMAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::XmlElement state ("NAMAudioProcessorState");
    state.setAttribute ("inputGainDb", inputGainDb.load());
    state.setAttribute ("outputGainDb", outputGainDb.load());
    state.setAttribute ("bypass", bypass.load() ? 1 : 0);
    state.setAttribute ("modelPath", getModelPath());
    copyXmlToBinary (state, destData);
}

void NAMAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> state (getXmlFromBinary (data, sizeInBytes));
    if (state == nullptr || ! state->hasTagName ("NAMAudioProcessorState"))
        return;
    setInputGain ((float) state->getDoubleAttribute ("inputGainDb", 0.0));
    setOutputGain ((float) state->getDoubleAttribute ("outputGainDb", 0.0));
    bypass.store (state->getIntAttribute ("bypass", 0) != 0);
    const juce::ScopedLock lock (modelLock);
    modelPath = state->getStringAttribute ("modelPath");
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NAMAudioProcessor();
}
