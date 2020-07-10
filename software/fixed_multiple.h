#ifndef FIXED_MULTIPLE_H
#define FIXED_MULTIPLE_H

#include "arduino.h"
#include "constants.h"
#include "pin_assignments.h"
#include "LED_functions.h"
#include "freq_update.h"

void change_Mode();
int Scale(int value, int range, float a, float b);

void toggleEngage();

extern int LFO1aMult;
extern int LFO1aDiv;

extern int LFO1bMult;
extern int LFO1bDiv;

extern bool IS_ON;

extern float global_mult;
extern int freqMode;

#endif
