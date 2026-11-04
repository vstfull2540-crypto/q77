#include "PluginEditor.h"

static juce::String dbText (float linear)
{
    if (linear < 0.00001f) return "-inf dB";
    return juce::String (juce::Decibels::gainToDecibels (linear), 1) + " dB";
}

LevelMeter::LevelMeter (juce::String labelText, std::atomic<float>& peakRef,
                        std::atomic<float>& rmsRef)
    : label (std::move (labelText)), peak (peakRef), rms (rmsRef) {}

void LevelMeter::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat();
    g.setColour (juce::Colour (0xff101820));
    g.fillRoundedRectangle (r, 8.0f);
    g.setColour (juce::Colour (0xffb9c5cf));
    g.setFont (14.0f);
    g.drawText (label, getLocalBounds().removeFromTop (26), juce::Justification::centred);
    auto area = getLocalBounds().reduced (12, 34).toFloat();
    const float peakVal = juce::jlimit (0.0f, 1.0f, peak.load());
    const float rmsVal = juce::jlimit (0.0f, 1.0f, rms.load());
    g.setColour (juce::Colour (0xff263642));
    g.fillRoundedRectangle (area, 4.0f);
    auto fill = area;
    fill.setY (area.getBottom() - area.getHeight() * peakVal);
    fill.setHeight (area.getHeight() * peakVal);
    g.setColour (peakVal > 0.90f ? juce::Colour (0xffff4b55)
                  : peakVal > 0.70f ? juce::Colour (0xffffc857)
                                    : juce::Colour (0xff43d9a3));
    g.fillRoundedRectangle (fill, 4.0f);
    const float y = area.getBottom() - area.getHeight() * rmsVal;
    g.setColour (juce::Colours::white);
    g.drawLine (area.getX(), y, area.getRight(), y, 2.0f);
    g.setColour (juce::Colour (0xffd8e2e8));
    g.setFont (12.0f);
    g.drawText (dbText (peak.load()), getLocalBounds().removeFromBottom (20),
                juce::Justification::centred);
}

NAMAudioProcessorEditor::NAMAudioProcessorEditor (NAMAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
      inputMeter ("INPUT PEAK", p.inputPeak, p.inputRms),
      outputMeter ("OUTPUT PEAK", p.outputPeak, p.outputRms)
{
    setSize (720, 440);
    titleLabel.setText ("NAM AUDIO PROCESSOR", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (24.0f, juce::Font::bold));
    titleLabel.setColour (juce::Label::textColourId, juce::Colour (0xfff1f5f9));
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel);

    for (auto* s : { &inputGain, &outputGain })
    {
        s->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 22);
        s->setRange (-24.0, 24.0, 0.1);
        s->setTextValueSuffix (" dB");
        addAndMakeVisible (*s);
    }
    inputGain.setValue (0.0);
    outputGain.setValue (0.0);
    inputGain.onValueChange = [this] { processor.setInputGain ((float) inputGain.getValue()); };
    outputGain.onValueChange = [this] { processor.setOutputGain ((float) outputGain.getValue()); };

    inputLabel.setText ("INPUT GAIN", juce::dontSendNotification);
    outputLabel.setText ("OUTPUT GAIN", juce::dontSendNotification);
    for (auto* l : { &inputLabel, &outputLabel })
    {
        l->setColour (juce::Label::textColourId, juce::Colour (0xffc7d2dc));
        l->setJustificationType (juce::Justification::centred);
        addAndMakeVisible (*l);
    }

    addAndMakeVisible (inputMeter);
    addAndMakeVisible (outputMeter);
    addAndMakeVisible (loadModelButton);
    addAndMakeVisible (bypassButton);
    addAndMakeVisible (modelLabel);
    addAndMakeVisible (statusLabel);
    loadModelButton.onClick = [this]
    {
        chooser = std::make_unique<juce::FileChooser> ("Choose a NAM model", juce::File{}, "*.nam");
        chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this] (const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile() && file.getFileExtension().equalsIgnoreCase (".nam"))
                {
                    processor.setModelPath (file);
                    modelLabel.setText ("Selected model: " + file.getFileName(), juce::dontSendNotification);
                    statusLabel.setText ("Model path saved. NAM DSP engine is not yet linked in this starter build.",
                                         juce::dontSendNotification);
                }
            });
    };
    bypassButton.onClick = [this]
    {
        const bool next = ! processor.bypass.load();
        processor.bypass.store (next);
        bypassButton.setButtonText (next ? "BYPASS: ON" : "BYPASS: OFF");
    };
    modelLabel.setColour (juce::Label::textColourId, juce::Colour (0xffe2e8f0));
    statusLabel.setColour (juce::Label::textColourId, juce::Colour (0xffffc857));
    statusLabel.setFont (juce::Font (12.0f));
    modelLabel.setText ("No .nam model selected", juce::dontSendNotification);
    statusLabel.setText ("Starter: meters + gain + bypass; NAM inference integration required.", juce::dontSendNotification);
    startTimerHz (30);
}

void NAMAudioProcessorEditor::timerCallback()
{
    inputMeter.repaint();
    outputMeter.repaint();
}

void NAMAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0b1118));
    g.setColour (juce::Colour (0xff17232d));
    g.fillRoundedRectangle (12.0f, 58.0f, 696.0f, 330.0f, 12.0f);
    g.setColour (juce::Colour (0xff2b3a46));
    g.drawRoundedRectangle (12.0f, 58.0f, 696.0f, 330.0f, 12.0f, 1.0f);
}

void NAMAudioProcessorEditor::resized()
{
    titleLabel.setBounds (20, 12, 500, 38);
    inputMeter.setBounds (30, 80, 100, 250);
    inputLabel.setBounds (150, 90, 130, 24);
    inputGain.setBounds (150, 112, 130, 140);
    outputLabel.setBounds (300, 90, 130, 24);
    outputGain.setBounds (300, 112, 130, 140);
    outputMeter.setBounds (460, 80, 100, 250);
    loadModelButton.setBounds (575, 90, 120, 42);
    bypassButton.setBounds (575, 145, 120, 36);
    modelLabel.setBounds (30, 345, 660, 24);
    statusLabel.setBounds (30, 390, 660, 32);
}
