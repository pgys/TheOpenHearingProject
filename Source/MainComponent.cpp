#include "MainComponent.h"
#include <JuceHeader.h>

/** Global variable for the visualizer constructor */
std::atomic<int> visualizerNumChannel{ 2 };
//==============================================================================
MainComponent::MainComponent():BandPassFilter(juce::dsp::IIR::Coefficients<float>::makeBandPass(44100, 20000, 0.1f)), gain{-24.0}
{
    // Make sure you set the size of the component after
    // you add any child components.
    
    juce::LookAndFeel::setDefaultLookAndFeel(&customLNF);
    setSize (400, 400);


    //get image from binary dataand set image component
    
    decltype(auto) Logo{ juce::ImageCache::getFromMemory(BinaryData::tohp_logo_pngCopy1_png,
        BinaryData::tohp_logo_pngCopy1_pngSize) };
    if (!Logo.isNull())
        mImageComponent.setImage(Logo, juce::RectanglePlacement::centred);
    else
        jassert(!Logo.isNull());

    decltype(auto) Bass{ juce::ImageCache::getFromMemory(BinaryData::bass_w_png,
        BinaryData::bass_w_pngSize) };
    if (!Bass.isNull())
        bassImageComponent.setImage(Bass, juce::RectanglePlacement::centred);
    else
        jassert(!Bass.isNull());

    decltype(auto) Soft{ juce::ImageCache::getFromMemory(BinaryData::soft_w_png,
        BinaryData::soft_w_pngSize) };
    if (!Soft.isNull())
        softImageComponent.setImage(Soft, juce::RectanglePlacement::centred);
    else
        jassert(!Soft.isNull());

    decltype(auto) Loud{ juce::ImageCache::getFromMemory(BinaryData::loud_w_png,
        BinaryData::loud_w_pngSize) };
    if (!Loud.isNull())
        loudImageComponent.setImage(Loud, juce::RectanglePlacement::centred);
    else
        jassert(!Loud.isNull());

    decltype(auto) Treble{ juce::ImageCache::getFromMemory(BinaryData::treble_w_png,
       BinaryData::treble_w_pngSize) };
    if (!Treble.isNull())
        trebleImageComponent.setImage(Treble, juce::RectanglePlacement::centred);
    else
        jassert(!Treble.isNull());

    decltype(auto) Default{ juce::ImageCache::getFromMemory(BinaryData::play_png,
        BinaryData::play_pngSize) };
    if (!Default.isNull())
        defaultImageComponent.setImage(Default, juce::RectanglePlacement::centred);
    else
        jassert(!Default.isNull());

    decltype(auto) qualityFactorDefault{ juce::ImageCache::getFromMemory(BinaryData::play_png,
       BinaryData::play_pngSize) };
    if (!qualityFactorDefault.isNull())
        defaultqualityFactorImageComponent.setImage(qualityFactorDefault, juce::RectanglePlacement::centred);
    else
        jassert(!qualityFactorDefault.isNull());


    //add image components and make them visible
    addAndMakeVisible(mImageComponent);
    addAndMakeVisible(bassImageComponent);
    addAndMakeVisible(softImageComponent);
    addAndMakeVisible(loudImageComponent);
    addAndMakeVisible(trebleImageComponent);
    addAndMakeVisible(defaultImageComponent);
    addAndMakeVisible(defaultqualityFactorImageComponent);

    //add other components, make them visible and set their initial states 
    //and features if necessary

    addAndMakeVisible(gainDescLabel);
    gainDescLabel.setFont(Vfont);

    addAndMakeVisible(timbre);
    timbre.setFont(Vfont);

    addAndMakeVisible(gainUnitLabel);
    gainUnitLabel.setFont(font);
   
    addAndMakeVisible(volumeLabel2);
    volumeLabel2.setFont(font);

    addAndMakeVisible(volumeLabel3);
    volumeLabel3.setFont(font);

    addAndMakeVisible(defaultLabel);
    defaultLabel.setFont(font);


    addAndMakeVisible(_Gain);
    _Gain.setRange(0.0, 6.0, 1.0);
    _Gain.setValue(-24, juce::NotificationType::dontSendNotification);
    _Gain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    _Gain.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    _Gain.setNumDecimalPlacesToDisplay(0);
    _Gain.onValueChange = [this]() {gain = _Gain.getValue(); };
    addAndMakeVisible(cutoffFrequency);
    cutoffFrequency.setRange(250.0, 5000.0);
    cutoffFrequency.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    cutoffFrequency.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    cutoffFrequency.setNumDecimalPlacesToDisplay(0);
  

    addAndMakeVisible(freqLabel);
    freqLabel.setText("Hz", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(qualityFactor);
    qualityFactor.setRange(0.1, 10.0);
    qualityFactor.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    qualityFactor.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    qualityFactor.setNumDecimalPlacesToDisplay(0);

    addAndMakeVisible(qualityFactorLabel);
    qualityFactorLabel.setText("Quality Factor", juce::NotificationType::dontSendNotification);


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

    //set maximum block size, sample rate, number channels and last sample rate
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.sampleRate = sampleRate;
    spec.numChannels = 2;
    setLastSampleRate(sampleRate);
   

    //reset Filters and the amplifier(Gain)
    LowShelfFilter.reset();
    HighShelfFilter.reset();
    Gain.reset();

    //prepare instances of the bandpass filter and the amplifier(Gain)
    LowShelfFilter.prepare(spec);
    HighShelfFilter.prepare(spec);
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

    //get the mid frequency and qualityFactor values and set the bandpass filter parameters
    UpdateFilter();

    //apply the bandpass filter and input amplification to the audio data.
    LowShelfFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    HighShelfFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
   
    //push the processed data to the visualizer
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

            auto* inBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel,
                bufferToFill.startSample);

            auto* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);


            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                outBuffer[sample] = inBuffer[sample] ;

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
    float HalfParentWidth = getParentWidth()/2;
    //set bounds of the GUI components
    mImageComponent.setBounds(40, 10,70, 70 );
    trebleImageComponent.setBounds(HalfParentWidth - 90, 590, 20, 20);
    bassImageComponent.setBounds(HalfParentWidth - 145, 590, 20, 20);
    loudImageComponent.setBounds(HalfParentWidth + 105, 590, 20, 20);
    softImageComponent.setBounds(HalfParentWidth + 50, 590, 20, 20);
    visualizer.setCentreRelative(0.5f, 0.5f);
    visualizer.setBounds(HalfParentWidth - 125, 150, 250, 80);
    cutoffFrequency.setBounds(HalfParentWidth - 165, 440, 150, 200);
    gainDescLabel.setBounds(HalfParentWidth - 15, 270, 200, 400);
    gainUnitLabel.setBounds(HalfParentWidth + 180, 520, 30, 40);
    timbre.setBounds(HalfParentWidth - 160, 270, 200, 400);
    _Gain.setBounds(HalfParentWidth + 30, 440, 150, 200);
    freqLabel.setBounds(HalfParentWidth - 15, 520, 30, 40);
}

void MainComponent::setLastSampleRate(double _sampleRate)const
{
    sampleRate = _sampleRate;
}

const double MainComponent::getlastSampleRate()
{
    return sampleRate;
}



void MainComponent::setGain(const juce::String& buttonName)
{
    Gain.setGainDecibels((float)_Gain.getValue());
}

void MainComponent::UpdateFilter()
{
    //get the values of the sliders from the gui
    float cutoffFreq = std::clamp<float>((float)cutoffFrequency.getValue(), minFrequency, maxFrequency/2); 
    
    //updates the lowshelf and high shelf filters
    *LowShelfFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(getlastSampleRate(), cutoffFreq * 2.f, qFactorVal, juce::Decibels::decibelsToGain(gain));
    *HighShelfFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(getlastSampleRate(), cutoffFreq, qFactorVal, juce::Decibels::decibelsToGain(gain));
   
}





/*set the parameters for the GUI waveform visualizer*/
Visualizer::Visualizer() :AudioVisualiserComponent{ visualizerNumChannel }
{
    setBufferSize(1028);
    setSamplesPerBlock(256);
    setColours(juce::Colours::deepskyblue, juce::Colours::yellow);
    
}
