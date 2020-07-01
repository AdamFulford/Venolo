#ifndef TAP_TEMPO_H
#define TAP_TEMPO_H

#include "arduino.h"
#include "constants.h"
#include "midi_functions.h"
#include "freq_update.h"

void tap_tempo();

extern long period1;
extern long period1a;
extern long period1b;
extern long period2;
extern long period3;
extern float swayScaled;
extern float wave1aInc;
extern float wave1bInc;
extern float wave2Inc;
extern float wave3Inc;
extern long t1;
extern long t2;
extern long t3;
extern bool direction1;
extern bool direction2;
extern bool direction3;
extern int tapSpeed; //multiplier for tap frequency
extern int freqMode;
extern float ratio2;
extern float ratio3;

#endif
