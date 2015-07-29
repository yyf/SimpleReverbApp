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

#include "portaudio.h" // To write a program using PortAudio, you must include the "portaudio.h" include file.
#include "schroeder.h" // use the implementation of Schroeder reverberation using comb and all pass

using namespace std;

PaStream *stream; //audio stream

const int sr = 44100;
double dur = 30;

typedef struct {
    double mix;
    SCHROEDER * s;    
} USER_DATA; // this USER_DATA struct includes data of schroeder type and data of double type

//==============================================================================

USER_DATA udata; // init user data, which will be used in the custom_callback

// interface
int init_audio(void *data, int num_in_channels, int num_out_channels, int sr);
int close_audio();

// The "callback" is a function that is called by the PortAudio engine whenever it has captured audio data, or when it needs more audio data for output.
// !!! some systems perform the callback in a special thread, or interrupt handler, to produce glitch-free audio, make sure you avoid certain time-consuming function calls, i.e.
// memory allocation/deallocation, I/O (including file I/O as well as console I/O, such as printf()), context switching (such as exec() or yield()), mutex operations, and etc

static int custom_callback(const void *input,
                           void *output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo *timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData);

// implemnetation 
int init_audio(void *data, int num_in_channels, int num_out_channels, int sr)
{
    PaError err;
    err = Pa_Initialize(); // 1. Before making any other calls to PortAudio, you 'must' call Pa_Initialize()
    if (err != paNoError) goto error;    
    // 2. Open the (default input/output devices) stream
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

    float *in = (float *) input; // the signals are of float data type. The signals must be between -1.0 and +1.0
    float *out = (float *) output;     

    /* Cast data passed through stream to our structure. */
    // You can pass a pointer to your data structure, i.e. USER_DATA, through PortAudio which will appear as userData    
    USER_DATA *udata = (USER_DATA *) userData; 
    
    // process input and assign it to output
    for (i = 0; i < frameCount; i++) {
        double s_out = (schroeder_next(udata->s, *in) * udata->mix) + (*in++ * (1 - udata->mix)); 
        // schroeder_next function takes a shoroeder data type and a double, and returns a double
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
    udata.s = schroeder_create(sr, 3.5); 
    // the schroeder_create function returns a pointer, assign it to udata.s
    // the schroeder_create takes sampling rate and rt60 as input arguments
    // rt60, reverberation time, is the time required for the reflections of a direct sound to decay to 60dB
    udata.mix = 0.5; // assign the mix variable a value of 0.5
    
    // gui setup
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
    // clean up
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
    // 
}

void MainContentComponent::buttonClicked (Button* button)
{
    if (button == startButton){
        std::cout<< "startButton clicked" << std::endl;
        init_audio((void *) &udata, 1, 2, sr);
    }
    
    else if (button == stopButton){
        std::cout<< "stopButton clicked" << std::endl;
        close_audio();
    }
}

