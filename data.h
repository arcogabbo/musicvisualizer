#ifndef DATA_H
#define DATA_H

#include <fftw3.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "color.h"
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>

#define WIDTH 800
#define HEIGHT 600
#define SAMPLE_RATE 44100
#define BARS 60
#define THICKNESS 15
#define DISTANCE 16
#define FIT_FACTOR 30

struct AudioData
{
    Uint8* position;
    Uint32 length;
    SDL_AudioFormat format;
    fftw_plan plan;
    fftw_complex *in;
    fftw_complex *out;
    SDL_Renderer *renderer;
    struct RGB* color; 
};

struct wrapper
{
    Uint8* stream;
    struct AudioData* audio;
};

sem_t play;
//no MUTEX on this int because only data.c->visualizerOutput uses it;
int NSAMPLES;

void myCallback(void*,Uint8*,int);
double Get16bitAudioSample(Uint8*,SDL_AudioFormat);
void* visualizerOutput(void*);
#endif
