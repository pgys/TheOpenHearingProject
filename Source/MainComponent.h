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

    //Bandpass Filter instance
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        BandPassFilter;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        HighShelfFilter;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        LowShelfFilter;

    //Gain Instance
    juce::dsp::Gain<float> Gain;

    //Member getter and setter functions
    void setLastSampleRate (double sampleRate)const;
    const double getlastSampleRate ();


    double gain{0.0};
   
    void setGain( const juce::String& buttonName);

    //GUI visualizer
    Visualizer visualizer;

private:
    //==============================================================================
    // Your private member variables go here...
    // 
    //labels
    juce::Label freqLabel;
    juce::Label qualityFactorLabel;
    juce::Label gainUnitLabel{ "Soft", "dB" };
    juce::Label volumeLabel2{ "Medium", "12 db" };
    juce::Label volumeLabel3{ "Loud", "16 db" };
    juce::Label defaultLabel{ "Default", "Default" };
    juce::Label gainDescLabel{ "Volume", "Mic Volume" };
    juce::Label timbre{ "cutoffFreq", "Cutoff" };

    //Sliders
    juce::Slider cutoffFrequency;
    juce::Slider qualityFactor;
    juce::Slider volume;
    juce::Slider _Gain;
    
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
    
    /** Bandwidth size*/
    float bandWidth{ 1000.f };
    
protected:
    //update bandpass filter
    void UpdateFilter();

    ////RadioGroup for volume buttons
    //enum radioId {
    //    volumeButtons = 100
    //};

    const float minFrequency{ 250.f };
    const float maxFrequency{ 5'000.f };
    const float qFactorVal{ 1.35f };

    //JUCE Look and Feel class override
    CustomLNF customLNF;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
 
//int getChannelCount() {
//    MainComponent Comp;
//    return Comp.getNumInputChannels();
//}


