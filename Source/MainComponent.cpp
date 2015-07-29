/*
 ==============================================================================
 
 This file was auto-generated! 
 It's modified from MAT 240D examples and added with JUCE GUI
 The link to MAT 240D: http://www.mat.ucsb.edu/240/D/index.html
 
 ==============================================================================
 */

#include "MainComponent.h"
#include <stdio.h>
#include <iostream>
#include "portaudio.h" // use portaudio 
#include "schroeder.h" // use implementation of Schroeder reverberation using comb and all pass

using namespace std;

PaStream *stream; //audio stream

const int sr = 44100;
double dur = 30;

typedef struct {
    double mix;
    SCHROEDER *s;    
} USER_DATA; // this USER_DATA struct includes data of schroeder type and data of double

//==============================================================================

USER_DATA udata; // init user data, which will be used in the custom_callback

static int custom_callback(const void *input,
                           void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo *timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData);


int init_audio(void *data, int num_in_channels, int num_out_channels, int sr)
{
    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) goto error;
    
    err = Pa_OpenDefaultStream(&stream, num_in_channels, num_out_channels, paFloat32, sr,
                               paFramesPerBufferUnspecified,
                               custom_callback,
                               data);
    if (err != paNoError) goto error;
    
    err = Pa_StartStream(stream);
    if (err != paNoError) goto error;
    return 0;
error:
    printf("Portaudio error: %s\n", Pa_GetErrorText(err));
    return err;
}

int close_audio()
{
    PaError err;
    err = Pa_StopStream(stream);
    if (err != paNoError) goto error;
    err = Pa_CloseStream(stream);
    if (err != paNoError) goto error;
    err = Pa_Terminate();
    if (err != paNoError) goto error;
    return 0;
error:
    printf("Portaudio error: %s\n", Pa_GetErrorText(err));
    return err;
}


static int custom_callback(const void *input,
                       void *output,
                       unsigned long frameCount,
                       const PaStreamCallbackTimeInfo *timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData)
{
    int i;
    
    float *in = (float *) input;
    float *out = (float *) output;

    USER_DATA *udata = (USER_DATA *) userData; 
    
    for (i = 0; i < frameCount; i++) {
        double s_out = (schroeder_next(udata->s, *in) * udata->mix) + (*in++ * (1 - udata->mix)); // schroeder_next function takes a shoroeder data type and a double, and returns a double
        *out++ = s_out;
        *out++ = s_out;
    }
    
    return 0;
}

//==============================================================================
MainContentComponent::MainContentComponent()
{
    // windown size
    setSize (600, 400);
    
    // init
    udata.s = schroeder_create(sr, 3.5); // create the schroeder data and assign it to udata in the constructor
    udata.mix = 0.5; // assign a mix value to the
    
    // gui
    startButton = new TextButton ("To test reverb");
    stopButton = new TextButton ("To stop reverb");
    addAndMakeVisible (startButton);
    addAndMakeVisible (stopButton);
    startButton->setBounds (50, 170, 120, 60);
    stopButton->setBounds (430, 170, 120, 60);
    startButton->addListener(this);
    stopButton->addListener(this);
}

MainContentComponent::~MainContentComponent()
{
    startButton = nullptr;
    stopButton = nullptr;
	Pa_Terminate();
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
    g.setFont (Font (24.0f));
    g.setColour (Colours::yellowgreen);
    g.drawText ("Simple Reverb App", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{

}


void MainContentComponent::buttonClicked (Button* button)
{

    if (button == startButton){
        std::cout<< "startButton clicked" << std::endl;
        init_audio((void *) &udata, 1, 2, sr);
    }
    
    else if (button == stopButton){
        close_audio();
    }
}

