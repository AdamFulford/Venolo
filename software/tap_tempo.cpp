  #include "tap_tempo.h"
    
    void tap_tempo()  //called when either tap button pressed or midi tap input received
{
    static unsigned long tap_length{}; 
    static unsigned long last_time{};
    static unsigned long last_tap_length{};
    static bool tap_flag{};

    tap_length = millis() - last_time;  //tap_length is difference between current and last time

    if (constants::min_tap <= tap_length && tap_length <= constants::max_tap){
      float freq1scaled{};
      if (tap_flag){ //flag set - one previous tap registered, this is the second tap
        tap_flag = false; //reset flag
        
        //re-calculate values for DACupdate()
        freq1scaled = 1 / (tap_length/1000.0);
        period1 = constants::samplerate/(freq1scaled);
        period1a = period1 * swayScaled;
        period1b = period1 * (1 - swayScaled);
        wave1aInc = float(constants::waveLength)/period1a;
        wave1bInc = float(constants::waveLength)/period1b;
        
        last_tap_length = tap_length;

        //reset LFO1 to peak and direction to down
        t1 = 0;     
        direction1 = constants::down; 
      }
      else{ //flag not set
        
        //if tap_length within tolerance then set tap_flag (to say this is first tap) and reset LFO (but do not change tempo)
        if(tap_length >= (constants::tap_tolerance * last_tap_length)){  
          tap_flag = true;
          
          //reset LFO1 to peak and direction to down
          t1 = 0;
          direction1 = constants::down; 
        }
        else{
          //re-calculate values for DACupdate()
          freq1scaled = 1 / (tap_length/1000.0);
          period1 = constants::samplerate/(freq1scaled);
          period1a = period1 * swayScaled;
          period1b = period1 * (1 - swayScaled);
          wave1aInc = float(constants::waveLength)/period1a;
          wave1bInc = float(constants::waveLength)/period1b;

          //update last_tap_length
          last_tap_length = tap_length;
          
          //reset LFO1 to peak and direction to down
          t1 = 0;
          direction1 = constants::down; 
          
        }

      }
    }
     //if tap length too long then set tap_flag (to say this is first tap) and reset LFO
    else if (tap_length > constants::max_tap){
      tap_flag = true;
      
      t1 = 0;   //reset LFO1 to peak
      direction1 = constants::down; 
    }
    //ignore if tap time too short
    else if (tap_length < constants::min_tap){  
      //do nothing
    }

    last_time = millis();   //always update last time
    }
