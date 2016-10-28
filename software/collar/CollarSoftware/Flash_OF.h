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
			// Check if the FENCE.bin file exists, Yes: Get fence from it, No: Make file and save defaults to it.
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

				char flashbuffer[2 + sizeof(fencePoints)];
				memcpy(&flashbuffer[0], &polyCorners, 	sizeof(polyCorners));
                memcpy(&flashbuffer[1],	&fenceversion, 	sizeof(fenceversion));
                memcpy(&flashbuffer[2], &fencePoints, 	sizeof(fencePoints));

    			SerialFlashFile flashFile = SerialFlash.open(file_fence);
                flashFile.write(flashbuffer, sizeof(flashbuffer));
                flashFile.close();
    		}
    		// Check if the SETTINGS.bin file exists, Yes: Get settings from it, No: Make file and save defaults to it.
			if(SerialFlash.exists(file_settings)){
    			SerialFlashFile file;
				file = SerialFlash.open(file_settings);
				if (file) {  // true if the file exists
					char flashbuffer[11];
					file.read(flashbuffer, sizeof(flashbuffer));
                    memcpy(&NODE_ADDRESS,   &flashbuffer[0],  1);   //Dest, Orig, Bytes
                    memcpy(&distThresh,     &flashbuffer[1],  1);
                    memcpy(&motionThresh,   &flashbuffer[2],  1);
                    memcpy(&testing,        &flashbuffer[3],  1);
                    memcpy(&magbias0,       &flashbuffer[5],  2);
                    memcpy(&magbias1,       &flashbuffer[7],  2);
                    memcpy(&magbias2,       &flashbuffer[9],  2);
				}
    		} else {
    			SerialFlash.createErasable(file_settings, 11);
    			char buf[11];
    			memcpy(&buf[0],	&NODE_ADDRESS, 	1);  
                memcpy(&buf[1], &distThresh,    1);
                memcpy(&buf[2], &motionThresh,  1);
                memcpy(&buf[3], &testing,    	1);
                memcpy(&buf[5], &magbias0,  	2);
                memcpy(&buf[7], &magbias1,    	2);
                memcpy(&buf[9], &magbias2,  	2);

                SerialFlashFile flashFile = SerialFlash.open(file_settings);
                flashFile.write(buf, 11);
                flashFile.close();
    		}
                    
    		// Make sure the Flash has finished all steps before returning.
    		while (SerialFlash.ready() == false) {
			}
    	}
    };

#endif