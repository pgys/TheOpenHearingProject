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
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>
        BandPassFilter;
    juce::dsp::Gain<float> Gain;
    void setLastSampleRate(double sampleRate);
    double getlastSampleRate();

    double gain{ 6.0 };
   
    void setGain( const juce::String& buttonName);

private:
    //==============================================================================
    // Your private member variables go here...
    
    juce::Label freqLabel;
    juce::Label resonanceLabel;
    juce::Slider middlefrequency;
    juce::Slider resonance;
    juce::Slider volume;
    juce::Label volumeLabel1{ "Soft", "9db" };
    juce::Label volumeLabel2{ "Medium", "12db"};
    juce::Label volumeLabel3{ "Loud", "15db" };
    juce::Label volumeM{ "Volume", "Volume" };
    juce::Font font{};
    juce::Font Vfont{ 30.0f };
    juce::ImageComponent mImageComponent;
    /*juce::Image hifrequency;
    juce::Image lofrequency;
    juce::Image hiresonance;
    juce::Image loresonance;
    juce::Image TOHPLogo{};*/
    juce::TextButton button1{ "+" };
    juce::TextButton button2{"++"};
    juce::TextButton button3{ "+++" };
    double sampleRate{ 44100 };
    Visualizer visualizer;
    //RadioGroup for volume buttons
    enum radioId {
        volumeButtons = 100
    };

    CustomLNF customLNF;
    
    void UpdateFilter();
    


    std::atomic<double> value;
    std::atomic<double> resValue;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
 