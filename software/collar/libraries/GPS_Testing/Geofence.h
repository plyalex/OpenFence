

#ifndef Geofence_h
#define Geofence_h

#include "Arduino.h"

struct position{
  float lat; //x
  float lon; //y
};

class Geofence 
{
	public:
		Geofence(void);
		float distance(struct position v, struct position w);
		float pointInPolygon(struct position p, struct position points[], int polyCorners);
		

	private:
		float degrees2radians(float degrees);
		float sqr(float x);
		float distBehind(struct position p, struct position w, struct position v);
		float dist2segment(struct position p, struct position v, struct position w);

};


#endif
