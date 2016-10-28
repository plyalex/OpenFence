#ifndef LoRa_OF_H
#define LoRa_OF_H
 
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include "Geofence.h"
#include "PinDefines.h"
#include "Flash_OF.h"

#define BASE_STATION_ADDRESS 1


// Singleton instance of the radio driver 
RH_RF95 radio(RF_CS, RF_INT); 
// Class to manage message delivery and receipt, using the driver declared above 
RHReliableDatagram manager(radio, NODE_ADDRESS); 


struct datapacket0{
  float lat;
  float lon;
  uint32_t gpstime;
  uint32_t date;
  uint16_t alerts; 
  uint8_t shocks;
  uint8_t ver;
};

struct datapacket1{
  uint8_t ver;
  uint8_t last;
  uint8_t numPts;
  uint8_t X;
  float lat0;
  float lon0;
  float lat1;
  float lon1;
};

struct datapacket2{ //Settings
  uint8_t New_RF_ID;
  uint8_t distThresh;
  uint8_t motionThresh;
  bool testing;
  int16_t magbias0;
  int16_t magbias1;
  int16_t magbias2;
};

//Global Variables
uint8_t receiveBuffer[RH_RF95_MAX_MESSAGE_LEN]; 
uint8_t transmitBuffer[sizeof(datapacket0)];


class LoRa_OF {
 
    protected:
 
    public:

    void initLoRa() {      
      if (!manager.init()) 
      { 
        SerialUSB.println("Init failed"); 
      } 
      // ///< Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range, 915 MHz, 10 dBm on RFM95W 
      radio.setModemConfig(RH_RF95::Bw500Cr45Sf128); 
      radio.setFrequency(915.0); 
      radio.setTxPower(5); 
    }

    bool sleep(){
      return radio.sleep();
    }


    int sendBias() {
      
      memcpy(transmitBuffer+0,  &magbias0, 2);
      memcpy(transmitBuffer+2,  &magbias1, 2);
      memcpy(transmitBuffer+4,  &magbias2, 2);
      radio.setHeaderFlags(0x3,0x0F);
      return phoneHome();

    }

    int sendPosition(struct position me, struct datetime now, uint16_t alerts, uint8_t shocks , uint8_t version) {

      memcpy(transmitBuffer+0,  &me.lat,   4);
      memcpy(transmitBuffer+4,  &me.lon,   4);
      memcpy(transmitBuffer+8,  &now.time, 4);
      memcpy(transmitBuffer+12, &now.date, 4);
      memcpy(transmitBuffer+16, &alerts,   2);
      memcpy(transmitBuffer+18, &shocks,   1);
      memcpy(transmitBuffer+19, &version,  1);
      radio.setHeaderFlags(0x0,0x0F);
      return phoneHome();

    }

  uint8_t power = 5;

  int  phoneHome(){
    
    // Send message to base station
    if(power > 5)
      power = power - 5;
    else 
      power = 5;

    radio.setTxPower(power); 

    memcpy(transmitBuffer+18, &power, 1);

    while(1){ 
      if (manager.sendtoWait(transmitBuffer, sizeof(transmitBuffer), BASE_STATION_ADDRESS)) 
      { 
        uint8_t len = sizeof(receiveBuffer); 
        uint8_t from; 
        uint8_t* bufPtr; 

        uint8_t last = 0, index = 0;

        SerialUSB.println("Sent");
        // Now wait for a reply from the base station 
        while (manager.recvfromAckTimeout(receiveBuffer, &len, 2000, &from)) 
        { 
          switch(radio.headerFlags())
          {
            case 0: //Location 
              break;
            case 1: //Fence
              SerialUSB.println("Fence Packet");                      
              memcpy(&fenceversion,             &receiveBuffer[0],  1);  //Version //Dest, Orig, Bytes
              memcpy(&last,                     &receiveBuffer[1],  1); //Last Y/N
              memcpy(&polyCorners,              &receiveBuffer[2],  1); //numPts
              memcpy(&index,                    &receiveBuffer[3],  1); 
              memcpy(&fencePoints[index].lat,   &receiveBuffer[4],  4);
              memcpy(&fencePoints[index].lon,   &receiveBuffer[8],  4);
              memcpy(&fencePoints[index+1].lat, &receiveBuffer[12], 4);
              memcpy(&fencePoints[index+1].lon, &receiveBuffer[16], 4);
              if(last) {
                //SerialUSB.print("Copy to Flash ");
                char flashbuffer[sizeof(fencePoints)+2];
                memcpy(&flashbuffer[0],   &polyCorners,     sizeof(polyCorners));
                memcpy(&flashbuffer[1],   &fenceversion,    sizeof(fenceversion));
                memcpy(&flashbuffer[2],   &fencePoints[0],  sizeof(fencePoints));
                SerialFlashFile flashFile = SerialFlash.open(file_fence);
                flashFile.erase();
                flashFile.write(flashbuffer, sizeof(flashbuffer));
                flashFile.close();
              }
              break;
                  
            case 2: //Settings
              //SerialUSB.println("Settings Packet");                   

              memcpy(&NODE_ADDRESS,   &receiveBuffer[0],  1);   //Dest, Orig, Bytes
              memcpy(&distThresh,     &receiveBuffer[1],  1);
              memcpy(&motionThresh,   &receiveBuffer[2],  1);
              memcpy(&testing,        &receiveBuffer[3],  1);
              memcpy(&magbias0,       &receiveBuffer[5],  2);
              memcpy(&magbias1,       &receiveBuffer[7],  2);
              memcpy(&magbias2,       &receiveBuffer[9],  2);
              
              setAddress(); //Updates the nodes address if it has been changed
              
              SerialFlashFile flashFile = SerialFlash.open(file_settings);
              flashFile.erase();
              flashFile.write(receiveBuffer, len);
              flashFile.close();

              SerialUSB.println("Updated Settings"); 
              break;
          }
        } 
        //SerialUSB.println("No More Replies"); 
        return 0;  //Sucess 
      } else {
        power = power+5;

        memcpy(transmitBuffer+18, &power, 1);

        if(power > 15){
          SerialUSB.println("sendtoWait failed");   
          return 1; //Fail
        } 
      }
    }
  }

  void setAddress(){
    radio.setHeaderFrom(NODE_ADDRESS);
    manager.setThisAddress(NODE_ADDRESS);
  }

};
#endif
