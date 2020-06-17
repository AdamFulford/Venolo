#include "panning.h"

uint16_t panAdjust(uint16_t pan_input)
{
  return pgm_read_word_near(constants::panLookup + pan_input);
  //return pan_input;
}
