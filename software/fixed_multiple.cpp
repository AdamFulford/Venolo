#include "fixed_multiple.h"

int changeMode(int currentmode)
{
   //Set all LFO counters to 0 and directions to down.
   t1 = t2 = t3 = 0;  
   direction1 = direction2 = direction3 = constants::down; 
   //flash all LEDs 5 times
   for (int i = 1; i < 5; i++)
   {
     allLEDsON();
     delay(100);
     allLEDsOFF();
     delay(100);
     allLEDsON();
   }
   delay(1000);
   allLEDsOFF();
   
  if (currentmode == 1){  //back to normal mode
     digitalWrite(engage_LED,HIGH);
     digitalWrite(midi_LED,HIGH);
     digitalWrite(engage_LED,HIGH);
     analogWrite(PWM_L,256);
     analogWrite(PWM_R,256);
     digitalWrite(LFO2Vol_LED, HIGH);
     digitalWrite(LFO2Pan_LED, HIGH);
     delay(2000);
     allLEDsOFF();
     delay(5);
     toggleEngage();
     resetLEDs();
     delay(5);
   
   float freq1scaled{};
   float freq2scaled{};
   float freq3scaled{};

     //read pot and scale with non-linear coeffs to get freq1scaled:
     freq1scaled = Scale(analogRead(freq1_pot),constants::in_max,constants::a_freq,constants::b_freq);
     //scale for frequency range:
     freq1scaled = (freq1scaled - constants::in_min) * (constants::freq1_max - constants::freq1_min) / (constants::in_max - constants::in_min) + constants::freq1_min;
     Serial.print("freq1scaled: ");
     Serial.println(freq1scaled);
   
   //read pot and scale with non-linear coeffs:
   freq2scaled = Scale(analogRead(freq2_pot),constants::in_max,constants::a_freq,constants::b_freq);
   freq3scaled = Scale(analogRead(freq3_pot),constants::in_max,constants::a_freq,constants::b_freq);
   
   //scale for frequency range:
   freq2scaled = (freq2scaled - constants::in_min) * (constants::freq2_max - constants::freq2_min) / (constants::in_max - constants::in_min) + constants::freq2_min;
   freq3scaled = (freq3scaled - constants::in_min) * (constants::freq3_max - constants::freq3_min) / (constants::in_max - constants::in_min) + constants::freq3_min;

     Serial.print("freq2scaled: ");
     Serial.println(freq2scaled);
     Serial.print("freq3scaled: ");
     Serial.println(freq3scaled);
   
   //update LFO2 and LFO3 period and waveinc:
   updatefreq1(freq1scaled);
   updatefreq2(freq2scaled);
   updatefreq3(freq3scaled);
     
     
     return 0; //switch to normal mode
  }
  else  //switch to fixed multiple mode
   digitalWrite(LFO2freq1_LED, HIGH);
   digitalWrite(LFO2freq3_LED, HIGH);
   digitalWrite(LFO3Vol_LED, HIGH);
   digitalWrite(LFO3Pan_LED,HIGH);
   delay(2000);
   allLEDsOFF();
   delay(5);
   toggleEngage();
   resetLEDs();
   delay(5);

     //Derive freq1scaled from pot value:
     float freq1scaled{};
     int freq2{};
     int freq3{};
     
     //read pot and scale with non-linear coeffs to get freq1scaled:
     freq1scaled = Scale(analogRead(freq1_pot),constants::in_max,constants::a_freq,constants::b_freq);
     //scale for frequency range:
     freq1scaled = (freq1scaled - constants::in_min) * (constants::freq1_max - constants::freq1_min) / (constants::in_max - constants::in_min) + constants::freq1_min;
     Mode1Period = (1.0/ (4.0 * freq1scaled * global_mult )) * 1.0E6; //microseconds

     //sway settings
     //up
     LFO1aMult = constants::swayUpMults[map(analogRead(sway_pot),constants::in_min,constants::in_max,0,constants::numSwaySettings - 1)];
     LFO1aDiv = constants::swayUpDivs[map(analogRead(sway_pot),constants::in_min,constants::in_max,0,constants::numSwaySettings - 1)];

      //down
     LFO1bMult = constants::swayDwnMults[map(analogRead(sway_pot),constants::in_min,constants::in_max,0,constants::numSwaySettings - 1)];
     LFO1bDiv = constants::swayDwnDivs[map(analogRead(sway_pot),constants::in_min,constants::in_max,0,constants::numSwaySettings - 1)]; 
     
     //read pots to get ratios for LFO2 and LFO3:
     freq2 = map(analogRead(freq2_pot),0,constants::in_max,0,constants::in_max);
     freq3 = map(analogRead(freq3_pot),0,constants::in_max,0,constants::in_max);

     //update LFO2 and LFO3 period and waveinc:
     updatefreq1Mode1(freq1scaled);
     updatefreq2Mode1(freq2);
     updatefreq3Mode1(freq3);
     



   return 1; //switch to fixed multiple mode
  
}



void toggleEngage() //this is to correct for engage button toggling IS_ON when pressed to change mode
{
       if(!IS_ON)
    {
     //digitalWrite(engage_LED,HIGH);
     IS_ON = true;
    }
   else
   {
     //digitalWrite(engage_LED,LOW);
     IS_ON = false;
   }
}
