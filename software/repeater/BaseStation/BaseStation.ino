// ***** INCLUDES *****
#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

// ***** CONSTANTS *****
#define MOBILE_NODE_ADDRESS 128
#define BASE_STATION_ADDRESS 1

// ***** PIN DEFINITION *****
// ***** MINI ULTRA PRO CONFIGURATIONS *****
const int radioInterruptPin = 2;  // D2 is used as radio interrupt pin
const int flashChipSelectPin = 4; // D4 is used as serial flash chip select pin
const int radioChipSelectPin = 5; // D5 is used as radio chip select pin
const int ledPin = 13;            // D13 has an LED 

// Singleton instance of the radio driver
RH_RF95 radio(radioChipSelectPin, radioInterruptPin);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(radio, BASE_STATION_ADDRESS);

void setup() 
{
  // Ensure serial flash is not interfering with radio communication on SPI bus
  pinMode(flashChipSelectPin, OUTPUT);
  digitalWrite(flashChipSelectPin, HIGH);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
    
  SerialUSB.begin(115200);
  SerialUSB.println("Testing");
  digitalWrite(ledPin, HIGH);
  // If radio initialization fail
  if (!manager.init())
  {
    SerialUSB.println("Init failed");
  }
  digitalWrite(ledPin, LOW);

  // FSK, Rb = 2.4 kbps, Fd = 4.8 kHz 915 MHz, 20 dBm on RFM69HCW
  radio.setModemConfig(RH_RF95::Bw500Cr45Sf128 );
  radio.setFrequency(915.0);
  radio.setTxPower(10);
}


uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t counter = 1;


uint8_t buffer[20];

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

struct position{
  float lat; //x
  float lon; //y
};

int polyCorners = 0;
position fencePoints[255];
uint8_t fenceversion;



void loop()
{
  struct datapacket0 d0;
  struct datapacket1 d1[128];

  d1[0].ver=1;
  d1[0].last=0;
  d1[0].numPts=3;
  d1[0].X=0;
  d1[0].lat0=101.101101;  
  d1[0].lon0=-37.911625; 
  d1[0].lat1=145.138707;
  d1[0].lon1=-37.911662;
  d1[1].ver=1;
  d1[1].last=1;
  d1[1].numPts=3;
  d1[1].X=2;
  d1[1].lat0=145.138653;
  d1[1].lon0=-37.911887;
  d1[1].lat1=0;
  d1[1].lon1=0;

  char c; 
  uint8_t flag;
  uint8_t index;

  if(SerialUSB.available()){

    flag = SerialUSB.read();
    //if(c >= '0' & c <= '9')
    //  flag = c - '0';


    //c = SerialUSB.read();
    //if (c != ',') SerialUSB.println("Unexpected Input");

    switch(flag){
      case 1: for(int j=0; j<20; j++){
                buffer[j]=SerialUSB.read();
              }
              memcpy(&index,          &buffer[3],  1);
              index=index>>1; //Divide by two
              if(index==0) memset(&d1[0], 0, sizeof(d1));  //If new set of fence points, remove the old ones.

              memcpy(&d1[index].ver,       &buffer[0],  1);   //Dest, Orig, Bytes
              memcpy(&d1[index].last,      &buffer[1],  1);
              memcpy(&d1[index].numPts,    &buffer[2],  1);
              memcpy(&d1[index].X,         &buffer[3],  1);
              memcpy(&d1[index].lat0,      &buffer[4],  4);
              memcpy(&d1[index].lon0,      &buffer[8],  4);
              memcpy(&d1[index].lat1,      &buffer[12], 4);
              memcpy(&d1[index].lon1,      &buffer[16], 4);
              // Make a new design for sending from python to arduino

              break;

    }
    
}


  // Message available from mobile node?
  if (manager.available())
  {
    uint8_t len = sizeof(buf);
    uint8_t from;
    uint8_t* bufPtr;
    
    // Now wait for a reply from the mobile node
    if (manager.recvfromAck(buf, &len, &from))
    {
      uint8_t flagfromNode = radio.headerFlags();
      switch(flagfromNode){
        case 0: 
                memcpy(&d0.lat,      &buf[0],  4);   //Dest, Orig, Bytes
                memcpy(&d0.lon,      &buf[4],  4);
                memcpy(&d0.gpstime,  &buf[8],  4);
                memcpy(&d0.date,     &buf[12], 4);
                memcpy(&d0.alerts,   &buf[16], 2);
                memcpy(&d0.shocks,   &buf[18], 1);
                memcpy(&d0.ver,      &buf[19], 1);


                SerialUSB.write((uint8_t *)&flagfromNode,1);
                SerialUSB.write((uint8_t *)&from,1);
                SerialUSB.write((uint8_t *)&d0,20);
                SerialUSB.println();
                //Human Readable
                // SerialUSB.print(from); SerialUSB.print(","); SerialUSB.print(radio.headerFlags());SerialUSB.print(",");
                // SerialUSB.print(d0.lat,6); SerialUSB.print(d0.lon,6); SerialUSB.print(","); SerialUSB.print(d0.gpstime);
                // SerialUSB.print(","); SerialUSB.print(d0.date); SerialUSB.print(","); SerialUSB.print(d0.alerts);
                // SerialUSB.print(","); SerialUSB.print(d0.shocks); SerialUSB.print(","); SerialUSB.print(d0.ver);
                break;
        case 1:
                break;

      }
      digitalWrite(ledPin, HIGH);  

      //
      digitalWrite(ledPin, LOW);

      // Send message to mobile node
      if(d0.ver != 0){//d1[0].ver){    //Not up to date with current fence, upload to node
        int toSend = (d1[0].numPts >> 1) + 1;
        uint8_t size =sizeof(datapacket1);
        uint8_t bufferLoRa[size];
        radio.setHeaderFlags(0x1,0x0F);

        for(int i=0; i<toSend; i++){
          memcpy(&bufferLoRa, &d1[i], size);   //Dest, Orig, Bytes
          if (!manager.sendtoWait(bufferLoRa, size, from))
            SerialUSB.println("sendtoWait failed");
        }
        
          

      }
      
    }
  }
}



uint32_t parsedecimal(char *str){
  uint32_t d = 0;
  
  while (str[0] != 0) {
   if ((str[0] > '9') || (str[0] < '0'))
     return d;
   d *= 10;
   d += str[0] - '0';
   str++;
  }
  return d;
}


    // flag = SerialUSB.read();
    // //if(c >= '0' & c <= '9')
    // //  flag = c - '0';


    // //c = SerialUSB.read();
    // //if (c != ',') SerialUSB.println("Unexpected Input");

    // switch(flag){
    //   case 1: for(int j=0;){
    //             c = SerialUSB.read();
    //             if(c >= '0' & c <= '9')
    //                   serialDataIn += c;
    //             if (c=='\n')
    //               continue;
    //             if ((buffidx == BUFFSIZ-1) || (c=='\r')){
    //               buffer[buffidx]=0;
    //               break;
    //             }
    //             buffer[buffidx++]=c;
    //           }
    //           memcpy(&d1[].ver,       &buffer[0],  1);   //Dest, Orig, Bytes
    //           memcpy(&d1[].last,      &buffer[1],  1);
    //           memcpy(&d1[].numPts,    &buffer[2],  1);
    //           memcpy(&d1[].X,         &buffer[3],  1);
    //           memcpy(&d1[].lat0,      &buffer[4],  4);
    //           memcpy(&d1[].lon0,      &buffer[8],  4);
    //           memcpy(&d1[].lat1,      &buffer[12], 4);
    //           memcpy(&d1[].lon1,      &buffer[16], 4);

    //           commaIndex[0] = buffer.indexOf(',');
    //           subString[0] = myString.substring(0, commaIndex[0]);

    //           for(int i=1; i<8; i++){
    //             commaIndex[i] = buffer.indexOf(',', commaIndex[i-1]);
    //             subString[i] = myString.substring(commaIndex[i-1], commaIndex[i]);
    //           }

    //           break;

    // }
