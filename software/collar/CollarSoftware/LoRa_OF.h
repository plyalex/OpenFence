#ifndef LoRa_OF_H
#define LoRa_OF_H
 
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include "Geofence.h"
#include "PinDefines.h"

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
  bool updated;
  uint8_t New_RF_ID;
  uint8_t distThresh;
  uint8_t motionThresh;
  int16_t magbias0;
  int16_t magbias1;
  int16_t magbias2;
  bool testing;
};

//Global Variables
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];  // ???
uint8_t buffer[sizeof(datapacket0)];


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
      radio.setTxPower(10); 

    }

    bool sleep(){
      return radio.sleep();
    }

    int sendPosition(struct position me, struct datetime now, uint16_t alerts, uint8_t shocks , uint8_t version) {
      struct datapacket0 d;

      d.lat=me.lat;
      d.lon=me.lon;
      d.gpstime=now.time;
      d.date=now.date;
      d.alerts=alerts;
      d.shocks=shocks;
      d.ver=version;

      memcpy(buffer+0, &d.lat, 4);
      memcpy(buffer+4, &d.lon, 4);
      memcpy(buffer+8, &d.gpstime, 4);
      memcpy(buffer+12, &d.date, 4);
      memcpy(buffer+16, &d.alerts, 2);
      memcpy(buffer+18, &d.shocks, 1);
      memcpy(buffer+19, &d.ver, 1);
      radio.setHeaderFlags(0x0,0x0F);
      return phoneHome();

    }

    int  phoneHome(){
      // Send message to base station 
      if (manager.sendtoWait(buffer, sizeof(buffer), BASE_STATION_ADDRESS)) 
      { 
        uint8_t len = sizeof(buf); 
        uint8_t from; 
        uint8_t* bufPtr; 

        uint8_t last = 0, index = 0;

        test=999;
        SerialUSB.println("Sent");
        
        // Now wait for a reply from the base station 
        if (manager.recvfromAckTimeout(buf, &len, 2000, &from)) 
        { 

            switch(radio.headerFlags())
            {
            case 0: //Location 
                    break;
            case 1: //Fence
                    SerialUSB.println("Fence Packet");                   
                    while(1){
                      
                      memcpy(&fenceversion,             &buf[0],  1);  //Version //Dest, Orig, Bytes
                      memcpy(&last,                     &buf[1],  1); //Last Y/N
                      memcpy(&polyCorners,              &buf[2],  1); //numPts
                      memcpy(&index,                    &buf[3],  1); 
                      memcpy(&fencePoints[index].lat,   &buf[4],  4);
                      memcpy(&fencePoints[index].lon,   &buf[8],  4);
                      memcpy(&fencePoints[index+1].lat, &buf[12], 4);
                      memcpy(&fencePoints[index+1].lon, &buf[16], 4);
                      SerialUSB.print(".");
                      if(last) break;
                      if(manager.recvfromAckTimeout(buf, &len, 2000, &from)){
                        if(radio.headerFlags() != 1) break;
                      
                      }else{
                        break;
                      }
                    }
                    SerialUSB.println("Received Fence points"); 
                    break;
                  
            case 2: //Settings
                    SerialUSB.println("Settings Packet");                   
                    memcpy(&NODE_ADDRESS,   &buffer[0],  1);   //Dest, Orig, Bytes
                    memcpy(&distThresh,     &buffer[1],  1);
                    memcpy(&motionThresh,   &buffer[2],  1);
                    memcpy(&magbias0,       &buffer[3],  2);
                    memcpy(&magbias1,       &buffer[5],  2);
                    memcpy(&magbias2,       &buffer[7],  2);
                    memcpy(&testing,        &buffer[9],  1);
                    SerialUSB.print(NODE_ADDRESS);
                    SerialUSB.println("Updated Settings"); 
                    break;
            }
        } 
        else 
        { 
          return 1; SerialUSB.println("Base station not responding!"); 
        } 
      } else 
      {
        return 1; SerialUSB.println("sendtoWait failed");  
      }
      return 0;
    }
};
#endif
