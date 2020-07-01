char junk;
//Asymmetric tremolo 
#include <SPI.h>
#include <Bounce2.h>
#include <MCP492X.h>
#include "constants.h"
#include "pin_assignments.h"
#include "midi_functions.h"
#include "tap_tempo.h"
#include "panning.h"
#include "fixed_multiple.h"
#include "LED_functions.h"
#include "freq_update.h"

MCP492X myDac(10);
IntervalTimer myTimer;

//setup debouncing for buttons:
Bounce assign2Button = Bounce();
Bounce assign3Button = Bounce();
Bounce assignMidiButton = Bounce();
Bounce engageButton = Bounce();
Bounce TAP = Bounce();

//global variables
int sway{}; //used in checkSway() only
float swayScaled{}; //used in checkSway() and checkTap() (and midi tap currently)

bool direction1{constants::up}; // flags for wave direction. Used in tap_tempo() and DACupdate()
bool direction2{constants::up};
bool direction3{constants::up}; 

int amp1{}; //used in checkAmp() amd DACupdate()
float amp1_a{}; //non-linear constants used in checkAmp() only
float amp1_b{};  //non-linear constants used in checkAmp() only

int amp2{}; //used in checkAmp() amd DACupdate()
float amp2_a{}; //non-linear constants used in checkAmp() and checkAssign()
float amp2_b{}; //non-linear constants used in checkAmp() and checkAssign()

int amp3{}; //used in checkAmp() amd DACupdate()
float amp3_a{}; //non-linear constants used in checkAmp() and checkAssign()
float amp3_b{}; //non-linear constants used in checkAmp() and checkAssign()

int vol_man{};  // manual control of volume used in checkMan() only, needs to be passed onto to next checkMan() call
float vol_manScaled{1.0}; // range 0 to 1  used in checkMan() amd DACupdate()

int pan_man{};  // manual control of pan used in checkMan() only, needs to be passed onto to next checkMan() call
float pan_man_L{1.0};  // range 1 to 0 used by checkMan() and DACupdate()
float pan_man_R{1.0};  // range 1 to 0 used by checkMan() and DACupdate()

//variables for wave shape
int shape1a{};  //0 = sine, 1 = triangle, 2 = square used by checkShape() and DACupdate()
int shape1a_last{};  //value 0 - 1023 used by checkShape()
int shape1b{};  //0 = sine, 1 = triangle, 2 = square used by checkShape() and DACupdate()
int shape1b_last{}; //value 0 - 1023 used by checkShape()
int shape2{};   //0 = sine, 1 = triangle, 2 = square used by checkShape() and DACupdate()
int shape2_last{}; //value 0 - 1023 used by checkShape()
int shape3{};   //0 = sine, 1 = triangle, 2 = square used by checkShape() and DACupdate()
int shape3_last{}; //value 0 - 1023 used by checkShape()

uint16_t LFO1{}; // used in DACupdate() and checkFreq(). Keeping as global for now as I may want LFO position outside of DACupdate()
uint16_t LFO2{};
uint16_t LFO3{};

int LFO2_assign{0};    //0 = nothing, 1 = vol, 2 = pan, 3 = LFO1freq, 4 = LFO3freq
int LFO3_assign{0};    //0 = nothing, 1 = vol, 2 = pan

bool LFO2_vol_set{false};    //multipliers to assign LFO2 to various parameters. Used in checkAssign() and DACupdate()
bool LFO2_pan_set{false};
bool LFO2_freq1_set{false};
bool LFO2_freq3_set{false};

bool LFO3_vol_set{false};   //multipliers to assign LFO3 to various parameters
bool LFO3_pan_set{false};

long t1{}; // counters for LFOs used in DACupdate and taptempo()
long t2{};
long t3{};

long period1{};   //used in checkFreq() and checkSway() and checkTap()
long period1a{};  //period going up used in checkFreq() and checkSway() checkTap() and DACupdate()
long period1b{};  //period going down used in checkFreq() and checkSway() checkTap() and DACupdate()
long period2{};   //used in checkFreq() and DACupdate()
long period3{};   //used in checkFreq() and DACupdate()


float wave1aInc{1.0};  //increment up used in checkFreq(), checkSway(), checkTap() and DACupdate()
float wave1bInc{1.0};  //increment down used in checkFreq(), checkSway(), checkTap() and DACupdate()
float wave2Inc{1.0};
float wave3Inc{1.0}; 

bool IS_ON{true}; //ON flag (engage button)

//midi_matrix used to store midi CC inputs
int midi_matrix[9][4] = {  //min, max, current_value, pot_number
  {0,0,0,freq1_pot},  //freq1
  {0,0,0,amp1_pot},  //amp1
  {0,0,0,sway_pot},  //sway
  {0,0,0,freq2_pot},  //freq2
  {0,0,0,amp2_pot},  //amp2
  {0,0,0,freq3_pot},  //freq3
  {0,0,0,amp3_pot},  //amp3
  {0,0,0,vol_pot},   //vol 
  {0,0,0,pan_pot}   //pan
};

int buttonState{};
int tapSpeed{1}; //multiplier for tap frequency
elapsedMillis holdtime; //times length both buttons are held
int freqMode{};  //0 = normal, 1 = fixed multiple.
float ratio2{2.0};  //ratio of freq2 / freq1 used in fixed multiple mode
float ratio3{2.0};  //ratio of freq3 / freq1 used in fixed multiple mode

void setup() {

//pinmodes

   pinMode(engage_LED, OUTPUT);
   pinMode(midi_LED, OUTPUT);
   pinMode(PWM_L,OUTPUT);
   pinMode(PWM_R,OUTPUT);
   pinMode(LFO2Vol_LED, OUTPUT);
   pinMode(LFO2Pan_LED, OUTPUT);
   pinMode(LFO2freq1_LED, OUTPUT);
   pinMode(LFO2freq3_LED, OUTPUT);
   pinMode(LFO3Vol_LED, OUTPUT);
   pinMode(LFO3Pan_LED,OUTPUT);
   pinMode(SPI_CS, OUTPUT);
   
   pinMode(assign2_sw,INPUT_PULLUP);
   pinMode(assign3_sw,INPUT_PULLUP);
   pinMode(assign_midi_sw,INPUT_PULLUP);
   pinMode(engage_sw,INPUT_PULLUP);
   pinMode(tap_sw,INPUT_PULLUP);


//buttons debounce
   assign2Button .attach(assign2_sw);
   assign2Button .interval(constants::wait);
   assign3Button .attach(assign3_sw);
   assign3Button .interval(constants::wait);
   assignMidiButton .attach(assign_midi_sw);
   assignMidiButton .interval(constants::wait);
   engageButton .attach(engage_sw);
   engageButton .interval(constants::wait);
   TAP .attach(tap_sw);
   TAP .interval(constants::wait);

// reset LEDs
resetLEDs();
VSSD();

//start timer interrupt
myTimer.begin(DACupdate, constants::interruptPeriod); //

amp1_a = constants::a_amp; //initial non-lin values amplitude pot scaling. these are global varibles because need to change depending on assignment
amp1_b = constants::b_amp;

amp2_a = constants::a_amp;
amp2_b = constants::b_amp;

amp3_a = constants::a_amp;
amp3_b = constants::b_amp;


//initialize SPI port (dac)
myDac.begin();  //DAC spi port

MidiInit();
//delay(2000);  //wait before starting loop

}


 
void checkFreq() {

int freq1Current{};
int freq2Current{};
int freq3Current{};

static int freq1{constants::in_max * 2};  //high number so (freq1Current - freq1) is always larger than tolerance
static int freq2{constants::in_max * 2};
static int freq3{constants::in_max * 2};

static float freq1scaled{}; //needs to be static so in fixed multiple mode freq2 and freq3 can update correctly
float freq2scaled{};
float freq3scaled{};

//freq1

if(midi_matrix[0][0] == midi_matrix[0][1]){   // if not set read analog input
freq1Current = map(analogRead(freq1_pot),0,constants::in_max,0,constants::in_max); //read from analog input
}
else{
  freq1Current = midi_matrix[0][2];    //read from midi input
}
freq1Current = Scale(freq1Current,constants::in_max,constants::a_freq,constants::b_freq); //scale freq1 with non-linear coefficients



//freq2
if(midi_matrix[3][0] == midi_matrix[3][1]){   // if not set read analog input
freq2Current = map(analogRead(freq2_pot),0,constants::in_max,0,constants::in_max);
}
else{
  freq2Current = midi_matrix[3][2];
}

if(freqMode == 0){
  freq2Current = Scale(freq2Current,constants::in_max,constants::a_freq,constants::b_freq);
}



//freq3
if(midi_matrix[5][0] == midi_matrix[5][1]){   // if not set read analog input
freq3Current = map(analogRead(freq3_pot),0,constants::in_max,0,constants::in_max);
}
else{
  freq3Current = midi_matrix[5][2];
}

if(freqMode == 0){
  freq3Current = Scale(freq3Current,constants::in_max,constants::a_freq,constants::b_freq);
}



if(freqMode == 0){ //no LFO1 or LFO3 modulation by LFO2 in fixed multiple mode
  
  //update freq1 and freq3 based on LFO2 assignment:
  int LFO2freq1{};  //value added to freq1 from to LFO2
  int LFO2freq3{};  //value added to freq3 from LFO2
  
   if(LFO2 > 2048){
   LFO2freq1 = ((LFO2 * 2 / float(constants::DAC_max)) - 1) * (amp2 / float(constants::in_max)) * (float(constants::in_max) - freq1Current);
   LFO2freq3 = ((LFO2 * 2 / float(constants::DAC_max)) - 1) * (amp2 / float(constants::in_max)) * (float(constants::in_max) - freq3Current);
   }
   
   if(LFO2 < 2048){
   LFO2freq1 = ((LFO2 * 2 / float(constants::DAC_max)) - 1) * (amp2 / float(constants::in_max)) * freq1Current;
   LFO2freq3 = ((LFO2 * 2 / float(constants::DAC_max)) - 1) * (amp2 / float(constants::in_max)) * freq3Current;
   }
    
    freq1Current = freq1Current + (LFO2_freq1_set*LFO2freq1);
    freq3Current = freq3Current + (LFO2_freq3_set*LFO2freq3);

}

//check freq1Current has changed, if so calculate values for DACupdate()

  if (abs(freq1Current-freq1)>constants::freq1Tolerance){//if reading from pot exceeds tolerance
    freq1 = freq1Current;//new frequency- number between 0 and 1023
    freq1scaled = (freq1 - constants::in_min) * (constants::freq1_max - constants::freq1_min) / (constants::in_max - constants::in_min) + constants::freq1_min;

updatefreq1(freq1scaled);

/*
    //period1 = constants::samplerate / (freq1scaled / 100);
    period1 = constants::samplerate / (freq1scaled);
    period1a = period1 * swayScaled;
    //period1b = period1 * (1 - swayScaled);
    period1b = period1 - period1a;
    wave1aInc = float(constants::waveLength - 1)/period1a;
    wave1bInc = float(constants::waveLength - 1)/period1b;

    

    if(freqMode ==1){
      freq2scaled = freq1scaled * ratio2;
      //period2 = constants::samplerate/(2 * freq2scaled/100);
      period2 = constants::samplerate/(2 * freq2scaled);
      wave2Inc = float(constants::waveLength - 1)/period2; //float 

      freq3scaled = freq1scaled * ratio3;
      //period3 = constants::samplerate/(2* freq3scaled/100);
      period3 = constants::samplerate/(2* freq3scaled);
      wave3Inc = float(constants::waveLength - 1)/period3;

      //t1=t2=t3=0;
      //direction1 = direction2 = direction3 = constants::down;
    }
/*
Serial.print("period1a: ");
Serial.println(period1a);
Serial.print("period1b: ");
Serial.println(period1b);
Serial.print("wave1aInc: ");
Serial.println(wave1aInc,6);
Serial.print("wave1bInc: ");
Serial.println(wave1bInc,6);
*/

  }

//check freq2Current has changed, if so calculate values for DACupdate()

  if (abs(freq2Current-freq2)>constants::freq2Tolerance){//if reading from pot exceeds tolerance
      freq2 = freq2Current;//new frequency- number between 0 and 1023
      
      if(freqMode == 0){
        freq2scaled = (freq2 - constants::in_min) * (constants::freq2_max - constants::freq2_min) / (constants::in_max - constants::in_min) + constants::freq2_min;

      }
      else if(freqMode == 1){
        ratio2 = constants::ratios[map(freq2,constants::in_min,constants::in_max,0,constants::numRatios - 1)];
        freq2scaled = freq1scaled * ratio2;
        //Serial.print("ratio2 = ");
        //Serial.println(ratio2,4);
      }
      updatefreq2(freq2scaled);
      /*
      //period2 = constants::samplerate/(2 * freq2scaled/100);
      period2 = constants::samplerate/(2 * freq2scaled);
      wave2Inc = float(constants::waveLength - 1)/period2; //float 
      */
  }

//check freq3Current has changed, if so calculate values for DACupdate()

  if (abs(freq3Current-freq3)>constants::freq3Tolerance){//if reading from pot exceeds tolerance
      freq3 = freq3Current;//new frequency- number between 0 and 1023

      if(freqMode == 0){
        freq3scaled = (freq3 - constants::in_min) * (constants::freq3_max - constants::freq3_min) / (constants::in_max - constants::in_min) + constants::freq3_min;

      }
      else if(freqMode == 1){
        ratio3 = constants::ratios[map(freq3,constants::in_min,constants::in_max,0,constants::numRatios - 1)];
        freq3scaled = freq1scaled * ratio3;
        //Serial.print("ratio3 = ");
        //Serial.println(ratio3,4);
      }
      updatefreq3(freq3scaled);
      /*
      //period3 = constants::samplerate/(2* freq3scaled/100);
      period3 = constants::samplerate/(2* freq3scaled);
      wave3Inc = float(constants::waveLength - 1)/period3;
      */
  }
 
}

void  checkAmp(){
int amp1Current{};
int amp2Current{};
int amp3Current{};

if(midi_matrix[1][0] == midi_matrix[1][1]){   // if not set read analog input
  amp1Current = map(analogRead(amp1_pot),0,constants::in_max,0,constants::in_max);  
}
else{
    amp1Current = midi_matrix[1][2];  
}

if(midi_matrix[4][0] == midi_matrix[4][1]){   // if not set read analog input
  amp2Current = map(analogRead(amp2_pot),0,constants::in_max,0,constants::in_max);
}
else{
    amp2Current = midi_matrix[4][2];  
}


if(midi_matrix[6][0] == midi_matrix[6][1]){   // if not set read analog input
  amp3Current = map(analogRead(amp3_pot),0,constants::in_max,0,constants::in_max);
}
else{
    amp3Current = midi_matrix[6][2];  
}

//scale with non-linear coefficients:

  amp1Current = Scale(amp1Current,constants::in_max,amp1_a,amp1_b); 
  amp2Current = Scale(amp2Current,constants::in_max,amp2_a,amp2_b);
  amp3Current = Scale(amp3Current,constants::in_max,amp3_a,amp3_b);
  
  if (abs(amp1Current-amp1)>constants::amp1Tolerance){//if reading from pot exceeds tolerance
    amp1 = amp1Current;//new amplitude- number between 0 and 1023
  }
  if (abs(amp2Current-amp2)>constants::amp2Tolerance){//if reading from pot exceeds tolerance
    amp2 = amp2Current;//new amplitude- number between 0 and 1023
  }
  if (abs(amp3Current-amp3)>constants::amp3Tolerance){//if reading from pot exceeds tolerance
    amp3 = amp3Current;//new amplitude- number between 0 and 1023
  }
}


void  checkSway(){
int swayCurrent{};

if(midi_matrix[2][0] == midi_matrix[2][1]){   // if not set read analog input
    swayCurrent = map(analogRead(sway_pot),0,constants::in_max,0,constants::in_max);
}
else{
    swayCurrent = midi_matrix[2][2];
}
  
//need new scale function here to have wide middle band

  if (abs(swayCurrent-sway)>constants::swayTolerance){//if reading from pot exceeds tolerance
    sway = swayCurrent;//new frequency- number between 0 and 1023
    swayScaled = (sway - constants::in_min) * (constants::sway_max - constants::sway_min) / (constants::in_max - constants::in_min) + constants::sway_min;
    period1a = period1 * swayScaled;
    period1b = period1 - period1a;
    wave1aInc = float(constants::waveLength - 1)/period1a;             
    wave1bInc = float(constants::waveLength - 1)/period1b;

  }
}

void  checkMan(){
int vol_manCurrent{};
int pan_manCurrent{};

if(midi_matrix[7][0] == midi_matrix[7][1]){   // if not set read analog input
    
    vol_manCurrent = map(analogRead(vol_pot),0,constants::in_max,0,constants::in_max);
}
else{
    vol_manCurrent = midi_matrix[7][2];
}

if(midi_matrix[8][0] == midi_matrix[8][1]){   // if not set read analog input
    
  pan_manCurrent = map(analogRead(pan_pot),0,constants::in_max,constants::in_max,0);
}
else{
    pan_manCurrent = midi_matrix[8][2];

}

//need new scale function here to have wide middle band
  
  if (abs(vol_manCurrent-vol_man)>constants::vol_manTolerance){
    //if reading from pot exceeds tolerance
    vol_man = vol_manCurrent;
    vol_manScaled = (vol_man /float(constants::in_max));  //range 0 to 1;
  }

 // Serial.print("pan_manCurrent = ");
 // Serial.println(pan_manCurrent);
    
  //call panAdjust(); //adjust current pan value to match pan Law
  pan_manCurrent = panAdjust(pan_manCurrent);


  //Serial.print("pan_manCurrent = ");
  //Serial.println(pan_manCurrent);
    
  if (abs(pan_manCurrent-pan_man)>constants::pan_manTolerance){
    //if reading from pot exceeds tolerance
    float pan_manScaled{};
    pan_man = pan_manCurrent;
    pan_manScaled = ((pan_man / float(constants::in_max)) * 2) + (-1); //range -1 to 1 


    if (pan_manScaled == 0.0) { 
      pan_man_L = 1.0;
      pan_man_R = 1.0;
      } 
    else if(pan_manScaled > 0.0){
      pan_man_L = (1.0 - pan_manScaled);
      pan_man_R = 1.0;  
    }
    else if (pan_manScaled < 0.0) {
      pan_man_L = 1.0;
      pan_man_R = (1.0 + pan_manScaled);  
    }
  }


}

void checkAssign(){
  assign2Button.update();
  assign3Button.update();
  assignMidiButton.update();
  
if(assign2Button.fallingEdge()){
  LFO2_assign = LFO2_assign + 1;
  if(LFO2_assign > 4){
    LFO2_assign = 0;
  }

switch (LFO2_assign){
  case 0: //no assignment
  LFO2_vol_set = false;
  LFO2_pan_set = false;
  LFO2_freq1_set = false;
  LFO2_freq3_set = false;
  //digitalWrite(LFO2freq3_LED, LOW);
  break;
  case 1: //vol assignment
  LFO2_vol_set = true;
  LFO2_pan_set = false;
  LFO2_freq1_set = false;
  LFO2_freq3_set = false;
  amp2_a = constants::a_amp;
  amp2_b = constants::b_amp;
  //digitalWrite(LFO2Vol_LED, HIGH);
  break;
  case 2: //pan assignment
  amp2_a = constants::a_lin;
  amp2_b = constants::b_lin;
  LFO2_vol_set = false;
  LFO2_pan_set = true;
  LFO2_freq1_set = false;
  LFO2_freq3_set = false;
  //digitalWrite(LFO2Vol_LED, LOW);
  //digitalWrite(LFO2Pan_LED, HIGH);
  break;
  case 3: //freq1 assignment
  amp2_a = constants::a_lin;
  amp2_b = constants::b_lin;
  LFO2_vol_set = false;
  LFO2_pan_set = false;
  LFO2_freq1_set = true;
  LFO2_freq3_set = false;
  //digitalWrite(LFO2Pan_LED, LOW);
 // digitalWrite(LFO2freq1_LED, HIGH);
  break;
  case 4: //freq3 assignment
  amp2_a = constants::a_lin;
  amp2_b = constants::b_lin;
  LFO2_vol_set = false;
  LFO2_pan_set = false;
  LFO2_freq1_set = false;
  LFO2_freq3_set = true;
  //digitalWrite(LFO2freq1_LED, LOW);
  //digitalWrite(LFO2freq3_LED, HIGH);
  break;
  
}
resetLEDs();
}

if(assign3Button.fallingEdge()){
  
    LFO3_assign = LFO3_assign + 1;
    if(LFO3_assign > 2){
      LFO3_assign = 0;
    }

  switch (LFO3_assign){
    case 0: //no assignment
    LFO3_vol_set = false;
    LFO3_pan_set = false;
  //digitalWrite(LFO3Pan_LED, LOW);
    break;
    case 1: //vol assignment
    amp3_a = constants::a_amp;
    amp3_b = constants::b_amp;
    LFO3_vol_set = true;
    LFO3_pan_set = false;
 // digitalWrite(LFO3Vol_LED, HIGH);
    break;
    case 2: //pan assignment
    amp3_a = constants::a_lin;
    amp3_b = constants::b_lin;
    LFO3_vol_set = false;
    LFO3_pan_set = true;
 // digitalWrite(LFO3Vol_LED, LOW);
 // digitalWrite(LFO3Pan_LED, HIGH);
    break;
  }

resetLEDs();
}

if(assignMidiButton.fallingEdge()){
  digitalWrite(midi_LED, HIGH);

for(int j = 0; j < 9; j++){  

midi_matrix[j][0] = analogRead(midi_matrix[j][3]);
midi_matrix[j][1] = midi_matrix[j][0];  //set max same as min
   
}
//populate all midi matrix min values
 
}

if(assignMidiButton.risingEdge()){
    digitalWrite(midi_LED, LOW);

for(int k = 0; k < 9; k++){ 

if(abs(analogRead(midi_matrix[k][3]) - midi_matrix[k][0]) < constants::midiPotTol){ //if less than tolerance
  
//do nothing
}
else{ //else more than tolerance set to analog read value
midi_matrix[k][1] = analogRead(midi_matrix[k][3]);  //set max value
}

   
}
  
}



}

void checkShape(){

  int shape1aCurrent{map(analogRead(shape1a_pot),0,constants::in_max,0,constants::in_max)};  
  int shape1bCurrent{map(analogRead(shape1b_pot),0,constants::in_max,0,constants::in_max)};   
  int shape2Current{map(analogRead(shape2_pot),0,constants::in_max,0,constants::in_max)};   
  int shape3Current{map(analogRead(shape3_pot),0,constants::in_max,0,constants::in_max)};   
  
  
  if (abs(shape1aCurrent-shape1a_last)>constants::shapeTolerance){
      //if reading from pot exceeds tolerance
      shape1a_last = shape1aCurrent;
  
  shape1a = map(shape1a_last,0,constants::in_max,0,constants::numShapes - 1);
      
  }
  
  if (abs(shape1bCurrent-shape1b_last)>constants::shapeTolerance){
      //if reading from pot exceeds tolerance
      shape1b_last = shape1bCurrent;
      
  shape1b = map(shape1b_last,0,constants::in_max,0,constants::numShapes - 1);
  }
  
  if (abs(shape2Current-shape2_last)>constants::shapeTolerance){
      //if reading from pot exceeds tolerance
      shape2_last = shape2Current;
      
  shape2 = map(shape2_last,0,constants::in_max,0,constants::numShapes - 1);
  }
  
  if (abs(shape3Current-shape3_last)>constants::shapeTolerance){
      //if reading from pot exceeds tolerance
      shape3_last = shape3Current;
      
  shape3 = map(shape3_last,0,constants::in_max,0,constants::numShapes - 1);
  }


}

void checkEngage(){

  engageButton.update();

  if(engageButton.fallingEdge()){
  
    if(!IS_ON){
  
      IS_ON = true;
      digitalWrite(engage_LED, HIGH);
    }
  
     else{
  
      IS_ON = false;
      digitalWrite(engage_LED, LOW);
    }
    buttonState = buttonState + 2;
    if (buttonState ==3){
      holdtime = 0; //reset timer
    }

  }

  if(engageButton.risingEdge()){
    buttonState = buttonState - 2;
  }

}

void MCP4922_write(byte dac, int value) {    //check value type
   myDac.analogWrite(dac,1,1,1,value);
}

int Scale(int value, int range, float a, float b){

int Scaled;

if (value < (a * range)) {
Scaled = (b/a)*value;
    }
else {
Scaled = ((b-1) / (a-1) * value) + (range * (1 - ((b-1)/(a-1))));
    }
return Scaled;
}


void checkTap(){

TAP.update(); //button constants::update

  if(TAP.fallingEdge()){
    tapSpeed = 1;
    tap_tempo();
    buttonState = buttonState + 1;
    if (buttonState ==3){ 
      holdtime = 0; //reset timer
    }
  }

  if(TAP.risingEdge()){
    buttonState = buttonState - 1;
  }

}

void checkBoth(){


  if (buttonState == 3){

    if (holdtime >= 2000){
      Serial.println("MODE SELECT!");

      myTimer.end();  //stop interrupt timer
      freqMode = changeMode(freqMode);
      holdtime = 0;
      myTimer.begin(DACupdate, constants::interruptPeriod);
    }
    
  }
}

void DACupdate(){ //Called by myTimer
  //increment t and reset each time it reaches period
  
  t1 += 1;
  t2 += 1;
  t3 += 1;
  
  //toggle direction of wave 1
  if(direction1 == constants::up){  //going up
   if (t1 > period1a){
    t1 = 0;
    direction1 = constants::down;  //set direction to down
   }
  }
  if(direction1 == constants::down){             //going down
   if (t1 > period1b){  
    t1 = 0;
    direction1 = constants::up;  //set direction to up
   }
  }
  

  //toggle direction of wave 2
  if(direction2 == constants::up){  //going up
   if (t2 > period2){
    t2 = 0;
    direction2 = constants::down;  //set direction to down
   }
  }
  if(direction2 == constants::down){                //going down
   if (t2 > period2){  
    t2 = 0;
    direction2 = constants::up;  //set direction to up
   }
  }

  //toggle direction of wave 3
  if(direction3 == constants::up){  //going up
   if (t3 > period3){
    t3 = 0;
    direction3 = constants::down;  //set direction to down
   }
  }
  if(direction3 == constants::down){                //going down
   if (t3 > period3){  
    t3 = 0;
    direction3 = constants::up;  //set direction to up
   }
  }

//phase correction in fixed multiple mode:
if(freqMode == 1){
  if(t1 == 0)
  {
    if ((abs(t2-t1) < constants::phaseCorrectTime) && (direction2 == direction1))
    {
      t2=0; //reset LFO2 timer to match
    }
    if ((abs(t3-t1) < constants::phaseCorrectTime) && (direction3 == direction1))
    {
      t3=0; //reset LFO3 timer to match
    }
  }

}

//wave indexes
static int wave1Num{};
static int wave2Num{};
static int wave3Num{};

//set wave1 indexes and lookup values

    if(direction1 == constants::up){  //going up
    wave1Num = t1*wave1aInc;


     // ratio1 = wave1Num - int(wave1Num);
  //read data from correct wave table for wave 1
  switch (shape1a) {
  case 0:  //sine wave
    // statements
    LFO1 = pgm_read_word_near(constants::sine + wave1Num);
    break;
  case 1:  //triangle wave
    // statements
    LFO1 = pgm_read_word_near(constants::triangle + wave1Num);
    break;
  case 2:  //square wave
    // statements
    LFO1 = pgm_read_word_near(constants::square + wave1Num);
    break;
  case 3: //qtr_sine_upr
      LFO1 = pgm_read_word_near(constants::qtr_sine_upr + wave1Num);
  break;
  case 4: //qtr_sine_lwr
      LFO1 = pgm_read_word_near(constants::qtr_sine_lwr + wave1Num);
  break;
  case 5: //inverted_sine
      LFO1 = pgm_read_word_near(constants::inverted_sine + wave1Num);
  break;  
}
    }
    if(direction1 == constants::down){  //going down
    wave1Num = constants::waveLength - (t1*wave1bInc) - 1;
    //read data from correct wave table for wave 1
    switch (shape1b) {
  case 0:
    // statements
    LFO1 = pgm_read_word_near(constants::sine + wave1Num);
    break;
  case 1:
    // statements
    LFO1 = pgm_read_word_near(constants::triangle + wave1Num); 
    break;
  case 2:
    // statements
    LFO1 = pgm_read_word_near(constants::square + wave1Num);
    break;
  case 3: //qtr_sine_upr
      LFO1 = pgm_read_word_near(constants::qtr_sine_upr + wave1Num);
  break;
  case 4: //qtr_sine_lwr
      LFO1 = pgm_read_word_near(constants::qtr_sine_lwr + wave1Num);
  break;
  case 5: //inverted_sine
      LFO1 = pgm_read_word_near(constants::inverted_sine + wave1Num);
  break;  
}
    }


//set wave2 indexes
    if(direction2 == constants::up){  //going up
    wave2Num = t2*wave2Inc;
    }
    if(direction2 == constants::down){  //going down
    wave2Num = constants::waveLength - (t2*wave2Inc) - 1;
    }

//set wave3 indexes
    if(direction3 == constants::up){  //going up
    wave3Num = t3*wave3Inc;
    }
    if(direction3 == constants::down){  //going down
    wave3Num = constants::waveLength - (t3*wave3Inc) - 1;
    }
//read data from correct wave table for wave2


  switch (shape2) {
  case 0:  //sine wave
    // statements
    LFO2 = pgm_read_word_near(constants::sine + wave2Num);
    break;
  case 1:  //triangle wave
    // statements

    LFO2 = pgm_read_word_near(constants::triangle + wave2Num);
    break;
  case 2:  //square wave
    // statements
    LFO2 = pgm_read_word_near(constants::square + wave2Num);
    break;
  case 3: //qtr_sine_upr
      LFO2 = pgm_read_word_near(constants::qtr_sine_upr + wave2Num);
  break;
  case 4: //qtr_sine_lwr
      LFO2 = pgm_read_word_near(constants::qtr_sine_lwr + wave2Num);
  break;
  case 5: //inverted_sine
      LFO2 = pgm_read_word_near(constants::inverted_sine + wave2Num);
  break;  
}

//read data from correct wave table for wave3

  switch (shape3) {
  case 0:  //sine wave
    // statements
    LFO3 = pgm_read_word_near(constants::sine + wave3Num);
    break;
  case 1:  //triangle wave
    // statements
    LFO3 = pgm_read_word_near(constants::triangle + wave3Num);
    break;
  case 2:  //square wave
    // statements
    LFO3 = pgm_read_word_near(constants::square + wave3Num);
    break;
  case 3: //qtr_sine_upr
      LFO3 = pgm_read_word_near(constants::qtr_sine_upr + wave3Num);
  break;
  case 4: //qtr_sine_lwr
      LFO3 = pgm_read_word_near(constants::qtr_sine_lwr + wave3Num);
  break;
  case 5: //inverted_sine
      LFO3 = pgm_read_word_near(constants::inverted_sine + wave3Num);
  break;  
}
 //***********************************************************************************
//Scaled LFOs for volume
float LFO1Scaled{LFO1 * (amp1/float(constants::in_max)) / (constants::DAC_max) + (1 - (amp1/float(constants::in_max)))}; //range 0 - 1 inverted
float LFO2Scaled{(LFO2_vol_set * LFO2 * (amp2/float(constants::in_max)) / (constants::DAC_max)) + (1 - (LFO2_vol_set *(amp2/float(constants::in_max))))}; //range 0 - 1 inverted
float LFO3Scaled{(LFO3_vol_set * LFO3 * (amp3/float(constants::in_max)) / (constants::DAC_max)) + (1 - (LFO3_vol_set *(amp3/float(constants::in_max))))}; //range 0 - 1 inverted

//Scaled LFOs for pan
//call panAdjust() to adjust current pan value to match pan Law

if (LFO2_pan_set == 1)
  LFO2 = 4 * panAdjust(LFO2/4);
if (LFO3_pan_set == 1)
  LFO3 = 4 * panAdjust(LFO3/4);

//calculate pan values for LFO2 and LFO3, in range -1 to 1.
float LFO2pan{((LFO2 / float(constants::DAC_max)) * (LFO2_pan_set * 2 * amp2 / float(constants::in_max))) + (-1 * LFO2_pan_set * amp2 / float(constants::in_max))};
float LFO3pan{((LFO3 / float(constants::DAC_max)) * (LFO3_pan_set * 2 * amp3 / float(constants::in_max))) + (-1 * LFO3_pan_set * amp3 / float(constants::in_max))};

float LFO2pan_L{};
float LFO2pan_R{};
float LFO3pan_L{};
float LFO3pan_R{};

//convert pan values into L and R volume multipliers
if (LFO2pan == 0.0) { 
  LFO2pan_L = 1.0;
  LFO2pan_R = 1.0;
  } 
else if(LFO2pan > 0.0){
  LFO2pan_L = (1.0 - LFO2pan);
  LFO2pan_R = 1.0;  
}
else if (LFO2pan < 0.0) {
  LFO2pan_L = 1.0;
  LFO2pan_R = (1.0 + LFO2pan);
}

if (LFO3pan == 0.0) { 
  LFO3pan_L = 1.0;
  LFO3pan_R = 1.0;
  } 
else if(LFO3pan > 0.0){
  LFO3pan_L = (1.0 - LFO3pan);
  LFO3pan_R = 1.0;  
}
else if (LFO3pan < 0.0) {
  LFO3pan_L = 1.0;
  LFO3pan_R = (1.0 + LFO3pan);
}

//(x - constants::in_min) * (out_max - out_min) / (constants::in_max - constants::in_min) + out_min;

// combine all LFOs and manual inputs to dac outputs:

uint16_t dac1{vol_manScaled * LFO1Scaled * LFO2Scaled * LFO3Scaled * LFO2pan_L * LFO3pan_L * pan_man_L * constants::DAC_max}; //Left channel
uint16_t dac2{vol_manScaled * LFO1Scaled * LFO2Scaled * LFO3Scaled * LFO2pan_R * LFO3pan_R * pan_man_R * constants::DAC_max}; //Right channel


//smoothing on dac outputs:
static int readIndex{};
static uint16_t dac1_total{};
static uint16_t dac2_total{};
static uint16_t dac1_array[constants::numReadings] = { };
static uint16_t dac2_array[constants::numReadings] = { };

// subtract the last reading:S
dac1_total = dac1_total - dac1_array[readIndex];
dac2_total = dac2_total - dac2_array[readIndex];
// read from the sensor:
dac1_array[readIndex] = dac1;
dac2_array[readIndex] = dac2;
// add the reading to the total:
dac1_total = dac1_total + dac1_array[readIndex];
dac2_total = dac2_total + dac2_array[readIndex];
// advance to the next position in the array:
readIndex = readIndex + 1;

// if we're at the end of the array...
if (readIndex >= constants::numReadings) {
  // ...wrap around to the beginning:
  readIndex = 0;
}


static uint16_t dac1_average{};
static uint16_t dac2_average{};

//calculate averages
dac1_average = dac1_total / constants::numReadings;
dac2_average = dac2_total / constants::numReadings;



//write PWM values for LED output

uint16_t PWM_L_value{map(dac1_average,constants::DAC_min,constants::DAC_max,constants::PWM_min,constants::PWM_max)};
uint16_t PWM_R_value{map(dac2_average,constants::DAC_min,constants::DAC_max,constants::PWM_min,constants::PWM_max)};

/*
uint16_t PWM_L_value{};
uint16_t PWM_R_value{};

if ( dac1_average <= (constants::DAC_max  / 2.0) )
  PWM_L_value = constants::PWM_min;
else
  PWM_L_value = constants::PWM_max;

if ( dac2_average <= (constants::DAC_max  / 2.0) )  
  PWM_R_value = constants::PWM_min;
else
  PWM_R_value = constants::PWM_max;
*/

analogWrite(PWM_L,PWM_L_value);
analogWrite(PWM_R,PWM_R_value);

//scale dac outputs to useful range (and invert due to control range for VCA chip):
dac1_average = map(dac1_average,constants::DAC_min,constants::DAC_max,constants::DAC_max_out,constants::DAC_min_out);
dac2_average = map(dac2_average,constants::DAC_min,constants::DAC_max,constants::DAC_max_out,constants::DAC_min_out);

//check engage switch is on
if(IS_ON){   //write dac values to MCP4922
//write values to dac
  MCP4922_write(0, dac1_average);
  MCP4922_write(1, dac2_average);

}
else{ //write bypass values to MCP4922

  MCP4922_write(0, constants::bypass_level);
  MCP4922_write(1, constants::bypass_level);
    }
}

void loop() {


checkFreq();   //check freq inputs
checkAmp();   // check amplitude inputs
checkSway();  // check sway inputs
checkShape(); //check wave shape inputs
checkMan();  //manual vol and pan inputs

checkTap();   //check tap tempo for freq1
checkAssign(); //check assignment buttons
checkMidi();  //check midi input
checkEngage();  //check engage button
checkBoth();

//Serial.print("buttonState: ");
//Serial.println(buttonState);


//Debug LFO shapes - delay will mess up midi input!

/*
Serial.println(LFO2);
Serial.print(",");
Serial.print(LFO3);
Serial.print(",");
Serial.println(LFO1);

delay(5);

*/

}
