
#include "midi_functions.h"

//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
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
    else if(number == constants::miditap){
    
      if(value > 63){    //over half
        
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

//send CC message

//MIDI.sendControlChange(number,value,channel);

Serial1.write(constants::CC_cmd + channel - 1);
Serial1.write(number);
Serial1.write(value);

}

void checkMidi()
{
  MIDI.read();
}

void ProgramFunction(byte number, byte channel){
  //MIDI.sendProgramChange(number,channel);
Serial1.write(constants::PC_cmd + channel - 1);
Serial1.write(number);
}
