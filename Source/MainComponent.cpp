#include "MainComponent.h"
#include <JuceHeader.h>


//==============================================================================
MainComponent::MainComponent():BandPassFilter(juce::dsp::IIR::Coefficients<float>::makeBandPass(44100, 20000, 0.1f))
{
    // Make sure you set the size of the component after
    // you add any child components.
    
    juce::LookAndFeel::setDefaultLookAndFeel(&customLNF);
    setSize (400, 400);

    

    
    auto tohpLogo{ juce::ImageCache::getFromMemory(BinaryData::tohp_logo_pngCopy1_png,
        BinaryData::tohp_logo_pngCopy1_pngSize) };
    if (!tohpLogo.isNull())
        mImageComponent.setImage(tohpLogo, juce::RectanglePlacement::stretchToFit);
    else
        jassert(!tohpLogo.isNull());

    addAndMakeVisible(mImageComponent);
    addAndMakeVisible(volumeM);
    volumeM.setFont(Vfont);

    addAndMakeVisible(volumeLabel1);
    volumeLabel1.setFont(font);
   
    addAndMakeVisible(volumeLabel2);
    volumeLabel2.setFont(font);

    addAndMakeVisible(volumeLabel3);
    volumeLabel3.setFont(font);

    addAndMakeVisible(button1);
    button1.setClickingTogglesState(true);

    addAndMakeVisible(button2);
    button2.setClickingTogglesState(true);

    addAndMakeVisible(button3);
    button3.setClickingTogglesState(true);

    button1.setRadioGroupId(volumeButtons);
    button2.setRadioGroupId(volumeButtons);
    button3.setRadioGroupId(volumeButtons);
    button1.onClick = [this] {setGain(button1.getButtonText()); };
    button2.onClick = [this] {setGain(button2.getButtonText()); };
    button3.onClick = [this] {setGain(button3.getButtonText()); };

    middlefrequency.setRange(250.0, 5000);
    middlefrequency.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    middlefrequency.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    freqLabel.setText("Frequency", juce::NotificationType::dontSendNotification);
    resonance.setRange(2.0, 5.0);
    resonance.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    resonanceLabel.setText("Resonance", juce::NotificationType::dontSendNotification);
    resonance.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);

    addAndMakeVisible(middlefrequency);
    addAndMakeVisible(freqLabel);
    addAndMakeVisible(resonance);
    addAndMakeVisible(resonanceLabel);
    addAndMakeVisible(visualizer);

    

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.sampleRate = sampleRate;
    spec.numChannels = 2;
    setLastSampleRate(sampleRate);
    BandPassFilter.prepare(spec);
    Gain.prepare(spec);
    BandPassFilter.reset();
    Gain.reset();
    visualizer.clear();
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!
    
    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)

   
    
    auto* device = deviceManager.getCurrentAudioDevice();
    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
    auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;
   
    juce::dsp::AudioBlock<float> block(bufferToFill.buffer->getArrayOfWritePointers(),
        bufferToFill.buffer->getNumChannels(),
        bufferToFill.startSample,
        bufferToFill.numSamples);
    
    auto MidFreq = (float)middlefrequency.getValue();
    auto res = (float)resonance.getValue();
    Gain.setGainDecibels((float)gain);

    UpdateFilter();
    BandPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    Gain.process(juce::dsp::ProcessContextReplacing<float>(block));
    visualizer.pushBuffer(bufferToFill);

    for (auto channel = 0; channel < maxOutputChannels; ++channel)
    {
        if ((!activeOutputChannels[channel]) || maxInputChannels == 0)
        {
            bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        else
        {
            auto actualInputChannel = channel % maxInputChannels; // [1]

            if (!activeInputChannels[channel]) // [2]
            {
                bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
            else // [3]
            {
                auto* inBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel,
                    bufferToFill.startSample);
                
                auto* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
                
                
                for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                    outBuffer[sample] = inBuffer[sample];
            }
        }
    }

}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    auto gradient = juce::ColourGradient(juce::Colours::seagreen,  0.0f, 0.0f, juce::Colours::deepskyblue, 0.0f, 800.0f, false);
    g.setGradientFill(gradient);
    g.fillAll();
    g.setColour(juce::Colour(10));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    mImageComponent.setBounds(5, 10,70, 70 );
    visualizer.setCentreRelative(0.5f, 0.5f);
    visualizer.setBounds(60, 300, 250, 30);
    middlefrequency.setBounds(70, 450, 100, 200);
    //resonanceLabel.setBounds(10, 10, 90, 20);
    resonance.setBounds(270, 450, 100, 200);
    //volumeLabel.setBounds(10, 290, 90, 20 );
    //volume.setBounds(10, 400, getWidth() - 190, 100);
    //commit

    auto area = getLocalBounds().reduced(10);
    auto row = area.removeFromTop(100);

    volumeM.setBounds(140, -30, 300, 300);
    
    volumeLabel1.setBounds(40, 151, 100, 100);
    volumeLabel2.setBounds(180, 151, 100, 100);
    volumeLabel3.setBounds(315, 151, 100, 100);
    
    button1.setBounds(10,150, 120, 30);
    button2.setBounds(140, 150, 120, 30);
    button3.setBounds(270, 150, 120, 30);
    button1.getButtonText().toUpperCase();
    button2.getButtonText().toUpperCase();
    button3.getButtonText().toUpperCase();


    //row2 = (row.removeFromTop(row2.getHeight() + 20).reduced(5, 10));

}

void MainComponent::setLastSampleRate(double sampleRate)
{
    this->sampleRate = sampleRate;
}

double MainComponent::getlastSampleRate()
{
    return sampleRate;
}


void MainComponent::setGain(const juce::String& buttonName)
{
    if (buttonName == "Soft")
        gain = 9.0;
    else if (buttonName == "Medium")
        gain = 12.0;
    else
        gain = 15.0;
}

void MainComponent::UpdateFilter()
{
    auto MidFreq = (float)middlefrequency.getValue();
    auto res = (float)resonance.getValue();
    
    if(MidFreq > 0 && res > 0)
        *BandPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(getlastSampleRate(), MidFreq, res);
   
}

Visualizer::Visualizer():AudioVisualiserComponent{2}
{
    setBufferSize(1028);
    setSamplesPerBlock(256);
    setColours(juce::Colours::deepskyblue, juce::Colours::yellow);
    
}
