/*
 ==============================================================================
 
 This file is from http://www.mat.ucsb.edu/240/D/index.html by Dr. Andrés Cabrera
 
 ==============================================================================
 */

#ifndef SCHROEDER_H
#define SCHROEDER_H

#include "math.h"

#include "comb.h"
#include "allpass.h"

typedef struct {
    COMB *c1, *c2, *c3, *c4;
    ALLPASS *a1, *a2;
} SCHROEDER; // 


SCHROEDER *schroeder_create(int sr, double rt60)
{
    SCHROEDER *s = (SCHROEDER *) malloc(sizeof(SCHROEDER));


    double comb_gain = pow(10.,((-3.0 * 29.7 )/(rt60 * 1000.0)));
    s->c1 = comb_create(sr, 29.7, comb_gain);
    comb_gain = pow(10.,((-3.0 * 37.1 )/(rt60 * 1000.0)));
    s->c2 = comb_create(sr, 37.1, comb_gain);
    comb_gain = pow(10.,((-3.0 * 41.1 )/(rt60 * 1000.0)));
    s->c3 = comb_create(sr, 41.1, comb_gain);
    comb_gain = pow(10.,((-3.0 * 43.7 )/(rt60 * 1000.0)));
    s->c4 = comb_create(sr, 43.7, comb_gain);

    s->a1 = allpass_create(sr, 5, 0.7);
    s->a2 = allpass_create(sr, 1.7, 0.7);

    return s;
}

double schroeder_next(SCHROEDER *s, double in)
{
    double out = comb_next(s->c1, in) + comb_next(s->c2, in)
            + comb_next(s->c3,in) + comb_next(s->c4,in);
    out = allpass_next(s->a2, allpass_next(s->a1, out) );

    return out * 0.1;
}

void schroeder_free(SCHROEDER *s)
{
    comb_free(s->c1);
    comb_free(s->c2);
    comb_free(s->c3);
    comb_free(s->c4);
    allpass_free(s->a1);
    allpass_free(s->a2);
    free(s);
}


#endif // SCHROEDER_H
