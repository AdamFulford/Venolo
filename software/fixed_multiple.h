#ifndef FIXED_MULTIPLE_H
#define FIXED_MULTIPLE_H

#include "arduino.h"
#include "constants.h"
#include "pin_assignments.h"
#include "LED_functions.h"
#include "freq_update.h"

int changeMode(int currentmode);
int Scale(int value, int range, float a, float b);

void toggleEngage();

extern long t1;
extern long t2;
extern long t3;

extern bool direction1;
extern bool direction2;
extern bool direction3;

extern int LFO1aMult;
extern int LFO1aDiv;

extern int LFO1bMult;
extern int LFO1bDiv;

extern bool IS_ON;

extern float global_mult;

#endif
