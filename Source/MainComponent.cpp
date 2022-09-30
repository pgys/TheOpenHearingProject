#include "MainComponent.h"
#include <JuceHeader.h>

/** Global variable for the visualizer constructor */
int visualizerNumChannel{ 2 };
//==============================================================================
MainComponent::MainComponent():BandPassFilter(juce::dsp::IIR::Coefficients<float>::makeBandPass(44100, 20000, 0.1f)), gain{3.0}
{
    // Make sure you set the size of the component after
    // you add any child components.
    
    juce::LookAndFeel::setDefaultLookAndFeel(&customLNF);
    setSize (400, 400);


    //get image from binary dataand set image component
    
    auto Logo{ juce::ImageCache::getFromMemory(BinaryData::tohp_logo_pngCopy1_png,
        BinaryData::tohp_logo_pngCopy1_pngSize) };
    if (!Logo.isNull())
        mImageComponent.setImage(Logo, juce::RectanglePlacement::centred);
    else
        jassert(!Logo.isNull());

    auto Bass{ juce::ImageCache::getFromMemory(BinaryData::bass_w_png,
        BinaryData::bass_w_pngSize) };
    if (!Bass.isNull())
        bassImageComponent.setImage(Bass, juce::RectanglePlacement::centred);
    else
        jassert(!Bass.isNull());

    auto Soft{ juce::ImageCache::getFromMemory(BinaryData::soft_w_png,
        BinaryData::soft_w_pngSize) };
    if (!Soft.isNull())
        softImageComponent.setImage(Soft, juce::RectanglePlacement::centred);
    else
        jassert(!Soft.isNull());

    auto Loud{ juce::ImageCache::getFromMemory(BinaryData::loud_w_png,
        BinaryData::loud_w_pngSize) };
    if (!Loud.isNull())
        loudImageComponent.setImage(Loud, juce::RectanglePlacement::centred);
    else
        jassert(!Loud.isNull());

    auto Treble{ juce::ImageCache::getFromMemory(BinaryData::treble_w_png,
       BinaryData::treble_w_pngSize) };
    if (!Treble.isNull())
        trebleImageComponent.setImage(Treble, juce::RectanglePlacement::centred);
    else
        jassert(!Treble.isNull());

    auto Default{ juce::ImageCache::getFromMemory(BinaryData::play_png,
        BinaryData::play_pngSize) };
    if (!Default.isNull())
        defaultImageComponent.setImage(Default, juce::RectanglePlacement::centred);
    else
        jassert(!Default.isNull());

    auto ResonanceDefault{ juce::ImageCache::getFromMemory(BinaryData::play_png,
       BinaryData::play_pngSize) };
    if (!ResonanceDefault.isNull())
        defaultResonanceImageComponent.setImage(ResonanceDefault, juce::RectanglePlacement::centred);
    else
        jassert(!ResonanceDefault.isNull());


    //add image components and make them visible
    addAndMakeVisible(mImageComponent);
    addAndMakeVisible(bassImageComponent);
    addAndMakeVisible(softImageComponent);
    addAndMakeVisible(loudImageComponent);
    addAndMakeVisible(trebleImageComponent);
    addAndMakeVisible(defaultImageComponent);
    addAndMakeVisible(defaultResonanceImageComponent);

    //add other components, make them visible and set their initial states 
    //and features if necessary

    addAndMakeVisible(volumeM);
    volumeM.setFont(Vfont);

    addAndMakeVisible(volumeLabel1);
    volumeLabel1.setFont(font);
   
    addAndMakeVisible(volumeLabel2);
    volumeLabel2.setFont(font);

    addAndMakeVisible(volumeLabel3);
    volumeLabel3.setFont(font);

    addAndMakeVisible(defaultLabel);
    defaultLabel.setFont(font);

   /* addAndMakeVisible(button1);
    button1.setClickingTogglesState(true);

    addAndMakeVisible(button2);
    button2.setClickingTogglesState(true);

    addAndMakeVisible(button3);
    button3.setClickingTogglesState(true);*/

    addAndMakeVisible(_Gain);
    _Gain.setRange(-24, 0, 1);
    _Gain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    _Gain.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    _Gain.setNumDecimalPlacesToDisplay(0);

    addAndMakeVisible(middlefrequency);
    middlefrequency.setRange(250.0, 5000.0);
    middlefrequency.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    middlefrequency.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    middlefrequency.setNumDecimalPlacesToDisplay(0);

    addAndMakeVisible(freqLabel);
    freqLabel.setText("Frequency", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(resonance);
    resonance.setRange(2.0, 5.0);
    resonance.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    resonance.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    resonance.setNumDecimalPlacesToDisplay(0);

    addAndMakeVisible(resonanceLabel);
    resonanceLabel.setText("Resonance", juce::NotificationType::dontSendNotification);


    addAndMakeVisible(visualizer);

    ////create radio group for button group effect

    //button1.setRadioGroupId(volumeButtons);
    //button2.setRadioGroupId(volumeButtons);
    //button3.setRadioGroupId(volumeButtons);

    ////set on click event callback function
    //button1.onClick = [this] {setGain(button1.getButtonText()); };
    //button2.onClick = [this] {setGain(button2.getButtonText()); };
    //button3.onClick = [this] {setGain(button3.getButtonText()); };

   

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

    //set maximum block size, sample rate, number channels and last sample rate
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.sampleRate = sampleRate;
    spec.numChannels = 2;
    setLastSampleRate(sampleRate);
   

    //reset instances of the bandpass filter and the amplifier(Gain)
    BandPassFilter.reset();
    Gain.reset();

    //prepare instances of the bandpass filter and the amplifier(Gain)
    BandPassFilter.prepare(spec);
    Gain.prepare(spec);

   //clear the visualizer before next display
    visualizer.clear();
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!
    
    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)

    visualizerNumChannel = bufferToFill.buffer->getNumChannels();

    
    auto* device = deviceManager.getCurrentAudioDevice();
    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
    auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;
   
    juce::dsp::AudioBlock<float> block(bufferToFill.buffer->getArrayOfWritePointers(),
        bufferToFill.buffer->getNumChannels(),
        bufferToFill.startSample,
        bufferToFill.numSamples);
    
   
    
    // set the gain level
    Gain.setGainDecibels((float)gain);

    //get the mid frequency and resonance values and set the bandpass filter parameters
    UpdateFilter();

    //apply the bandpass filter and input amplification to the audio data.
    BandPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    Gain.process(juce::dsp::ProcessContextReplacing<float>(block));
    
    //push the processed data to the visualizer
    visualizer.pushBuffer(bufferToFill);

    for (auto channel = 0; channel < maxOutputChannels; ++channel)
    {
        


        //apply output amplification
        //Gain.process(juce::dsp::ProcessContextReplacing<float>(block));

        if ((!activeOutputChannels[channel]) || maxInputChannels == 0)
        {
            bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        else
        {
            auto actualInputChannel = channel % maxInputChannels; // [1]

            auto* inBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel,
                bufferToFill.startSample);

            auto* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);


            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                outBuffer[sample] = inBuffer[sample];

            //apply output amplification
           Gain.process(juce::dsp::ProcessContextReplacing<float>(block));
        }
        //    if (!activeInputChannels[channel]) // [2]
        //    {
        //        bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        //    }
        //    else // [3]
        //    {
        //  
        //        //apply output amplification
        //        Gain.process(juce::dsp::ProcessContextReplacing<float>(block));
        //    }
        //}
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
    float HalfParentWidth = getParentWidth()/2;
    //set bounds of the GUI components
    mImageComponent.setBounds(40, 10,70, 70 );
    trebleImageComponent.setBounds(HalfParentWidth - 145, 540, 20, 20);
    bassImageComponent.setBounds(HalfParentWidth - 90, 540, 20, 20);
    loudImageComponent.setBounds(HalfParentWidth + 135, 540, 20, 20);
    softImageComponent.setBounds(HalfParentWidth + 80, 540, 20, 20);
    visualizer.setCentreRelative(0.5f, 0.5f);
    visualizer.setBounds(HalfParentWidth - 125, 160, 250, 80);
    middlefrequency.setBounds(HalfParentWidth - 165, 390, 150, 200);
    resonance.setBounds(HalfParentWidth + 60, 390, 150, 200);
    volumeM.setBounds(HalfParentWidth - 55, 140, 300, 300);
    _Gain.setBounds(HalfParentWidth - 57.5, 260, 150, 200);
}

void MainComponent::setLastSampleRate(double sampleRate)
{
    this->sampleRate = sampleRate;
}

double MainComponent::getlastSampleRate() const
{
    return sampleRate;
}

int MainComponent::getNumInputChannels()
{
    return numInputChannels;
}

void MainComponent::setNumInputChannels(const int numInputChannels)
{
    this->numInputChannels = numInputChannels;
}



void MainComponent::setGain(const juce::String& buttonName)
{
   /* if (buttonName == "+")
        this->gain = 6.0;
    else if (buttonName == "++")
        this->gain = 12.0;
    else
        this->gain = 16.0;*/

    Gain.setGainDecibels((float)_Gain.getValue());
}

void MainComponent::UpdateFilter()
{
    //get the values of the sliders from the gui
    auto MidFreq = (float)middlefrequency.getValue();
    auto res = (float)resonance.getValue();
    
    //updates the bandpassfilter
    if(MidFreq > 0 && res > 0)
        *BandPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(getlastSampleRate(), MidFreq, res);
   
}





/*set the parameters for the GUI waveform visualizer*/
Visualizer::Visualizer() :AudioVisualiserComponent{ visualizerNumChannel }
{
    setBufferSize(1028);
    setSamplesPerBlock(256);
    setColours(juce::Colours::deepskyblue, juce::Colours::yellow);
    
}
