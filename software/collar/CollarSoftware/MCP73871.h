#ifndef MCP73871_h
#define MCP73871_h

#include "Arduino.h"
#include "PinDefines.h"


class MCP73871
{
	public:
		MCP73871(void);
		uint8_t getStatus(void);
		void printStatus(void);


	private:

};

#endif
