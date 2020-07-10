#include "freq_update.h"

void updatefreq1(float freq1scaled)
{
    //period1 = constants::samplerate / (freq1scaled / 100);
    period1 = constants::samplerate / (freq1scaled);
    period1a = period1 * swayScaled;
    //period1b = period1 * (1 - swayScaled);
    period1b = period1 - period1a;
    wave1aInc = float(constants::waveLength - 1)/period1a;
    wave1bInc = float(constants::waveLength - 1)/period1b;


}

void updatefreq2(float freq2scaled)
{
      //period2 = constants::samplerate/(2 * freq2scaled/100);
      period2 = constants::samplerate/(2 * freq2scaled);
      wave2Inc = float(constants::waveLength - 1)/period2; //float 
     // Serial.print("freq2scaled: ");
     // Serial.println(freq2scaled);

}

void updatefreq3(float freq3scaled)
{
      //period3 = constants::samplerate/(2* freq3scaled/100);
      period3 = constants::samplerate/(2* freq3scaled);
      wave3Inc = float(constants::waveLength - 1)/period3;
    //  Serial.print("freq3scaled: ");
    //  Serial.println(freq3scaled);

}

void updatefreq1Mode1(float freq1scaled)
{
  Mode1Period = (1.0 / (4.0 * freq1scaled * global_mult )) * 1.0E6; //microseconds
  updateInterval();
}

void updatefreq2Mode1(int freq2)
{

  static int freq2last{2 * constants::in_max};

  freq2 = map(freq2,constants::in_min,constants::in_max,0,constants::numRatios - 1);
  
  if(abs(freq2 - freq2last) > 0){
    LFO2Mult = constants::ratioMults[freq2];
    LFO2Div = constants::ratioDivs[freq2];
    resetPhase();
  }

  freq2last = freq2;
}

void updatefreq3Mode1(int freq3)
{
  static int freq3last{2 * constants::in_max};
  freq3 = map(freq3,constants::in_min,constants::in_max,0,constants::numRatios - 1);

  if(abs(freq3 - freq3last) > 0){
    LFO3Mult = constants::ratioMults[freq3];
    LFO3Div = constants::ratioDivs[freq3];

    resetPhase();
  }

  freq3last = freq3;
}

void resetPhase()
{

  direction1 = constants::down;
  
  if(freqMode == 1){
   direction2 = constants::down;
   direction3 = constants::down;
   t=0;
   t2 = LFO2_offset;
   t3 = LFO3_offset;
  }
    t1=0;
}
