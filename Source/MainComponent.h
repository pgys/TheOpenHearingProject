#pragma once

#include <JuceHeader.h>
#include "CustomLNF.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class Visualizer :public juce::AudioVisualiserComponent {
public:
    Visualizer();
    ~Visualizer() = default;
    
private:
};



class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    //Filter instance
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        HighShelfFilter;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        LowShelfFilter;

    //Gain Instance
    juce::dsp::Gain<float> GainInstance;

    //Member getter and setter functions
    void setLastSampleRate (double sampleRate)const;
    const double getlastSampleRate ();


    double inputGainVar{0.0};
    double shelfGain{ 0.0 };

    //GUI visualizer
    Visualizer visualizer;

private:
    //==============================================================================
    // Your private member variables go here...
    // 
    //labels
    juce::Label freqLabel;
    juce::Label qualityFactorLabel;
    juce::Label shelfGainUnitLabel{ "Soft", "dB" };
    juce::Label shelfGainDescLabel{ "shelfGain", "Shelf Gain" };
    juce::Label cutoffFreqLabel{ "cutoffFreq", "Cutoff" };
    juce::Label inputGainLabel{ "inputGain", "Mic Volume" };
    juce::Label inputGainUnitLabel{ "inputGainUnit", "dB" };

    //Sliders
    juce::Slider cutoffFrequency;
    juce::Slider qualityFactor;
    juce::Slider volume;
    juce::Slider ShelfFiltersGain;
    juce::Slider InputGain;
    
    //Label fonts
    juce::Font font{};
    juce::Font Vfont{ 40.0f };

    //Image Components
    juce::ImageComponent mImageComponent;
    juce::ImageComponent softImageComponent;
    juce::ImageComponent loudImageComponent;
    juce::ImageComponent trebleImageComponent;
    juce::ImageComponent bassImageComponent;
    juce::ImageComponent defaultImageComponent;
    juce::ImageComponent defaultqualityFactorImageComponent;

    //sampleRate
    mutable double sampleRate{ 44100 };

    int numInputChannels{ 0 };

    juce::Slider::TextEntryBoxPosition GainTextBoxPos;
    
protected:
    //update bandpass filter
    void UpdateFilter();

    ////RadioGroup for volume buttons
    //enum radioId {
    //    volumeButtons = 100
    //};

    const float minFrequency{ 250.f };
    const float maxFrequency{ 6'000.f };
    const float qFactorVal{ 1.35f };

    //Shelf Gain values
    float lowShelfGain{ 0.0f };
    float highShelfGain{ 0.0f };

    //JUCE Look and Feel class override
    CustomLNF customLNF;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
 



