#ifndef FREQ_UPDATE_H
#define FREQ_UPDATE_H

#include "arduino.h"
#include "constants.h"

void updatefreq1(float freq1scaled);
void updatefreq2(float freq2scaled);
void updatefreq3(float freq3scaled);

void updatefreq1Mode1(float freq1scaled);
void updatefreq2Mode1(int freq2);
void updatefreq3Mode1(int freq3);

void resetPhase();

extern void updateInterval();


extern long period1;   //used in checkFreq() and checkSway() and checkTap()
extern long period1a;  //period going up used in checkFreq() and checkSway() checkTap() and DACupdate()
extern long period1b;  //period going down used in checkFreq() and checkSway() checkTap() and DACupdate()
extern long period2;   //used in checkFreq() and DACupdate()
extern long period3;   //used in checkFreq() and DACupdate()

extern float wave1aInc;  //increment up used in checkFreq(), checkSway(), checkTap() and DACupdate()
extern float wave1bInc;  //increment down used in checkFreq(), checkSway(), checkTap() and DACupdate()
extern float wave2Inc;
extern float wave3Inc; 

extern bool direction1;
extern bool direction2;
extern bool direction3;

extern int freqMode;  //0 = normal, 1 = fixed multiple.

//extern float ratio2;  //ratio of freq2 / freq1 used in fixed multiple mode
//extern float ratio3;  //ratio of freq3 / freq1 used in fixed multiple mode



extern int LFO2Mult;
extern int LFO2Div;

extern int LFO3Mult;
extern int LFO3Div;

extern int Mode1Period;
extern float swayScaled;
extern float global_mult;

extern int LFO2_offset;
extern int LFO3_offset;

extern long t;
extern long t1;
extern long t2;
extern long t3;

#endif
