#include "PinDefines.h"
#include "MCP73871.h"
#include "MPU9250.h"
#include "Audio_OF.h"
#include "LoRa_OF.h"
#include "Geofence.h"
#include <Wire.h>
#include <SPI.h>
#include <RTCZero.h>
#include <TinyGPS++.h>
#include "Flash_OF.h"


//#define WAITGPSFIX
#define GPSTestPoints
uint8_t NODE_ADDRESS = 11;
uint8_t distThresh = 5;
uint8_t motionThresh = 3;
bool testing = false;
int16_t magbias0 = 0;
int16_t magbias1 = 0;
int16_t magbias2 = 0;

uint8_t polyCorners = 0;
struct position fencePoints[256];
uint8_t fenceversion = 0;

int test=0;

MCP73871 battery;
RTCZero rtc;
TinyGPSPlus gps;
MPU9250 mpu9250;
Audio_OF audio;
LoRa_OF lora;
Flash_OF flash;
Geofence fence;

//Global Variables - Updatable in Web Interface
int distanceThresholds[]={0, 2, 4, 6, 8, 10}; //Distance in metres
// magbias[]={-90,400,0}; //Float in MPU9250.h library



position me, metransmitted;
datetime nowdt;
int alerts = 0;
int shocks = 0;

void standbyMode();
uint8_t threshhold = 0x04;


void setup() {
  init_pins();
  init_comms();
  delay(3000);
  init_gps();
  audio.initAudio();
  rtc.begin(); // initialize RTC

  fencePoints[0].lon=145.138458;  
  fencePoints[0].lat=-37.911625; 
  fencePoints[1].lon=145.138707;
  fencePoints[1].lat=-37.911662;
  fencePoints[2].lon=145.138653;
  fencePoints[2].lat=-37.911887;
  fencePoints[3].lon=145.138399; 
  fencePoints[3].lat=-37.911850;
  polyCorners=4;

  delay(1000);
  lora.initLoRa();
  flash.init();  
  lora.setAddress();

  init_mpu();
  //mpu9250.wakeOnmotion(0x02);

  // while(1){
  //   float compass = mpu9250.getHeading();
  //   SerialUSB.println(compass);
  //   delay(100);
  // }




  SerialUSB.println("Setup Complete");

  audio.enableAmp(); audio.setvolumeBoth(20); delay(500); audio.disableAmp(); // Short Beep


  #ifdef WAITGPSFIX
  SerialUSB.print("Getting GPS Fix");
  bool gpsfix=0;
  while(!gpsfix){
    bool temp=digitalRead(GPS_FIX);
    delay(1000);
    bool temp1=digitalRead(GPS_FIX);
    if(temp == 0 && temp1 == 0) gpsfix=1;
    SerialUSB.print(".");
  }
  SerialUSB.println("Fix Obtained");
  audio.enableAmp(); audio.setvolumeBoth(20); delay(500); audio.disableAmp(); //Beep Beep
  delay(500); audio.enableAmp(); delay(500); audio.disableAmp();
  #endif

  test = 0;
}


int dacValue = 0;
int i=0;

void loop() {
  

  //battery.printStatus();


  // if(mpu9250.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {  // On interrupt, check if data ready interrupt

  //   mpu9250.readAccelData(accelCount);  // Read the x/y/z adc values   
  //   // Now we'll calculate the accleration value into actual g's
  //   ax = (float)accelCount[0]*aRes - accelBias[0];  // get actual g value, this depends on scale being set
  //   ay = (float)accelCount[1]*aRes - accelBias[1];   
  //   az = (float)accelCount[2]*aRes - accelBias[2];  
   
  //   mpu9250.readGyroData(gyroCount);  // Read the x/y/z adc values
  //   // Calculate the gyro value into actual degrees per second
  //   gx = (float)gyroCount[0]*gRes - gyroBias[0];  // get actual gyro value, this depends on scale being set
  //   gy = (float)gyroCount[1]*gRes - gyroBias[1];  
  //   gz = (float)gyroCount[2]*gRes - gyroBias[2];   
  
  //   mpu9250.readMagData(magCount);  // Read the x/y/z adc values   
  //   // Calculate the magnetometer values in milliGauss
  //   // Include factory calibration per data sheet and user environmental corrections
  //   mx = (float)magCount[0]*mRes*magCalibration[0] - magbias[0];  // get actual magnetometer value, this depends on scale being set
  //   my = (float)magCount[1]*mRes*magCalibration[1] - magbias[1];  
  //   mz = (float)magCount[2]*mRes*magCalibration[2] - magbias[2];   
  // }



#ifndef GPSTestPoints
  updateMeNowDT();
#endif

#ifdef GPSTestPoints
  byte seconds=rtc.getSeconds();
  if(seconds < 10){     //Inside
    me.lat=-37.911765; 
    me.lon=145.138300;
    nowdt.time=51810;
    nowdt.date=240516;
  }else if(seconds <20){
    me.lat=-37.911643; 
    me.lon= 145.137766;
    nowdt.time=51820;
    nowdt.date=240516;
  }else if(seconds <30){
    me.lat=-37.911623;
    me.lon=145.137901;
    nowdt.time=51830;
    nowdt.date=240516;
  }else if(seconds <40){
    me.lat=-37.911520;
    me.lon=145.138454;
    nowdt.time=51840;
    nowdt.date=240516;
  }else if(seconds <50){
    me.lat=-37.911997;
    me.lon=145.138565;
    nowdt.time=51850;
    nowdt.date=240516;
  }else if(seconds <60){
    me.lat=-37.912090;
    me.lon=145.138230;
    nowdt.time=51900;
    nowdt.date=240516;
  }
#endif


  if(fence.distance(me, metransmitted) > distThresh){
    metransmitted=me;
    SerialUSB.println("Sending Packet");
    lora.sendPosition(me, nowdt, alerts, shocks, fenceversion);
  }
   

  SerialUSB.print(NODE_ADDRESS); SerialUSB.print("\t");
  SerialUSB.print(polyCorners); SerialUSB.print("\t");
  SerialUSB.print(fenceversion); SerialUSB.print("\t");
  SerialUSB.print(fencePoints[0].lat,6); SerialUSB.print("\t");
  SerialUSB.print(fencePoints[1].lat,6); SerialUSB.print("\t");
  SerialUSB.print(fencePoints[2].lat,6); SerialUSB.print("\t");
  SerialUSB.println(fencePoints[3].lat,6);




  fenceProperty result=fence.geofence(me, fencePoints, polyCorners);
  float compass = mpu9250.getHeading();

  // SerialUSB.println(compass);
  // SerialUSB.println(result.sideOutside);
  // SerialUSB.println(result.distance);
  // SerialUSB.println(result.bearing);

  static bool outside;

  if(result.distance > 0){

    if(!outside){
      alerts++;
      outside=1;
    } 
    
    int val = compass-result.bearing;
    int volume = 10; //result.distance * 100;
    if(volume>100) volume=100;

    if(val > 180) val=val-360;
    else if(val < -180) val=val+360;
    SerialUSB.println(val);
    if(val>0){
      if(val>90){
        audio.setvolumeBoth(0);
        audio.disableAmp();
      }
      else{
        //Sound RHS
        audio.enableAmp();
        audio.setvolumeRight(volume);
        audio.setvolumeLeft(0);
      } 
    }
    else{
      if(val < -90){
        audio.setvolumeBoth(0);
        audio.disableAmp();
      }
      else{
        //Sound LHS
        audio.enableAmp();
        audio.setvolumeLeft(volume);
        audio.setvolumeRight(0);

      } 
    }
  }else{
    outside=0;
  }


  // SerialUSB.print(digitalRead(MPU_INT)); SerialUSB.print("\t Int Status: ");
  // if(digitalRead(MPU_INT)){
  //   SerialUSB.println(mpu9250.readStatus(),HEX);
  //   mpu9250.wakeOnmotion();
    
  // }
  // else SerialUSB.println("No Interrupt");

  //delay(1000);
  smartDelay(1000);
}


static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (GPSSerial.available())
      gps.encode(GPSSerial.read());
  } while (millis() - start < ms);
}

void updateMeNowDT(){
  if (gps.location.isValid())
  {
    me.lat=gps.location.lat();
    me.lon=gps.location.lng();
  }
  if (gps.date.isValid())
  {
    nowdt.date=gps.date.value();
  }      
  if (gps.time.isValid())
  {
    nowdt.time=gps.time.value()/100; //Divide by 100 to remove fractions of a second
  } 
}

void standbyGPS(){
        GPSSerial.println("$PMTK161,0*28"); //Send GPS to Standby (wake with any char)
}

void standby(){
  standbyGPS();
  delay(10000);
  lora.sleep();
  delay(10000);
  audio.disableAmp();
  delay(10000);
  mpu9250.wakeOnmotion(motionThresh);
  mpu9250.readStatus();
  attachInterrupt(11, wake, HIGH);
  USBDevice.detach();
  
  delay(10000);
  standbyMode();
  delay(10000);
  
}


void standbyMode()
{
  

  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __DSB();
  __WFI();
  return;
}

void wake(){
  mpu9250.readStatus();
  return;
}


// void standbyMode(){
//   // Entering standby mode when connected
//   // via the native USB port causes issues.
//   NVMCTRL->CTRLB.bit.SLEEPPRM = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val;
//   SCB->SCR |=  SCB_SCR_SLEEPDEEP_Msk;
//   // Set the EIC (External Interrupt Controller) to wake up the MCU
//   // on an external interrupt from digital pin 0. (It's EIC channel 11)
//   EIC->WAKEUP.reg = EIC_WAKEUP_WAKEUPEN0;
//   mpu9250.wakeOnmotion(threshhold);
//   mpu9250.readStatus();
//   attachInterrupt(11, wake, HIGH);
//   __WFI();
//   //Continues from here:
//   // Try one of these: board_init(); SystemInit();
//   //SerialUSB.println("Awake!");
//   return;
// }

// void wake(){
//   REG_EIC_INTFLAG = EIC_INTFLAG_EXTINT0;
//   detachInterrupt(11);
//     //SerialUSB.begin(115200);
//     //SerialUSB.println("Interrupt!");
//   mpu9250.readStatus();
//   return;
// }
