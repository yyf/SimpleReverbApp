/*
 ==============================================================================
 
 This file is from http://www.mat.ucsb.edu/240/D/index.html by Dr. Andrés Cabrera 
 
 ==============================================================================
 */


#ifndef ALLPASS_H
#define ALLPASS_H

#include <stdlib.h>
#include <string.h>

#define MAX_DELAY 8192

typedef struct {

    double gain;
    int read_pos, write_pos;
    double buffer[MAX_DELAY];
} ALLPASS;


ALLPASS *allpass_create(int sr, double delay_time_ms, double gain)
{
    ALLPASS *c= (ALLPASS *) malloc(sizeof(ALLPASS));
    memset(c->buffer, 0, MAX_DELAY*sizeof(double));
    c->gain = gain;
    c->read_pos = 0;
    c->write_pos = sr * delay_time_ms / 1000.0;
    if (c->write_pos >= MAX_DELAY) {
        c->write_pos = MAX_DELAY - 1; /* Should print some warning */
    }

    return c;
}

double allpass_next(ALLPASS *c, double in)
{
    double out = c->buffer[c->read_pos++];
    if(c->read_pos == MAX_DELAY) {
        c->read_pos = 0;
    }

    c->buffer[c->write_pos++] = in + (c->gain * out);
    if(c->write_pos == MAX_DELAY) {
        c->write_pos = 0;
    }

    return out + (in * -c->gain);
}

void allpass_free(ALLPASS *c)
{
    free(c);
}


#endif
