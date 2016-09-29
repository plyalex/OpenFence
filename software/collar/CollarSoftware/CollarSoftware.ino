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


#define GPSTestPoints
uint8_t NODE_ADDRESS = 103;
uint8_t distThresh = 5;
uint8_t motionThresh = 3;
bool testing = true;
int16_t magbias0 = 0;
int16_t magbias1 = 0;
int16_t magbias2 = 0;

uint8_t polyCorners = 0;
struct position fencePoints[256];
uint8_t fenceversion = 0;

MCP73871 battery;
RTCZero rtc;
TinyGPSPlus gps;
MPU9250 mpu9250;
Audio_OF audio;
LoRa_OF lora;
Flash_OF flash;
Geofence fence;


// magbias[]={-90,400,0}; //Float in MPU9250.h library



position me, metransmitted;
datetime nowdt;
int alerts = 0;
int shocks = 0;
int sleepCounter = 0;


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
  // magbias0=-25;
  // magbias1=512;
  // magbias2=-457;

  //mpu9250.wakeOnmotion(0x02);
  // SerialUSB.println(magbias0);
  // while(1){
  //   float compass = mpu9250.getHeading();
  //   SerialUSB.println(compass);
  //   delay(100);
  // }

  //attachInterrupt(11, wake, HIGH); //This breaks compass if not wakeOnMotion

  SerialUSB.println("Setup Complete");
  audio.enableAmp(); audio.setvolumeBoth(20); delay(500); audio.disableAmp(); // Short Beep

  if(testing){
    audio.enableAmp(); audio.setvolumeBoth(20); delay(500); audio.disableAmp(); //Beep Beep
    delay(500); audio.enableAmp(); delay(500); audio.disableAmp();
  }

  if(!testing){
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
  }
}

void loop() {
  

  battery.printStatus();
  //SerialUSB.println(battery.getStatus(),BIN);
  //SerialUSB.println(fencePoints[0].lat);


  if(!testing){
    updateMeNowDT();
  } else {
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
  }



  if(fence.distance(me, metransmitted) > distThresh){
    sleepCounter = 0; //Recent Movement
    metransmitted=me;
    SerialUSB.println("Sending Packet");
    lora.sendPosition(me, nowdt, alerts, shocks, fenceversion);
  }


  // SerialUSB.print(NODE_ADDRESS); SerialUSB.print("\t");
  // SerialUSB.print(polyCorners); SerialUSB.print("\t");
  // SerialUSB.print(fenceversion); SerialUSB.print("\t");
  // SerialUSB.print(fencePoints[0].lat,6); SerialUSB.print("\t");
  // SerialUSB.print(fencePoints[1].lat,6); SerialUSB.print("\t");
  // SerialUSB.print(fencePoints[2].lat,6); SerialUSB.print("\t");
  // SerialUSB.println(fencePoints[3].lat,6);




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
    int volume = 10;//result.distance * 100;
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

  smartDelay(1000);
  
  // sleepCounter++;
  // if(sleepCounter > 1000 ){ // & !outside & !testing){
  //   sleepCounter=0;
  //   standby();
  //   audio.initAudio();
  //   audio.enableAmp(); audio.setvolumeBoth(20); delay(500); audio.disableAmp(); // Short Beep
  // }

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

void standby(){
  standbyGPS();
  lora.sleep();
  audio.disableAmp();
  mpu9250.wakeOnmotion(motionThresh);
  mpu9250.readStatus();
  attachInterrupt(11, wake, HIGH);
  USBDevice.detach();
  
  standbyMode();
  //USBDevice.attach();
  
}



