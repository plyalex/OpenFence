

#ifndef PA6C_h
#define PA6C_h

#include "Arduino.h"

#define BUFFSIZ 90 // plenty big


class PA6C
{
	public:
		PA6C(void);
		void initGPS(void);
		void standbyGPS(void);
		void getGPRMC(void);
		float getLatitude(void);
		float getLongitude(void);
		int getTime(void);
		int getDate(void);


	private:
		uint32_t parsedecimal(char *str);
		void readLine(void);

		// global variables
		static char bufferPA6C[BUFFSIZ];        // string buffer for the sentence
		static char buffidx;                // an indexer into the buffer

};

//Add ability to change refresh rate etc.



#endif

