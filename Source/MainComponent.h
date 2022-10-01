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

    //Gain Instance
    juce::dsp::Gain<float> Gain;

    //Member getter and setter functions
    void setLastSampleRate(const double sampleRate);
    double getlastSampleRate ()const;


    double gain;
   
    void setGain( const juce::String& buttonName);

    int getNumInputChannels();

    void setNumInputChannels(const int numInputChannels);

    //GUI visualizer
    Visualizer visualizer;

private:
    //==============================================================================
    // Your private member variables go here...
    // 
    //labels
    juce::Label freqLabel;
    juce::Label resonanceLabel;
    juce::Label volumeLabel1{ "Soft", "6 db" };
    juce::Label volumeLabel2{ "Medium", "12 db" };
    juce::Label volumeLabel3{ "Loud", "16 db" };
    juce::Label defaultLabel{ "Default", "Default" };
    juce::Label volumeM{ "Volume", "Volume" };

    //Sliders
    juce::Slider middlefrequency;
    juce::Slider resonance;
    juce::Slider volume;
    juce::Slider _Gain;
    
    //Label fonts
    juce::Font font{};
    juce::Font Vfont{ 30.0f };

    //Image Components
    juce::ImageComponent mImageComponent;
    juce::ImageComponent softImageComponent;
    juce::ImageComponent loudImageComponent;
    juce::ImageComponent trebleImageComponent;
    juce::ImageComponent bassImageComponent;
    juce::ImageComponent defaultImageComponent;
    juce::ImageComponent defaultResonanceImageComponent;

    ////Buttons
    //juce::TextButton button1{ "+" };
    //juce::TextButton button2{"++"};
    //juce::TextButton button3{ "+++" };

    //sampleRate
    double sampleRate{ 44100 };

    int numInputChannels{ 0 };
    

protected:
    //update bandpass filter
    void UpdateFilter();

    ////RadioGroup for volume buttons
    //enum radioId {
    //    volumeButtons = 100
    //};

    

    //JUCE Look and Feel class override
    CustomLNF customLNF;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
 
//int getChannelCount() {
//    MainComponent Comp;
//    return Comp.getNumInputChannels();
//}


