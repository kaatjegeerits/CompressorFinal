/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "C:\Users\kaatj\OneDrive\Bureaublad\Audio Prog\JUCE\CompressorV2\Builds\VisualStudio2022\Levelmeter.h"
#include "C:\Users\kaatj\OneDrive\Bureaublad\Audio Prog\JUCE\CompressorV2\Builds\VisualStudio2022\SliderButton.h"
struct LookAndFeel : juce::LookAndFeel_V4
{
	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
		float sliderPosProportional, float rotaryStartAngle,
		float rotaryEndAngle, juce::Slider& slider) override;
};

struct CustomRotarySlider : juce::Slider
{
	CustomRotarySlider(juce::RangedAudioParameter& param, const juce::String& unitSuffix)
		:juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
			juce::Slider::TextEntryBoxPosition::NoTextBox),
		param(&param),
		suffix(unitSuffix)
	{
		setLookAndFeel(&instance);
	}

	struct LabelPos
	{
		float pos;
		juce::String label;
	};

	juce::Array<LabelPos> labels;

	~CustomRotarySlider()
	{
		setLookAndFeel(nullptr);
	}

	void paint(juce::Graphics& g) override;
	juce::Rectangle<int> getSliderBounds() const;
	int getTextHeight() const { return 14; };
	juce::String getValueString() const;
private:
	LookAndFeel instance;
	juce::RangedAudioParameter* param;
	juce::String suffix;
};
//==============================================================================
/**
*/
class CompressorV2AudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer, private juce::MouseListener
{
public:
	CompressorV2AudioProcessorEditor(CompressorV2AudioProcessor&);
	~CompressorV2AudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;
	void timerCallback() override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseDown(const juce::MouseEvent& event) override;
	void setLabels();

private:
	CompressorV2AudioProcessor& audioProcessor;

	Gui::LevelMeter outputMeterLeft, outputMeterRight{};
	Gui::LevelMeter inputMeterLeft, inputMeterRight{};
	Gui::SliderButton sliderButton;
	Gui::SliderButton sliderLine;
	juce::Label thresholdLabel, kneeLabel, ratioLabel, attackLabel, releaseLabel, mixLabel, gainLabel;

	CustomRotarySlider thresholdSlider, kneeSlider, ratioSlider, attackSlider, releaseSlider, mixSlider, gainSlider;
	std::vector<juce::Component*> getComps();
	using APVTS = juce::AudioProcessorValueTreeState;
	APVTS::SliderAttachment thresholdSliderAttachment, kneeSliderAttachment, ratioSliderAttachment, attackSliderAttachment, releaseSliderAttachment, mixSliderAttachment, gainSliderAttachment;

	int originalPosition{ 450 };
	int minPositionWaveForm{ 100 };
	int maxPositionWaveForm{ 900 };
	int heightWaveForm{ 280 };

	bool isMouseDown{};
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorV2AudioProcessorEditor)
};
