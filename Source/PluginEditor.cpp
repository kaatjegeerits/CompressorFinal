/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

CompressorV2AudioProcessorEditor::CompressorV2AudioProcessorEditor(CompressorV2AudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p),
	thresholdSlider(*audioProcessor.apvts.getParameter("Threshold"), "dB"),
	attackSlider(*audioProcessor.apvts.getParameter("Attack"), "ms"),
	releaseSlider(*audioProcessor.apvts.getParameter("Release"), "ms"),
	kneeSlider(*audioProcessor.apvts.getParameter("Knee"), "dB"),
	ratioSlider(*audioProcessor.apvts.getParameter("Ratio"), ""),
	mixSlider(*audioProcessor.apvts.getParameter("Mix"), ""),
	gainSlider(*audioProcessor.apvts.getParameter("Gain"), "dB"),

	thresholdSliderAttachment(audioProcessor.apvts, "Threshold", thresholdSlider),
	kneeSliderAttachment(audioProcessor.apvts, "Knee", kneeSlider),
	ratioSliderAttachment(audioProcessor.apvts, "Ratio", ratioSlider),
	attackSliderAttachment(audioProcessor.apvts, "Attack", attackSlider),
	releaseSliderAttachment(audioProcessor.apvts, "Release", releaseSlider),
	mixSliderAttachment(audioProcessor.apvts, "Mix", mixSlider),
	gainSliderAttachment(audioProcessor.apvts, "Gain", gainSlider)
{
	thresholdSlider.labels.add({ 0.f, "-60dB" });
	thresholdSlider.labels.add({ 1.f, "12dB" });
	attackSlider.labels.add({ 0.f, "20ms" });
	attackSlider.labels.add({ 1.f, "500ms" });
	releaseSlider.labels.add({ 0.f, "20ms" });
	releaseSlider.labels.add({ 1.f, "500ms" });
	ratioSlider.labels.add({ 0.f,"1:1" });
	ratioSlider.labels.add({ 1.f,"100:1" });
	kneeSlider.labels.add({ 0.f,"0dB" });
	kneeSlider.labels.add({ 1.f,"1dB" });
	gainSlider.labels.add({ 0.f,"0dB" });
	gainSlider.labels.add({ 1.f,"36dB" });
	mixSlider.labels.add({ 0.f,"dry" });
	mixSlider.labels.add({ 1.f,"wet" });

	setSize(1000, 400);
	addMouseListener(this, true);

	addAndMakeVisible(outputMeterLeft);
	addAndMakeVisible(outputMeterRight);
	addAndMakeVisible(inputMeterLeft);
	addAndMakeVisible(inputMeterRight);
	addAndMakeVisible(audioProcessor.waveViewerCompressed);
	addAndMakeVisible(audioProcessor.waveViewerUncompressed);
	addAndMakeVisible(sliderButton);
	addAndMakeVisible(sliderLine);
	addAndMakeVisible(thresholdLabel);
	addAndMakeVisible(kneeLabel);
	addAndMakeVisible(ratioLabel);
	addAndMakeVisible(attackLabel);
	addAndMakeVisible(releaseLabel);
	addAndMakeVisible(mixLabel);
	addAndMakeVisible(gainLabel);


	for (auto* comp : getComps())
	{
		addAndMakeVisible(comp);
	}

	startTimerHz(24);
}

CompressorV2AudioProcessorEditor::~CompressorV2AudioProcessorEditor()
{
}

//==============================================================================
void CompressorV2AudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkblue.withBrightness(0.1f));

	audioProcessor.waveViewerCompressed.setColours(juce::Colours::darkslateblue, juce::Colours::seagreen);
	audioProcessor.waveViewerUncompressed.setColours(juce::Colours::darkslateblue.withAlpha(0.8f), juce::Colours::lightskyblue.withAlpha(0.2f));

	g.setColour(juce::Colours::darkslateblue.withLightness(0.2f));
	g.fillRoundedRectangle(100, 290, 570, 100, 5.f);
	g.fillRoundedRectangle(690, 290, 210, 100, 5.f);	

	setLabels();
}

void CompressorV2AudioProcessorEditor::setLabels()
{
	thresholdLabel.setBounds(125, 250, 100, 80);
	thresholdLabel.setText("Threshold", juce::NotificationType::dontSendNotification);

	kneeLabel.setBounds(230, 250, 100, 80);
	kneeLabel.setText("Knee", juce::NotificationType::dontSendNotification);
	
	ratioLabel.setBounds(330, 250, 100, 80);
	ratioLabel.setText("Ratio", juce::NotificationType::dontSendNotification);

	attackLabel.setBounds(450, 250, 100, 80);
	attackLabel.setText("Attack", juce::NotificationType::dontSendNotification);

	releaseLabel.setBounds(570, 250, 100, 80);
	releaseLabel.setText("Release", juce::NotificationType::dontSendNotification);

	gainLabel.setBounds(720, 250, 100, 80);
	gainLabel.setText("Gain", juce::NotificationType::dontSendNotification);

	mixLabel.setBounds(830, 250, 100, 80);
	mixLabel.setText("Mix", juce::NotificationType::dontSendNotification);
}

std::vector<juce::Component*> CompressorV2AudioProcessorEditor::getComps()
{
	return
	{
		&thresholdSlider,
		&kneeSlider,
		&ratioSlider,
		&attackSlider,
		&releaseSlider,
		&mixSlider,
		&gainSlider
	};
}

void CompressorV2AudioProcessorEditor::resized()
{
	//input/output meter
	juce::Rectangle<int> outputMeterboundLeft(juce::Point<int>(925, 20), juce::Point<int>(945, 375));
	outputMeterLeft.setBounds(outputMeterboundLeft);

	juce::Rectangle<int> outputMeterboundRight(juce::Point<int>(955, 20), juce::Point<int>(975, 375));
	outputMeterRight.setBounds(outputMeterboundRight);

	juce::Rectangle<int> inputMeterboundLeft(juce::Point<int>(25, 20), juce::Point<int>(45, 375));
	inputMeterLeft.setBounds(inputMeterboundLeft);

	juce::Rectangle<int> inputMeterboundRight(juce::Point<int>(55, 20), juce::Point<int>(75, 375));
	inputMeterRight.setBounds(inputMeterboundRight);

	//sliders
	juce::Rectangle<int> bounds(juce::Point<int>(100, 300), juce::Point<int>(900, 400));

	auto thresholdArea = bounds.removeFromLeft(bounds.getWidth() * 0.15);
	auto kneeArea = bounds.removeFromLeft(bounds.getWidth() * 0.1);
	auto ratioArea = bounds.removeFromLeft(bounds.getWidth() * 0.2);
	auto attackArea = bounds.removeFromLeft(bounds.getWidth() * 0.25);
	auto releaseArea = bounds.removeFromLeft(bounds.getWidth() * 0.35);
	auto mixArea = bounds.removeFromRight(bounds.getWidth() * 0.45);
	auto gainArea = bounds.removeFromRight(bounds.getWidth() * 0.75);

	thresholdSlider.setBounds(thresholdArea);
	kneeSlider.setBounds(kneeArea);
	ratioSlider.setBounds(ratioArea);
	attackSlider.setBounds(attackArea);
	releaseSlider.setBounds(releaseArea);
	mixSlider.setBounds(mixArea);
	gainSlider.setBounds(gainArea);

	//waveform
	juce::Rectangle<int> uncompressedArea(juce::Point<int>(minPositionWaveForm, 0), juce::Point<int>(originalPosition, heightWaveForm));
	juce::Rectangle<int> compressedArea(juce::Point<int>(originalPosition, 0), juce::Point<int>(maxPositionWaveForm, heightWaveForm));

	audioProcessor.waveViewerCompressed.setBounds(compressedArea);
	audioProcessor.waveViewerUncompressed.setBounds(uncompressedArea);

	//sliderButton
	juce::Rectangle<int> sliderButtonArea(juce::Point<int>(originalPosition - 5, 125), juce::Point<int>(originalPosition + 5, 175));
	sliderButton.setBounds(sliderButtonArea);
	sliderButton.isAlwaysOnTop();

	juce::Rectangle<int> sliderLineArea(juce::Point<int>(originalPosition - 2.5f, 0), juce::Point <int>(originalPosition + 2.5f, heightWaveForm));
	sliderLine.setBounds(sliderLineArea);
}

void CompressorV2AudioProcessorEditor::timerCallback()
{
	outputMeterLeft.setLevel(audioProcessor.getRMSValueOutput(0));
	outputMeterLeft.repaint();

	outputMeterRight.setLevel(audioProcessor.getRMSValueOutput(1));
	outputMeterRight.repaint();

	inputMeterLeft.setLevel(audioProcessor.getRMSValueInput(0));
	inputMeterLeft.repaint();

	inputMeterRight.setLevel(audioProcessor.getRMSValueInput(1));
	inputMeterRight.repaint();
}


void CompressorV2AudioProcessorEditor::mouseDrag(const juce::MouseEvent& event)
{
	if (isMouseDown)
	{
		int positionX = event.getDistanceFromDragStartX() + originalPosition;

		if (positionX > maxPositionWaveForm)
		{
			positionX = maxPositionWaveForm;
		}
		if (positionX < minPositionWaveForm)
		{
			positionX = minPositionWaveForm;
		}

		juce::Rectangle<int> sliderButtonArea(juce::Point<int>(positionX - 5, 125), juce::Point<int>(positionX + 5, 175));
		juce::Rectangle<int> uncompressedArea(juce::Point<int>(minPositionWaveForm, 0), juce::Point<int>(positionX, heightWaveForm));
		juce::Rectangle<int> compressedArea(juce::Point<int>(positionX, 0), juce::Point<int>(maxPositionWaveForm, heightWaveForm));
		juce::Rectangle<int> sliderLineArea(juce::Point<int>(positionX - 2.5f, 0), juce::Point <int>(positionX + 2.5f, 280));

		sliderButton.setBounds(sliderButtonArea);
		sliderLine.setBounds(sliderLineArea);
		audioProcessor.waveViewerCompressed.setBounds(compressedArea);
		audioProcessor.waveViewerUncompressed.setBounds(uncompressedArea);
	}
}

void CompressorV2AudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
	if (sliderButton.getLocalBounds().contains(event.getPosition()))
	{
		isMouseDown = true;

	}
	else
	{
		isMouseDown = false;
	}
}

void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
	float sliderPosProportional, float rotaryStartAngle,
	float rotaryEndAngle, juce::Slider& slider)
{
	using namespace juce;

	auto bounds = Rectangle<float>(x, y, width, height);
	g.setColour(juce::Colours::deeppink.withBrightness(0.5f));
	g.fillEllipse(bounds);

	g.setColour(juce::Colours::ghostwhite);
	g.drawEllipse(bounds, 2.f);

	if (auto* rswl = dynamic_cast<CustomRotarySlider*>(&slider))
	{
		auto center = bounds.getCentre();

		Path path;
		Rectangle<float> r;
		r.setLeft(center.getX() - 2);
		r.setRight(center.getX() + 2);
		r.setTop(bounds.getY());
		r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);

		path.addRoundedRectangle(r, 2.f);

		auto sliderAngleRad = jmap(sliderPosProportional, 0.f, 0.1f, rotaryStartAngle, rotaryEndAngle);
		path.applyTransform(AffineTransform().rotated(sliderAngleRad, center.getX(), center.getY()));
		g.fillPath(path);

		g.setFont(rswl->getTextHeight());
		auto text = rswl->getValueString();
		auto strWidth = g.getCurrentFont().getStringWidth(text);
		r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
		r.setCentre(center);
		g.setColour(Colours::black);
		g.fillRect(r);
		
		g.setColour(Colours::white);
		g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
	}
}

juce::String CustomRotarySlider::getValueString() const
{
	juce::String str(getValue());

	if (suffix.isNotEmpty())
	{
		str << " ";

		str << suffix;
	}
	return str;
}

void CustomRotarySlider::paint(juce::Graphics& g)
{
	using namespace juce;

	auto startAngle = degreesToRadians(180.f + 45.f);
	auto endAngle = degreesToRadians(180.f) + MathConstants<float>::twoPi;
	auto range = getRange();

	auto sliderBounds = getSliderBounds();
	
	getLookAndFeel().drawRotarySlider(g, sliderBounds.getX(), sliderBounds.getY(), sliderBounds.getWidth(), sliderBounds.getHeight(),
										jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), startAngle, endAngle, *this);

	auto center = sliderBounds.toFloat().getCentre();
	auto radius = sliderBounds.getWidth() * 0.5f;
	g.setColour(juce::Colours::ghostwhite);
	g.setFont(getTextHeight());
	auto numChoices = labels.size();

	for (int i = 0; i < numChoices; ++i)
	{
		auto pos = labels[i].pos;
		jassert(0.f <= pos);
		jassert(pos <= 1.f);

		auto angle = jmap(pos, 0.f, 1.f, startAngle, endAngle - degreesToRadians(45.f));
		auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1, angle);
		Rectangle<float> r;
		auto str = labels[i].label;
		r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
		r.setCentre(c);
		r.setY(r.getY() + getTextHeight());

		g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
	}
}

juce::Rectangle<int> CustomRotarySlider::getSliderBounds() const
{
	auto bounds = getLocalBounds();
	auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
	size -= getTextHeight() * 2;
	juce::Rectangle<int> r;
	r.setSize(size, size);
	r.setCentre(bounds.getCentreX(), 0);
	r.setY(0);

	return r;
}
//==============================================================================
