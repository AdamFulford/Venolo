#ifndef LED_FUNCTIONS_H
#define LED_FUNCTIONS_H

#include "arduino.h"
#include "pin_assignments.h"

void allLEDsON();
void allLEDsOFF();
void resetLEDs();
void VSSD();

extern bool LFO2_vol_set;    //multipliers to assign LFO2 to various parameters. Used in checkAssign() and DACupdate()
extern bool LFO2_pan_set;
extern bool LFO2_freq1_set;
extern bool LFO2_freq3_set;

extern bool LFO3_vol_set;   //multipliers to assign LFO3 to various parameters
extern bool LFO3_pan_set;

extern bool IS_ON;

#endif
