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

Serial.print("freq1scaled: ");
Serial.println(freq1scaled);

    if(freqMode ==1){
      float freq2scaled{};
      float freq3scaled{};
      
      freq2scaled = freq1scaled * ratio2;
      updatefreq2(freq2scaled);

      freq3scaled = freq1scaled * ratio3;
      updatefreq3(freq3scaled);
    }



}

void updatefreq2(float freq2scaled)
{
      //period2 = constants::samplerate/(2 * freq2scaled/100);
      period2 = constants::samplerate/(2 * freq2scaled);
      wave2Inc = float(constants::waveLength - 1)/period2; //float 
      Serial.print("freq2scaled: ");
      Serial.println(freq2scaled);

}

void updatefreq3(float freq3scaled)
{
      //period3 = constants::samplerate/(2* freq3scaled/100);
      period3 = constants::samplerate/(2* freq3scaled);
      wave3Inc = float(constants::waveLength - 1)/period3;
      Serial.print("freq3scaled: ");
      Serial.println(freq3scaled);

}
