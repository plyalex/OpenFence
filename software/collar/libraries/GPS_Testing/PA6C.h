

#ifndef PA6C_h
#define PA6C_h

#include "Arduino.h"

#define BUFFSIZ 90 // plenty big


class PA6C
{
	public:
		PA6C(void);
		void getGPRMC(void);
		double getLatitude(void);
		double getLongitude(void);
		int getTime(void);
		int getDate(void);


	private:

		uint32_t parsedecimal(char *str);
		void readLine(void);
		int parseData();

		// global variables
		static char buffer[BUFFSIZ];        // string buffer for the sentence
		static char buffidx;                // an indexer into the buffer

};


#endif

