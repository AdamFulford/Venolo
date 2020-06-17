#ifndef TAP_TEMPO_H
#define TAP_TEMPO_H

#include "arduino.h"
#include "constants.h"


void tap_tempo();

extern long period1;
extern long period1a;
extern long period1b;
extern float swayScaled;
extern float wave1aInc;
extern float wave1bInc;
extern long t1;
extern bool direction1;

#endif
