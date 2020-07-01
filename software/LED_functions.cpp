#include "LED_functions.h"

void allLEDsON(){
   digitalWrite(engage_LED,HIGH);
   digitalWrite(midi_LED,HIGH);
   analogWrite(PWM_L,256);
   analogWrite(PWM_R,256);
   digitalWrite(LFO2Vol_LED, HIGH);
   digitalWrite(LFO2Pan_LED, HIGH);
   digitalWrite(LFO2freq1_LED, HIGH);
   digitalWrite(LFO2freq3_LED, HIGH);
   digitalWrite(LFO3Vol_LED, HIGH);
   digitalWrite(LFO3Pan_LED,HIGH);
}

void allLEDsOFF(){
   digitalWrite(engage_LED,LOW);
   digitalWrite(midi_LED,LOW);
   analogWrite(PWM_L,0);
   analogWrite(PWM_R,0);
   digitalWrite(LFO2Vol_LED, LOW);
   digitalWrite(LFO2Pan_LED, LOW);
   digitalWrite(LFO2freq1_LED, LOW);
   digitalWrite(LFO2freq3_LED, LOW);
   digitalWrite(LFO3Vol_LED, LOW);
   digitalWrite(LFO3Pan_LED,LOW);
}

void resetLEDs(){
  
   if(LFO2_vol_set)
   {
    digitalWrite(LFO2Vol_LED, HIGH);
   }
   else{
    digitalWrite(LFO2Vol_LED, LOW);
   }

   if(LFO2_pan_set)
   {
    digitalWrite(LFO2Pan_LED, HIGH);
   }
   else{
    digitalWrite(LFO2Pan_LED, LOW);
   }
    
  if(LFO2_freq1_set)
   {
    digitalWrite(LFO2freq1_LED, HIGH);
   }
  else{
    digitalWrite(LFO2freq1_LED, LOW);
  }
  
   if(LFO2_freq3_set)
   {
    digitalWrite(LFO2freq3_LED, HIGH);
   }
   else
   {
    digitalWrite(LFO2freq3_LED, LOW);
   }
    
   if(LFO3_vol_set)
   {
    digitalWrite(LFO3Vol_LED, HIGH);
   }
   else
   {
    digitalWrite(LFO3Vol_LED, LOW);
   }

   if(LFO3_pan_set)
   {
    digitalWrite(LFO3Pan_LED, HIGH);
   }
   else
   {
    digitalWrite(LFO3Pan_LED, LOW);
   }   

   if(IS_ON)
    {
     digitalWrite(engage_LED,HIGH);
    }
   else
   {
     digitalWrite(engage_LED,LOW);
   }
}

void VSSD() //Venus Special Startup Dance
{
allLEDsOFF();
  
 int pause{75};
 int LED_Matrix[10] = { PWM_L , PWM_R, midi_LED, LFO2Vol_LED, LFO2Pan_LED, LFO2freq1_LED, LFO2freq3_LED, LFO3Vol_LED, LFO3Pan_LED, engage_LED, };

 for (int i=0; i<11; i++){

if(i < 2){
  

  digitalWrite(LED_Matrix[i],HIGH);
  delay(pause);
  }

else if(i < 10){
  digitalWrite(LED_Matrix[i],HIGH);
  digitalWrite(LED_Matrix[i-2],LOW);
  delay(pause);
}
else{
  digitalWrite(LED_Matrix[i-2],LOW);
  delay(10*pause);
}

}
allLEDsOFF();
delay(1000);
digitalWrite(LED_Matrix[9],HIGH);
delay(1000);
digitalWrite(LED_Matrix[9],LOW);
delay(1000);
resetLEDs();

}
