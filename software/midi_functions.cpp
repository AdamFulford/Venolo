
#include "midi_functions.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void MidiInit()
{
MIDI.begin(MIDI_CHANNEL_OMNI);
MIDI.setHandleControlChange(CCFunction);    //byte channel, byte number, byte value
MIDI.setHandleProgramChange(ProgramFunction);   //byte channel, byte number
}

void CCFunction(byte channel, byte number, byte value){



if(channel == constants::midichan){

int midiexp{};

    if(number == constants::midiexpCC){
    
    
            midiexp = map(value,0,127,0,constants::in_max);
            for (int i=0; i < 9; i++){
    
                if(midi_matrix[i][0] == midi_matrix[i][1]){   //if min and max the same, i.e. parameter NOT set
                  //skip
                }
                else{     // parameter set
                  midi_matrix[i][2] = map(midiexp,0,constants::in_max,midi_matrix[i][0],midi_matrix[i][1]);
                }
            }
    
    }
    if(number == constants::miditap){
    
      if(value > 63){    //over half
       tapSpeed = 1;
       tap_tempo();
      }
                      
      if(value <= 63){    //under half
        //do nothing
  
      }
    }

    else if(number == constants::miditap_dbl){

      if(value > 63){    //over half
       tapSpeed = 2;
       tap_tempo();
      }
                      
      if(value <= 63){    //under half
        //do nothing
      }
    }
 
    
else{
      //do nothing
    }
}

else{   //different midi channel
  //do nothing
}

}

void ProgramFunction(byte number, byte channel){

}

void checkMidi()
{
  
MIDI.read();

}
