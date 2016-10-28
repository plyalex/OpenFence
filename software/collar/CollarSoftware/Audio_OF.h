#ifndef Audio_OF_H
#define Audio_OF_H
 
#include "math.h"
#include "PinDefines.h"
#include <Wire.h>

#define MAXADDR 0x2F  // Address of MAX5387 
#define RegA    0x11  // Potentiometer wiper A
#define RegB    0x12  // Potentiometer wiper B
#define RegAB   0x13  // Both Potentiometer wipers move to the same location

// Create a logarithmically scaling volume from the linear potentiometer
// Each 10% increase in the volume requested doubles the output volume.
static const uint8_t volumeLUT[101]={0,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,4,5,
                                  5,5,6,6,6,7,7,7,8,8,9,9,10,10,11,11,12,13,14,14,15,16,17,
                                  18,19,20,21,22,24,25,26,28,29,31,33,35,37,39,41,43,46,
                                  48,51,54,57,60,64,67,71,75,80,84,89,94,99,105,111,117,
                                  124,131,139,146,155,164,173,183,193,204,216,228,241,255};


class Audio_OF {
 
    protected:
      void enable_pwm() {
        REG_TCC1_CTRLA |= TCC_CTRLA_ENABLE;             // Enable the TCC0 output
        while (TCC1->SYNCBUSY.bit.ENABLE);              // Wait for synchronization
      }

      void disable_pwm() {
        REG_TCC1_CTRLA &= ~TCC_CTRLA_ENABLE;             // Enable the TCC0 output
        while (TCC1->SYNCBUSY.bit.ENABLE);              // Wait for synchronization
      }

    public:
      void initAudio(){
        digitalWrite(AMP_EN,LOW); //Turn off Amp
        setvolumeBoth(0); //Set volume to 0

        clock_init(4, 8); //Divide GCLK4 by a factor of 8, giving 6MHz

        // Enable the port multiplexer for the digital pin D9 
        PORT->Group[g_APinDescription[9].ulPort].PINCFG[g_APinDescription[9].ulPin].bit.PMUXEN = 1;
        
        // Connect the TCC0 timer to digital output D9 - port pins are paired odd PMUO and even PMUXE
        // F & E specify the timers: TCC0, TCC1 and TCC2
        PORT->Group[g_APinDescription[8].ulPort].PMUX[g_APinDescription[8].ulPin >> 1].reg = PORT_PMUX_PMUXO_E;

        // Feed GCLK4 to TCC0 and TCC1
        REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                           GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                           GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
        while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

        // Dual slope PWM operation: timers countinuously count up to PER register value then down 0
        REG_TCC1_WAVE |= TCC_WAVE_POL(0xF) |            // Reverse the output polarity on all TCC1 outputs
                         TCC_WAVE_WAVEGEN_DSBOTH;       // Setup dual slope PWM on TCC1
        while (TCC1->SYNCBUSY.bit.WAVE);                // Wait for synchronization

        // Each timer counts up to a maximum or TOP value set by the PER register,
        // this determines the frequency of the PWM operation: 
        REG_TCC1_PER = 880;         // Set the frequency of the PWM on TCC1 to 850Hz
        while (TCC1->SYNCBUSY.bit.PER);                 // Wait for synchronization
        
        // Set the PWM signal to output 50% duty cycle
        REG_TCC1_CC1 = 440;                             // TCC1 CC1 - on D9
        while (TCC1->SYNCBUSY.bit.CC1);                 // Wait for synchronization
        
        // Divide the 6MHz signal by 8 giving 750kHz (1.333us) TCC0 timer tick and enable the outputs
        REG_TCC1_CTRLA |= TCC_CTRLA_PRESCALER_DIV8 |    // Divide GCLK4 by 8
                          TCC_CTRLA_ENABLE;             // Enable the TCC0 output
        while (TCC1->SYNCBUSY.bit.ENABLE);              // Wait for synchronization
      }

      void clock_init(int ID, int Divisor){
        REG_GCLK_GENDIV = GCLK_GENDIV_DIV(Divisor) |    // Divide the 48MHz clock source by divisor 
                          GCLK_GENDIV_ID(ID);           // Select Generic Clock (GCLK) 
        while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

        REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                           GCLK_GENCTRL_GENEN |         // Enable GCLKx
                           GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                           GCLK_GENCTRL_ID(ID);         // Select GCLKx
        while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
      }

      void enableAmp(){
        digitalWrite(AMP_EN,HIGH);
        enable_pwm();
      }
      void disableAmp(){
        digitalWrite(AMP_EN,LOW);
        disable_pwm();
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
