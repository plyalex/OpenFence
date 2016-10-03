#ifndef MCP73871_h
#define MCP73871_h

#include "Arduino.h"
#include "PinDefines.h"


class MCP73871
{
	public:
		MCP73871(){
			;;
		}

		uint8_t getStatus(){
			uint8_t var=0;
			var += digitalRead(CHARGE_S1)<<2;
		  	var += digitalRead(CHARGE_S2)<<1;
		  	var += digitalRead(CHARGE_PG);
		  	return var;
		}

		void printStatus(){
			uint8_t var= getStatus();

			switch(var){
				case 0: SerialUSB.println("Temp/Timer Fault"); break;
				case 2: SerialUSB.println("Constant Volt/Curr/Precond"); break;
				case 3: SerialUSB.println("Low Battery Output"); break;
				case 4: SerialUSB.println("Charge Complete"); break;
				case 6: SerialUSB.println("No Battery Present"); break;
				case 7: SerialUSB.println("No Input Power"); break;
			}


		}


	private:

};

#endif
