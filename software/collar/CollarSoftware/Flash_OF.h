#ifndef Flash_OF_H
#define Flash_OF_H
	
	#include "PinDefines.h"
	#include "LoRa_OF.h"
	#include <SerialFlash.h>
	
	char file_fence[]="FENCE.bin";
	char file_settings[]="SETTINGS.bin";

	class Flash_OF {
 
    protected:
 
    public:
    	void init(){
			// Start SerialFlash
			if (!SerialFlash.begin(FLASH_CS)) {
				while (1) {
				  SerialUSB.println ("Cannot access SPI Flash chip");
				  delay (1000);
				}
			}

    		if(SerialFlash.exists(file_fence)){
    			SerialFlashFile file;
    			SerialUSB.println("Fence File Exists");
				file = SerialFlash.open(file_fence);
					if (file) {  // true if the file exists
						char flashbuffer[2 + sizeof(fencePoints)];
						file.read(flashbuffer, sizeof(fencePoints));
						memcpy(&polyCorners, 	&flashbuffer[0], 	sizeof(polyCorners));
                        memcpy(&fenceversion, 	&flashbuffer[1],	sizeof(fenceversion));
                        memcpy(&fencePoints, 	&flashbuffer[2], 	sizeof(fencePoints));
                        SerialUSB.println("Fence Loaded from Flash");
                    }
    		} else {
    			SerialFlash.createErasable(file_fence, sizeof(fencePoints)+2);
    		}

			if(SerialFlash.exists(file_settings)){
    			SerialFlashFile file;
				file = SerialFlash.open(file_settings);
					if (file) {  // true if the file exists
						char flashbuffer[10];
						file.read(flashbuffer, sizeof(flashbuffer));
	                    memcpy(&NODE_ADDRESS,   &flashbuffer[0],  1);   //Dest, Orig, Bytes
	                    memcpy(&distThresh,     &flashbuffer[1],  1);
	                    memcpy(&motionThresh,   &flashbuffer[2],  1);
	                    memcpy(&testing,        &flashbuffer[3],  1);
	                    memcpy(&magbias0,       &flashbuffer[5],  2);
	                    memcpy(&magbias1,       &flashbuffer[7],  2);
	                    memcpy(&magbias2,       &flashbuffer[9],  2);

	                    SerialUSB.println(flashbuffer[0],BIN);
	                    SerialUSB.println(flashbuffer[1],BIN);
	                    SerialUSB.println(flashbuffer[2],BIN);
	                    SerialUSB.println(flashbuffer[3],BIN);
	                    SerialUSB.println(flashbuffer[4],BIN);
	                    SerialUSB.println(flashbuffer[5],BIN);
	                    SerialUSB.println(flashbuffer[6],BIN);
	                    SerialUSB.println(flashbuffer[7],BIN);
	                    SerialUSB.println(flashbuffer[8],BIN);
	                    SerialUSB.println(flashbuffer[9],BIN);
					}
    		} else {
    			SerialFlash.createErasable(file_settings, 10);
    		}
                    



    		while (SerialFlash.ready() == false) {
			}
    	}

    };


#endif