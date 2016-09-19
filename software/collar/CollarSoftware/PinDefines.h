#ifndef PinDefines_h
#define PinDefines_h

	#define DAC_OUT     A0
	#define AMP_EN      A2
	#define AUX_0       9
	#define AUX_1       8
	#define AUX_2       A4
	#define GPS_FIX     3
	//define GPS_TX,    RX = Serial
	#define FLASH_CS    ATN
	#define RF_INT      2
	#define RF_CS       5
	#define MPU_INT     11
	#define CHARGE_S1   26 //PIN_LED_TXL
	#define CHARGE_S2   A5
	#define CHARGE_PG   25 //PIN_LED_RXL

	#define GPSSerial 	Serial1

	//MPU 0x68, POT 0x2F
	//GPS_TX, RX = Serial1
	//Serial Monitor = SerialUSB
	
	#include "Geofence.h"

	//Move this to retrieving from the flash memory
	extern uint8_t NODE_ADDRESS;
	extern uint8_t distThresh;
	extern uint8_t motionThresh;
	extern bool testing;
	extern int16_t magbias0;
	extern int16_t magbias1;
	extern int16_t magbias2;

	extern int polyCorners;
    extern struct position fencePoints[255];
    extern uint8_t fenceversion;

    extern int test;


#endif