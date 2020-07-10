#ifndef TAP_TEMPO_H
#define TAP_TEMPO_H

#include "arduino.h"
#include "constants.h"
#include "midi_functions.h"
#include "freq_update.h"

void tap_tempo();

extern int tapSpeed; //multiplier for tap frequency
extern int freqMode;

#endif
