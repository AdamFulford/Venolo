#ifndef MIDI_FUNCTIONS_H
#define MIDI_FUNCTIONS_H


#include "arduino.h"
#include "constants.h"
#include <MIDI.h>
#include "tap_tempo.h"

void CCFunction(byte channel, byte number, byte value);
void ProgramFunction(byte number, byte channel);
void MidiInit();
void checkMidi();


extern int midi_matrix[9][4];
extern int tapSpeed;


#endif
