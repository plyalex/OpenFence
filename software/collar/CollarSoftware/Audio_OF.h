#ifndef Audio_OF_H
#define Audio_OF_H
 
#include "math.h"
#include "PinDefines.h"
#include <Wire.h>

#define MAXADDR 0x2F 
#define RegA    0x11
#define RegB    0x12
#define RegAB   0x13

static const uint8_t volumeLUT[100]={0,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,4,5,
                                  5,5,6,6,6,7,7,7,8,8,9,9,10,10,11,11,12,13,14,14,15,16,17,
                                  18,19,20,21,22,24,25,26,28,29,31,33,35,37,39,41,43,46,
                                  48,51,54,57,60,64,67,71,75,80,84,89,94,99,105,111,117,
                                  124,131,139,146,155,164,173,183,193,204,216,228,241,255};

class Audio_OF {
 
    protected:
 
    public:
      void initAudio(){
        digitalWrite(AMP_EN,LOW); //Turn off Amp
        setvolumeBoth(0); //Set volume to 0
        analogWriteResolution(2);
      }

      void enableAmp(){
        digitalWrite(AMP_EN,HIGH);
      }

      void setvolumeRight(int volume){
        Wire.beginTransmission(MAXADDR);
        Wire.write(RegA);
        Wire.write(volumeLUT[volume]);
        Wire.endTransmission();
      }

      void setvolumeLeft(int volume){
        Wire.beginTransmission(MAXADDR);
        Wire.write(RegB);
        Wire.write(volumeLUT[volume]);
        Wire.endTransmission();
      }

      void setvolumeBoth(int volume){
        Wire.beginTransmission(MAXADDR);
        Wire.write(RegAB);
        Wire.write(volumeLUT[volume]);
        Wire.endTransmission();
      }


  
  

  };
#endif


