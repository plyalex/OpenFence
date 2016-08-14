// ***** INCLUDES *****
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <RTCZero.h>
#include "PA6C.h"
#include "Geofence.h"

// ***** CONSTANTS ***** 
#define MOBILE_NODE_ADDRESS 128 
#define BASE_STATION_ADDRESS 1

#define UpdateDistance 1 


/* Create an rtc object */
RTCZero rtc;

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 0;
const byte hours = 16;

/* Change these values to set the current initial date */
const byte day = 15;
const byte month = 6;
const byte year = 15;


// ***** PIN DEFINITION ***** 
// ***** MINI ULTRA PRO CONFIGURATIONS ***** 
const int radioInterruptPin = 2;  // D2 is used as radio interrupt pin 
const int flashChipSelectPin = 4; // D4 is used as serial flash chip select pin
const int radioChipSelectPin = 5; // D5 is used as radio chip select pin 
const int ledPin = 13;            // D13 has an LED 

#define PIN_DAC0 A0

// Singleton instance of the radio driver 
RH_RF95 radio(radioChipSelectPin, radioInterruptPin); 
// Class to manage message delivery and receipt, using the driver declared above 
RHReliableDatagram manager(radio, MOBILE_NODE_ADDRESS); 

PA6C gps;
Geofence fence;

struct datapacket{
  float lat;
  float lon;
  uint32_t gpstime;
  uint32_t date;
  uint16_t alerts; 
  uint16_t shocks;
};

//Global Variables
struct position p, pTransmitted;
struct position fencePts[256];
int fenceCorners = 4;
int alerts = 0;
int shocks = 0;

void setup() {
  // GPS Start
  Serial.begin(115200);

    // Ensure serial flash is not interfering with radio communication on SPI bus 
  pinMode(flashChipSelectPin, OUTPUT); 
  digitalWrite(flashChipSelectPin, HIGH); 
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, LOW); 
  SerialUSB.begin(115200); 
  SerialUSB.println("Testing"); 
  // If radio initialization fail 
  if (!manager.init()) 
  { 
    SerialUSB.println("Init failed"); 
  } 
  
  // ///< Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range, 915 MHz, 10 dBm on RFM95W 
  radio.setModemConfig(RH_RF95::Bw500Cr45Sf128); 
  radio.setFrequency(915.0); 
  radio.setTxPower(10); 
  

  //Wait for GPS Fix...
  pTransmitted.lat=10;
  pTransmitted.lon=10;

  //Some Test Points
  fencePts[0].lat = -37.911318; 
  fencePts[0].lon = 145.138143;
  fencePts[1].lat = -37.911462;
  fencePts[1].lon = 145.139120;
  fencePts[2].lat = -37.912300;
  fencePts[2].lon = 145.138902;
  fencePts[3].lat = -37.912135;
  fencePts[3].lon = 145.137907;

  analogWriteResolution(2);

  rtc.begin(); // initialize RTC
  
  // Set the time
  rtc.setHours(hours);
  rtc.setMinutes(minutes);
  rtc.setSeconds(seconds);
  
  // Set the date
  rtc.setDay(day);
  rtc.setMonth(month);
  rtc.setYear(year);
 
}

int dacValue = 0;

uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; 
uint8_t buffer[sizeof(datapacket)];
struct datapacket d;


void loop() {

  //gps.getGPRMC();
  //p.lat=gps.getLatitude();
  //p.lon=gps.getLongitude();  
  
  //me.lat = -37.911779; //Inside
  //me.lon = 145.138567;
  //me.lat = -37.911643; //Just Outside 
  //me.lon = 145.137766;
  //me.lat = -37.913647; //Well outside
  //me.lon = 145.137045;
  byte seconds=rtc.getSeconds();
  if(seconds < 10){     //Inside
    d.lat=-37.911765; 
    d.lon=145.138300;
    d.gpstime=51810;
    d.date=240516;
  }else if(seconds <20){
    d.lat=-37.911643; 
    d.lon= 145.137766;
    d.gpstime=51820;
    d.date=240516;
  }else if(seconds <30){
    d.lat=-37.911623;
    d.lon=145.137901;
    d.gpstime=51830;
    d.date=240516;
  }else if(seconds <40){
    d.lat=-37.911520;
    d.lon=145.138454;
    d.gpstime=51840;
    d.date=240516;
  }else if(seconds <50){
    d.lat=-37.911997;
    d.lon=145.138565;
    d.gpstime=51850;
    d.date=240516;
  }else if(seconds <60){
    d.lat=-37.912090;
    d.lon=145.138230;
    d.gpstime=51860;
    d.date=240516;
  }
  p.lat=d.lat;
  p.lon=d.lon;

  if(fence.distance(p,pTransmitted) > UpdateDistance){
    sendPosition();  //Check for errors
  }

 
  float distance = fence.pointInPolygon(p,fencePts,fenceCorners);
  SerialUSB.println(distance);

  if(distance>0){
      //Audio
      audioout();
      alerts++;
    }else if(distance>2){
      //Louder Audio
      audioout();
      alerts++;
    }else if(distance>4){
      //Loudest Audio
      audioout();
      alerts++;
    }else if(distance>6){
      //Loudest Audio
      audioout();
      //Small Shock
      shocks++;
    }else if(distance>8){
      //Loudest Audio
      audioout();
      //Bigger Shock
      shocks++;
    }else{
      //New Containment Fence
    }
    // delay(200);
    // gps.getGPRMC();
    // p.lat=gps.getLatitude();
    // p.lon=gps.getLongitude();  
    // distance = fence.pointInPolygon(p,fencePts,fenceCorners);
    // sendPosition();  //Check for errors
  

  
  delay(500);

}
void audioout(){
  byte seconds=rtc.getSeconds();
  byte seconds2=rtc.getSeconds();
  while(seconds == seconds2){
    int dacValue = !dacValue;
    delayMicroseconds(600);
    // Generate a voltage between 0 and 3.3V.
    analogWrite(PIN_DAC0, dacValue);
    seconds2=rtc.getSeconds();
  }
}

int sendPosition() {
  //pTransmitted.lat=gps.getLatitude();
  //pTransmitted.lon=gps.getLongitude();
  pTransmitted.lat=d.lat;
  pTransmitted.lon=d.lon;
  //d.lat=gps.getLatitude();
  //d.lon=gps.getLongitude();
  //d.gpstime=gps.getTime();
  //d.date=gps.getDate();
  d.alerts=alerts;
  d.shocks=shocks;


  memcpy(buffer+0, &d.lat, 4);
  memcpy(buffer+4, &d.lon, 4);
  memcpy(buffer+8, &d.gpstime, 4);
  memcpy(buffer+12, &d.date, 4);
  memcpy(buffer+16, &d.alerts, 2);
  memcpy(buffer+18, &d.shocks, 2);
  radio.setHeaderFlags(0x1,0x0F);
  return phoneHome();

}

int  phoneHome(){
  // Send message to base station 
  if (manager.sendtoWait(buffer, sizeof(buffer), BASE_STATION_ADDRESS)) 
  { 
    uint8_t len = sizeof(buf); 
    uint8_t from; 
    uint8_t* bufPtr; 
    
    // Now wait for a reply from the base station 
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from)) 
    { 
      digitalWrite(ledPin, HIGH); 
      SerialUSB.print("RX: "); 
      bufPtr = buf; 
      while(len-- > 0) 
      {
        SerialUSB.write(*bufPtr++); 
      } 
      SerialUSB.println(); 
      delay(200); 
      digitalWrite(ledPin, LOW); 
    } 
    else 
    { 
      return 1; // SerialUSB.println("Base station not responding!"); 
    } 
  } else return 1; //SerialUSB.println("sendtoWait failed");  
  return 0;
}
