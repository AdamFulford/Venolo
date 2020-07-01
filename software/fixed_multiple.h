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

extern float ratio2;
extern float ratio3;

extern bool IS_ON;

#endif
