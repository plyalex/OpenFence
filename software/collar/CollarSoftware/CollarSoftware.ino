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
uint8_t distThresh = 2;
uint8_t motionThresh = 3;
bool testing = true;
int16_t magbias0 = -270;
int16_t magbias1 = 168;
int16_t magbias2 = -70;

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

  SerialUSB.print(magbias0); SerialUSB.print("\t");
  SerialUSB.print(magbias1); SerialUSB.print("\t");
  SerialUSB.print(magbias2); SerialUSB.println("\t");

  init_mpu();


// Re enable me for sleep mode??
 // mpu9250.wakeOnmotion(motionThresh);
 // mpu9250.readStatus();


  // while(1){
  //   float compass = mpu9250.getHeading();
  //   compass = compass + 11.716667;
  //   SerialUSB.println(compass);
  //   // while (GPSSerial.available())
  //   //   SerialUSB.write(GPSSerial.read());
  //   delay(200);
  // }

  // int vol = 50;
  // while(1){
  //   SerialUSB.print("Volume: "); SerialUSB.println(vol);
  //   audio.enableAmp(); 
  //   audio.setvolumeBoth(vol); 
  //   delay(1500);
  //   vol=vol+2; 
  //   audio.disableAmp(); 
  //   delay(1000);
  // }

  testing = 1;
  // motionThresh = 3;

  

  SerialUSB.print(NODE_ADDRESS); SerialUSB.print("\t");
  SerialUSB.print(polyCorners); SerialUSB.print("\t");
  SerialUSB.print(fenceversion); SerialUSB.print("\t");
  SerialUSB.print(fencePoints[0].lat,7); SerialUSB.print("\t");
  SerialUSB.print(fencePoints[1].lat,7); SerialUSB.print("\t");
  SerialUSB.print(fencePoints[2].lat,7); SerialUSB.print("\t");
  SerialUSB.println(fencePoints[3].lat,7);
  SerialUSB.print(magbias0); SerialUSB.print("\t");
  SerialUSB.print(magbias1); SerialUSB.print("\t");
  SerialUSB.print(magbias2); SerialUSB.println("\t");

  SerialUSB.println("Setup Complete");
  audio.enableAmp(); audio.setvolumeBoth(20); delay(500); audio.disableAmp(); // Short Beep

  if(testing){
    audio.enableAmp(); audio.setvolumeBoth(20); delay(500); audio.disableAmp(); //Beep Beep
    delay(500); audio.enableAmp(); delay(500); audio.disableAmp();
    me.lat=-37.909507; 
    me.lon=145.135128;
    nowdt.time=60005;
    nowdt.date=191016;
    metransmitted=me;
  }

  if(!testing){
    SerialUSB.print("Getting GPS Fix");
    while(!gps.location.isValid())
      smartDelay(500);
    SerialUSB.println("Fix Obtained");
    updateMeNowDT();
    metransmitted=me;

    audio.enableAmp(); audio.setvolumeBoth(20); delay(500); audio.disableAmp(); //Beep Beep
    delay(500); audio.enableAmp(); delay(500); audio.disableAmp();
  }
}

void loop() {
  

  //battery.printStatus();
  //SerialUSB.println(battery.getStatus(),BIN);
  //SerialUSB.println(fencePoints[0].lat);

  if(!testing){
    updateMeNowDT();
  // } else {
  //   byte seconds=rtc.getSeconds();
  //   if(seconds < 10){     //Inside
  //     me.lat=-37.911765; 
  //     me.lon=145.138300;
  //     nowdt.time=51810;
  //     nowdt.date=240516;
  //   }else if(seconds <20){
  //     me.lat=-37.911643; 
  //     me.lon= 145.137766;
  //     nowdt.time=51820;
  //     nowdt.date=240516;
  //   }else if(seconds <30){
  //     me.lat=-37.911623;
  //     me.lon=145.137901;
  //     nowdt.time=51830;
  //     nowdt.date=240516;
  //   }else if(seconds <40){
  //     me.lat=-37.911520;
  //     me.lon=145.138454;
  //     nowdt.time=51840;
  //     nowdt.date=240516;
  //   }else if(seconds <50){
  //     me.lat=-37.911997;
  //     me.lon=145.138565;
  //     nowdt.time=51850;
  //     nowdt.date=240516;
  //   }else if(seconds <60){
  //     me.lat=-37.912090;
  //     me.lon=145.138230;
  //     nowdt.time=51900;
  //     nowdt.date=240516;
  //   }
  // }
  } else {
    byte seconds=rtc.getSeconds();
    if(seconds < 5){     //Inside
      me.lat=-37.909507; 
      me.lon=145.135128;
      nowdt.time=60005;
      nowdt.date=191016;
    }else if(seconds <10){ //inside
      me.lat=-37.909630; 
      me.lon= 145.135133;
      nowdt.time=60010;
      nowdt.date=191016;
    }else if(seconds <15){ //outside
      me.lat=-37.909624; 
      me.lon=145.135013;
      nowdt.time=60015;
      nowdt.date=191016;
    }else if(seconds <20){ //outside
      me.lat=-37.909623; 
      me.lon=145.134983;
      nowdt.time=60020;
      nowdt.date=191016;
    }else if(seconds <25){ //outside
      me.lat=-37.909623; 
      me.lon=145.134972;
      nowdt.time=60025;
      nowdt.date=191016;
    }else if(seconds <30){ //outside
      me.lat=-37.909673; 
      me.lon=145.134976;
      nowdt.time=60030;
      nowdt.date=191016;
    }else if(seconds <35){ //outside
      me.lat=-37.909704; 
      me.lon=145.135008;
      nowdt.time=60035;
      nowdt.date=191016;
    }else if(seconds <40){ //inside
      me.lat=-37.909729; 
      me.lon= 145.135105;
      nowdt.time=60040;
      nowdt.date=191016;
    }else if(seconds <45){ //inside
      me.lat=-37.909750;
      me.lon=145.135287;
      nowdt.time=60005;
      nowdt.date=191016;
    }else if(seconds <50){ //inside
      me.lat=-37.909647; 
      me.lon=145.135288;
      nowdt.time=60050;
      nowdt.date=191016;
    }else if(seconds <55){ //inside
      me.lat=-37.909506; 
      me.lon=145.135305;
      nowdt.time=60055;
      nowdt.date=191016;
    }
  }

  if(fence.distance(me, metransmitted) > distThresh){
    sleepCounter = 0; //Recent Movement
    //SerialUSB.println("Sending Packet");
    if(lora.sendPosition(me, nowdt, alerts, shocks, fenceversion)){
      // audio.enableAmp(); audio.setvolumeBoth(20); delay(500); audio.disableAmp(); //Beep Beep Failed to Send
      // delay(500); audio.enableAmp(); delay(500); audio.disableAmp();
    } else {
      metransmitted=me;
    }
  }



  fenceProperty result=fence.geofence(me, fencePoints, polyCorners);

  float compass = mpu9250.getHeading();

  
  SerialUSB.println(compass);


  static bool outside;

  if(result.distance > 0){
    SerialUSB.print("Side: \t");
    SerialUSB.print(result.sideOutside);
    SerialUSB.print("\t Distance: \t");
    SerialUSB.print(result.distance);
    SerialUSB.print("\t Azimuth: \t");
    SerialUSB.print(result.bearing);
    SerialUSB.print("\t Compass: \t");
    SerialUSB.print(compass);
    SerialUSB.println("\t C+var-A: \t");


    if(!outside){
      alerts++;
      outside=1;
    } 
    
    int volume = result.distance * 10; //10;
    //if(volume>10) volume=10;
    if(volume>50) volume=50;
    audio.enableAmp();
    audio.setvolumeBoth(volume);
    // delay(1000);
    // audio.disableAmp();

    int val = compass + 11.716667 + result.bearing;
    while(abs(val)>180){
      if(val > 180) val=val-360;
      else if(val < -180) val=val+360;
    }
    SerialUSB.println(val);

    if(val>0){
      if(val>90){
        audio.setvolumeBoth(0);
        audio.disableAmp();
      }
      else{
        //Sound RHS
        audio.enableAmp();
        audio.setvolumeLeft(0);
        audio.setvolumeRight(volume);
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
        audio.setvolumeRight(0);
        audio.setvolumeLeft(volume);
      } 
    }
  }else{
    outside=0;
    audio.setvolumeBoth(0);
    audio.disableAmp();
  }

  smartDelay(500);
  
  sleepCounter++;
  if(sleepCounter > 15 & !outside & !testing){
    sleepCounter=0;
    //audio.enableAmp(); audio.setvolumeBoth(20); delay(1000); audio.disableAmp(); // Long Beep    
    standby();
    //audio.enableAmp(); audio.setvolumeBoth(20); delay(500); audio.disableAmp(); // Short Beep
  }

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
  delay(100);  //Ensure the GPS Serial Message has time to complete before sleeping
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
  delay(100);
  //USBDevice.detach();
  
  standbyMode();

  detachInterrupt(11);
  USBDevice.attach();
  standbyGPS();  //wakes GPS back up

  return;
}



