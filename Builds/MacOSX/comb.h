/*
 ==============================================================================
 
 This file is from http://www.mat.ucsb.edu/240/D/index.html by Dr. Andrés Cabrera
 
 ==============================================================================
 */

#ifndef COMB_H
#define COMB_H

#include <stdlib.h>
#include <string.h>

#define MAX_DELAY 8192

typedef struct {

    double gain;
    int read_pos, write_pos;
    double buffer[MAX_DELAY];
} COMB;


COMB *comb_create(int sr, double delay_time_ms, double gain)
{
    COMB *c= (COMB *) malloc(sizeof(COMB));
    memset(c->buffer, 0, MAX_DELAY*sizeof(double));
    c->gain = gain;
    c->read_pos = 0;
    c->write_pos = sr * delay_time_ms / 1000.0;

    return c;
}

double comb_next(COMB *c, double in)
{
    double out = c->buffer[c->read_pos++];
    if(c->read_pos == MAX_DELAY) {
        c->read_pos = 0;
    }

    c->buffer[c->write_pos++] = in + (c->gain * out);
    if(c->write_pos == MAX_DELAY) {
        c->write_pos = 0;
    }

    return out;
}

void comb_free(COMB *c)
{
    free(c);
}


#endif
