// ***** INCLUDES *****
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
#include "PA6C.h"

// ***** CONSTANTS ***** 
#define MOBILE_NODE_ADDRESS 128 
#define BASE_STATION_ADDRESS 2 

// ***** PIN DEFINITION ***** 
// ***** MINI ULTRA PRO CONFIGURATIONS ***** 
const int radioInterruptPin = 2;  // D2 is used as radio interrupt pin 
const int flashChipSelectPin = 4; // D4 is used as serial flash chip select pin
const int radioChipSelectPin = 5; // D5 is used as radio chip select pin 
const int ledPin = 13;            // D13 has an LED 

// Singleton instance of the radio driver 
RH_RF95 radio(radioChipSelectPin, radioInterruptPin); 
// Class to manage message delivery and receipt, using the driver declared above 
RHReliableDatagram manager(radio, MOBILE_NODE_ADDRESS); 

PA6C gps;

struct datapacket{
  float lat;
  float lon;
  uint32_t gpstime;
  uint32_t date;
  uint16_t alerts; 
  uint16_t shocks;
};

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
  
}

uint8_t buf[RH_RF95_MAX_MESSAGE_LEN]; 
uint8_t buffer[sizeof(datapacket)];
struct datapacket d;


void loop() {
  gps.getGPRMC();
  d.lat=gps.getLatitude();
  d.lon=gps.getLongitude();
  d.gpstime=gps.getTime();
  d.date=gps.getDate();
  d.shocks=1;
  d.alerts=5;

  memcpy(buffer+0, &d.lat, 4);
  memcpy(buffer+4, &d.lon, 4);
  memcpy(buffer+8, &d.gpstime, 4);
  memcpy(buffer+12, &d.date, 4);
  memcpy(buffer+16, &d.alerts, 2);
  memcpy(buffer+18, &d.shocks, 2);
  

  phoneHome();
  
  delay(500);

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
