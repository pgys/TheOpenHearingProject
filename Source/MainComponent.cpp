#include "MainComponent.h"
#include <JuceHeader.h>

/** Global variable for the visualizer constructor */
std::atomic<int> visualizerNumChannel{ 0 };
//==============================================================================
MainComponent::MainComponent() :inputGainVar{ -24.0 }, shelfGain{ 0.0 }
{
    // Make sure you set the size of the component after
    // you add any child components.

    juce::LookAndFeel::setDefaultLookAndFeel(&customLNF);
    setSize(400, 400);


    //get image from binary dataand set image component

    decltype(auto) Logo{ juce::ImageCache::getFromMemory(BinaryData::tohp_logo_pngCopy1_png,
        BinaryData::tohp_logo_pngCopy1_pngSize) };
    if (!Logo.isNull())
        logoImageComponent.setImage(Logo, juce::RectanglePlacement::centred);
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
    addAndMakeVisible(logoImageComponent);
    addAndMakeVisible(bassImageComponent);
    addAndMakeVisible(softImageComponent);
    addAndMakeVisible(loudImageComponent);
    addAndMakeVisible(trebleImageComponent);
    addAndMakeVisible(defaultImageComponent);
    addAndMakeVisible(defaultqualityFactorImageComponent);

    //add other components, make them visible and set their initial states 
    //and features if necessary

    addAndMakeVisible(shelfGainDescLabel);
    shelfGainDescLabel.setFont(Vfont);

    addAndMakeVisible(highShelfCutoffhighShelfFreqUnitLabel);
    highShelfCutoffhighShelfFreqUnitLabel.setFont(Vfont);

    addAndMakeVisible(shelfGainUnitLabel);
    shelfGainUnitLabel.setFont(font);

    addAndMakeVisible(inputGainUnitLabel);
    inputGainUnitLabel.setFont(font);

    addAndMakeVisible(inputGainLabel);
    inputGainLabel.setFont(Vfont);

    addAndMakeVisible(headsetWarning);
    headsetWarning.setFont(font);


    addAndMakeVisible(shelfFiltersGain);
    shelfFiltersGain.setRange(0.0, 6.0, 1.0);
    shelfFiltersGain.setValue(-24, juce::NotificationType::dontSendNotification);
    shelfFiltersGain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    shelfFiltersGain.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    shelfFiltersGain.setNumDecimalPlacesToDisplay(0);
    shelfFiltersGain.onValueChange = [this]() {shelfGain = shelfFiltersGain.getValue(); };


    addAndMakeVisible(inputGain);
    inputGain.setRange(0.0, 6.0, 1.0);
    inputGain.setValue(-24, juce::NotificationType::dontSendNotification);
    inputGain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    inputGain.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    inputGain.setNumDecimalPlacesToDisplay(0);
    inputGain.onValueChange = [this]() {inputGainVar = inputGain.getValue(); };

    addAndMakeVisible(highShelfCutoffFrequency);
    highShelfCutoffFrequency.setRange(250.0, 5000.0);
    highShelfCutoffFrequency.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    highShelfCutoffFrequency.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    highShelfCutoffFrequency.setNumDecimalPlacesToDisplay(0);


    addAndMakeVisible(highShelfFreqUnitLabel);
    highShelfFreqUnitLabel.setText("Hz", juce::NotificationType::dontSendNotification);

    addAndMakeVisible(qualityFactor);
    qualityFactor.setRange(0.1, 10.0);
    qualityFactor.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    qualityFactor.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    qualityFactor.setNumDecimalPlacesToDisplay(0);

    addAndMakeVisible(qualityFactorLabel);
    qualityFactorLabel.setText("Quality Factor", juce::NotificationType::dontSendNotification);


    
    addAndMakeVisible(visualizer);


    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(2, 2);
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
    //get device number of channels

    auto* device = deviceManager.getCurrentAudioDevice();
    visualizerNumChannel = device->getActiveInputChannels().toInteger();
    visualizer.setNumChannels(visualizerNumChannel);

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
    GainInstance.reset();

    //prepare instances of the bandpass filter and the amplifier(Gain)
    LowShelfFilter.prepare(spec);
    HighShelfFilter.prepare(spec);
    GainInstance.prepare(spec);

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
    GainInstance.setGainDecibels((float)inputGainVar);

    //get the mid frequency and qualityFactor values and set the bandpass filter parameters
    UpdateFilter();

    //apply the bandpass filter and input amplification to the audio data.
    LowShelfFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    HighShelfFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    GainInstance.process(juce::dsp::ProcessContextReplacing<float>(block));

    //push the processed data to the visualizer if gain > 0.0
    if (inputGainVar > 0.0f)
        visualizer.pushBuffer(bufferToFill);
    else
        visualizer.clear();



    for (auto channel = 0; channel < maxOutputChannels; ++channel)
    {
        if (inputGainVar <= 0) {
            bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        }

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

    juce::Rectangle<int> area = getLocalBounds();
    
    area.removeFromTop(10);

    
    const int componentWidth = 200;
    const int componentHeight = 200;
    float HalfParentWidth = getParentWidth()/2;
    //set bounds of the GUI components
    auto logoArea = area.removeFromLeft(area.getWidth() * 1.f / 5.f).removeFromTop(area.getHeight() * 1.f / 8.f);
    auto headphoneWarningLabel = area.removeFromBottom(area.getHeight() *1.f/7.f);
    auto subArea = area.removeFromLeft(area.getWidth() * 3.f / 4.f).removeFromBottom(area.getHeight() * 5.f / 6.f);
    auto visualizerArea = subArea.removeFromTop(subArea.getHeight() * 1.f / 6.f);
    //auto inputGainSliderLabelArea = subArea.removeFromTop(subArea.getHeight() * 1.f / 5.f);
    //auto inputGainSliderArea = subArea.removeFromTop(subArea.getHeight() * 1.f / 4.f);
    //auto shelfGainandhighShelfCutoffFrequencyLabelArea = subArea.removeFromTop(subArea.getHeight() * 1.f / 3.f);
    //auto shelfGainandhighShelfCutoffFrequencySliderArea = subArea.removeFromTop(subArea.getHeight() * 1.f / 2.f);
    auto highShelfCutoffFrequencyArea = subArea.removeFromLeft(subArea.getWidth() * 1.f / 3.f).removeFromTop(subArea.getHeight()* 4.f/5.f);
    auto inputGainArea = subArea.removeFromLeft(subArea.getWidth() * 1.f / 2.f).removeFromTop(subArea.getHeight() * 4.f / 5.f);
    auto shelfGainArea = subArea.removeFromLeft(subArea.getWidth()).removeFromTop(subArea.getHeight() * 4.f / 5.f);
    

    
    logoImageComponent.setBounds(logoArea);
    visualizer.setBounds(visualizerArea.removeFromTop(visualizerArea.getHeight() * 8.f / 9.f));
    inputGainLabel.setBounds(inputGainArea.removeFromTop(inputGainArea.getHeight() * 1.f/4.f).removeFromRight(componentWidth));
    inputGain.setBounds(inputGainArea.removeFromTop(inputGainArea.getHeight() * 1.f / 3.f));
    highShelfCutoffFrequency.setBounds(highShelfCutoffFrequencyArea.removeFromBottom(highShelfCutoffFrequencyArea.getHeight() * 1.f / 4.f));
    highShelfCutoffhighShelfFreqUnitLabel.setBounds(highShelfCutoffFrequencyArea.removeFromBottom(highShelfCutoffFrequencyArea.getHeight() * 1.f / 3.f).removeFromRight(componentWidth - 35));
    shelfFiltersGain.setBounds(shelfGainArea.removeFromBottom(shelfGainArea.getHeight() * 1.f / 4.f));
    shelfGainDescLabel.setBounds(shelfGainArea.removeFromBottom(shelfGainArea.getHeight() *1.f/3.f).removeFromRight(componentWidth));
    bassImageComponent.setBounds( highShelfCutoffFrequency.getX() + highShelfCutoffFrequency.getWidth()/3.0f, highShelfCutoffFrequency.getBottom() + 5.f, 15.f, 15.f);
    trebleImageComponent.setBounds(bassImageComponent.getX() + 35.f, bassImageComponent.getY(), 15.f ,15.f);
    softImageComponent.setBounds(shelfFiltersGain.getX() + shelfFiltersGain.getWidth() / 3.0f, shelfFiltersGain.getBottom() + 5.f, 15.f, 15.f);
    loudImageComponent.setBounds(softImageComponent.getX() + 37.f, softImageComponent.getY(), 15.f, 15.f);
    headsetWarning.setBounds(headphoneWarningLabel.removeFromTop(headphoneWarningLabel.getHeight() * 4.f / 4.f));
    shelfGainUnitLabel.setBounds(shelfFiltersGain.getX() + shelfFiltersGain.getWidth(), shelfFiltersGain.getY() + shelfFiltersGain.getHeight()/2.2 - shelfFiltersGain.getTextBoxHeight()/2, 30.f, 30.f);
    inputGainUnitLabel.setBounds(inputGain.getX() + inputGain.getWidth(), inputGain.getY() + inputGain.getHeight() / 2.2 - inputGain.getTextBoxHeight() / 2, 30.f, 30.f);
    highShelfFreqUnitLabel.setBounds(highShelfCutoffFrequency.getX() + highShelfCutoffFrequency.getWidth(), highShelfCutoffFrequency.getY() + highShelfCutoffFrequency.getHeight() / 2.5 - highShelfCutoffFrequency.getTextBoxHeight() / 2, 40.f, 40.f);

   /* inputGainLabel.setBounds(inputGainSliderLabelArea.removeFromRight(inputGainSliderLabelArea.getWidth()/2 + inputGainLabel.getWidth()/2));
    inputGain.setBounds(inputGainSliderArea.removeFromRight(inputGainSliderArea.getWidth() * 1.7f / 3.f).removeFromLeft(inputGainSliderArea.getWidth() * 1.f / 3.f));*/
    ////logoImageComponent.setBounds(40, 10, 70, 70);
    //
    //trebleImageComponent.setBounds(HalfParentWidth - 90, 590, 20, 20);
    //bassImageComponent.setBounds(HalfParentWidth - 145, 590, 20, 20);
    //loudImageComponent.setBounds(HalfParentWidth + 105, 590, 20, 20);
    //softImageComponent.setBounds(HalfParentWidth + 50, 590, 20, 20);
    ////visualizer.setCentreRelative(0.5f, 0.5f);
   
    ////visualizer.setBounds(HalfParentWidth - 125, 150, 250, 80);
    //highShelfCutoffFrequency.setBounds(SliderBounds);
    ////highShelfCutoffFrequency.setBounds(HalfParentWidth - 165, 440, 150, 200);
    //shelfGainDescLabel.setBounds(HalfParentWidth - 5, 320, 200, 300);
    //shelfGainUnitLabel.setBounds(HalfParentWidth + 180, 520, 30, 40);
    //highShelfCutoffhighShelfFreqUnitLabel.setBounds(HalfParentWidth - 160, 320, 200, 300);
    //shelfFiltersGain.setBounds(SliderBounds);
    ////shelfFiltersGain.setBounds(HalfParentWidth + 30, 440, 150, 200);
    //highShelfFreqUnitLabel.setBounds(HalfParentWidth - 15, 520, 30, 40);
    //InputGain.setBounds(SliderBounds);
    ////InputGain.setBounds(HalfParentWidth - (InputGain.getWidth() / 2 - 15), 300, 150, 100);
    //inputGainLabel.setBounds(HalfParentWidth - (inputGainLabel.getWidth() / 2), 230, 200, 100);
    //inputGainUnitLabel.setBounds(HalfParentWidth - (InputGain.getWidth() / 2 - 15) + 150, 330, 40, 40);
}

void MainComponent::setLastSampleRate(double _sampleRate)const
{
    sampleRate = _sampleRate;
}

const double MainComponent::getlastSampleRate()
{
    return sampleRate;
}

void MainComponent::UpdateFilter()
{
    //get the values of the sliders from the gui
    float cutoffFreq = std::clamp<float>((float)highShelfCutoffFrequency.getValue(), minFrequency, maxFrequency/2); 
    


    //updates the lowshelf and high shelf filters
    *LowShelfFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(getlastSampleRate(), cutoffFreq * 2.f, qFactorVal, juce::Decibels::decibelsToGain(shelfGain));
    *HighShelfFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(getlastSampleRate(), cutoffFreq, qFactorVal, juce::Decibels::decibelsToGain(shelfGain));
   
}





/*set the parameters for the GUI waveform visualizer*/
Visualizer::Visualizer() :AudioVisualiserComponent{ visualizerNumChannel }
{
    setBufferSize(1028);
    setSamplesPerBlock(256);
    setColours(juce::Colours::deepskyblue, juce::Colours::yellow);
    
}
