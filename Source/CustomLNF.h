/*
  ==============================================================================

    CustomLNF.h
    Created: 28 Jul 2022 8:14:03am
    Author:  Emmanuel Egwu

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class CustomLNF:public juce::LookAndFeel_V4 {

public:
    juce::ColourGradient backGroundGrad{ juce::Colours::seagreen,  0.0f, 0.0f,
        juce::Colours::deepskyblue, 0.0f, 800.0f, false };

    void 	drawLinearSlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos,
        float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider&) override;

    void 	drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider&) override;
};